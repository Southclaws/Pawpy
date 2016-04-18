/*==============================================================================


	Pawpy - Python Utility for Pawn

		Copyright (C) 2016 Barnaby "Southclaw" Keene

		This program is free software: you can redistribute it and/or modify it
		under the terms of the GNU General Public License as published by the
		Free Software Foundation, either version 3 of the License, or (at your
		option) any later version.

		This program is distributed in the hope that it will be useful, but
		WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
		See the GNU General Public License for more details.

		You should have received a copy of the GNU General Public License along
		with this program.  If not, see <http://www.gnu.org/licenses/>.


==============================================================================*/


#include <string>
#include <stack>
#include <thread>
#include <mutex>

using std::string;
using std::stack;
using std::thread;
using std::mutex;

#include "main.hpp"
#include "python_meta.hpp"

#include "pawpy.hpp"
#include <sdk.hpp>


stack<Pawpy::pycall_t> Pawpy::call_stack;
mutex Pawpy::call_stack_mutex;


int Pawpy::run_python(string module, string function, string callback)
{
	debug("run_python: %s, %s, %s", module.c_str(), function.c_str(), callback.c_str());

	pycall_t call;

	call.module = module;
	call.function = function;
	call.callback = callback;

	return run_call(call);
}

int Pawpy::run_python_threaded(string module, string function, string callback)
{
	debug("run_python_threaded: %s, %s, %s", module.c_str(), function.c_str(), callback.c_str());

	pycall_t call;
	thread* t = nullptr;

	call.module = module;
	call.function = function;
	call.callback = callback;

	debug("run_python_threaded: creating thread");
	t = new thread(run_call_thread, call);

	if(t == nullptr)
	{
		samp_printf("ERROR: Unable to create thread for Python call.");
		return 1;
	}

	call.threadid = t->get_id();

	t->detach();

	delete t;

	return 0;
}

void Pawpy::run_call_thread(pycall_t pycall)
{
	debug("run_call_thread: %s, %s, %s", pycall.module.c_str(), pycall.function.c_str(), pycall.callback.c_str());

	PyGILState_STATE gstate = PyGILState_Ensure();

	debug("run_call_thread: calling run_call");
	long result_val = run_call(pycall);

	PyGILState_Release(gstate);

	char result_str[24];

	_ltoa_s(result_val, result_str, 10);

	pycall.returns = _strdup(result_str);

	call_stack_mutex.lock();
	call_stack.push(pycall);
	call_stack_mutex.unlock();
}

long Pawpy::run_call(pycall_t pycall)
{
	debug("run_call: %s, %s, %s", pycall.module.c_str(), pycall.function.c_str(), pycall.callback.c_str());

	PyObject* name_ptr = PyUnicode_FromString(pycall.module.c_str());

	if(name_ptr == nullptr)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to convert module name to PyUnicode object.");
		return 0;
	}

	char* cwd;

	cwd = _getcwd(NULL, 0);

	PyObject* sysPath = PySys_GetObject((char*)"path");
	PyObject* programName = PyUnicode_FromString(cwd);
	PyList_Append(sysPath, programName);
	Py_DECREF(programName);

	free(cwd);

	PyObject* module_ptr = PyImport_Import(name_ptr);

	if(module_ptr == nullptr)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to load module: '%s'", pycall.module.c_str());
        return 0;
    }

	Py_DECREF(name_ptr);
	debug("run_call: imported module '%s'", pycall.module.c_str());

	name_ptr = PyUnicode_FromString(pycall.function.c_str());

	if(PyObject_HasAttr(module_ptr, name_ptr) == NULL)
	{
        samp_pyerr();
        samp_printf("ERROR: Module has no attribute: '%s'", pycall.function.c_str());
        return 0;
	}

	Py_DECREF(name_ptr);
	debug("run_call: scanned for attribute '%s'", pycall.function.c_str());

	PyObject* func_ptr = nullptr;

	func_ptr = PyObject_GetAttr(module_ptr, name_ptr);

	if(func_ptr == nullptr)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to convert function name to function object: '%s'", pycall.function.c_str());
        return 0;
	}

	if(!PyCallable_Check(func_ptr))
	{
        samp_pyerr();
        samp_printf("ERROR: Function not found or is not callable: '%s'", pycall.function.c_str());
        return 0;
	}

	debug("run_call: checked for function existence and callability '%s'", pycall.function.c_str());

	PyObject* args_ptr = PyTuple_New(0);

	if(args_ptr == nullptr)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to create new PyTuple object.");
        return 0;
	}

	debug("run_call: created argument tuple");

	/*
		Todo: process arguments of various types from Pawn call.

		Loop args, PyLong_FromLong/String/Array then PyTuple_SetItem, etc.
	*/

	debug("run_call: calling into Python module '%s' at function '%s'", pycall.module.c_str(), pycall.function.c_str());

	PyObject* result_ptr = PyObject_CallObject(func_ptr, args_ptr);
	Py_DECREF(args_ptr);

	debug("run_call: finished running Python module");

	if(result_ptr == nullptr)
	{
		samp_pyerr();
		samp_printf("ERROR: Python function call failed.");
	}

	long result_val = PyLong_AsLong(result_ptr);
	debug("run_call: optained module result value '%d' and returning", result_val);

	return result_val;
}

void Pawpy::amx_tick(AMX* amx)
{
	if(call_stack_mutex.try_lock() == false)
		return;

	if(call_stack.empty())
		return;

	Pawpy::pycall_t call;
	int error = 0;
	int amx_idx = -1;
	cell amx_addr;
	cell amx_ret;
	cell *phys_addr; 

	while(!Pawpy::call_stack.empty())
	{
		call = Pawpy::call_stack.top();

		error = amx_FindPublic(amx, call.callback.c_str(), &amx_idx);

		if(error == AMX_ERR_NONE)
		{
			debug("amx_tick callback: %s, %s, %s", call.module.c_str(), call.function.c_str(), call.callback.c_str());

			// callback format: (string[], len)
			amx_Push(amx, strlen(call.returns));
			amx_PushString(amx, &amx_addr, &phys_addr, call.returns, 0, 0);
			amx_PushString(amx, &amx_addr, &phys_addr, call.module.c_str(), 0, 0);

			amx_Exec(amx, &amx_ret, amx_idx);
			amx_Release(amx, amx_addr);

			switch(amx_ret)
			{
			case 0:
				break;
			case 1:
				continue;
			}
		}
		else
		{
			samp_printf("ERROR: amx_FindPublic returned %d.", error);
		}

		Pawpy::call_stack.pop();
	}

	call_stack_mutex.unlock();
}
