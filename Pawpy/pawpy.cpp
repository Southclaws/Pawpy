/*==============================================================================

	Pawpy - Python Utility for Pawn
	Southclaw
	2016

==============================================================================*/


#include <string>
#include <thread>

using std::thread;

#include "main.hpp"
#include "python_meta.hpp"

#include "pawpy.hpp"
#include <sdk.hpp>


stack<Pawpy::pycall_t> Pawpy::call_stack;


int Pawpy::thread_call(string module, string function, string callback)
{
	pycall_t call;
	thread* t = nullptr;

	call.module = module;
	call.function = function;
	call.callback = callback;

	t = new thread(run_call, call);

	if(t == nullptr)
	{
		// raise
		return 1;
	}

	call.threadid = t->get_id();

	t->detach();

	delete t;

	return 0;
}

void Pawpy::run_call(pycall_t pycall)
{
	PyObject* name_ptr = PyUnicode_FromString(pycall.module.c_str());

	if(name_ptr == nullptr)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to convert module name to PyUnicode object.");
		return;
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
        return;
    }

	Py_DECREF(name_ptr);

	name_ptr = PyUnicode_FromString(pycall.function.c_str());

	if(PyObject_HasAttr(module_ptr, name_ptr) != NULL)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to convert function name to function object: '%s'", pycall.function.c_str());
        return;
	}

	Py_DECREF(name_ptr);

	PyObject* func_ptr = nullptr;
	
	func_ptr = PyObject_GetAttr(module_ptr, name_ptr);

	if(func_ptr == nullptr)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to convert function name to function object: '%s'", pycall.function.c_str());
        return;
	}

	if(!PyCallable_Check(func_ptr))
	{
        samp_pyerr();
        samp_printf("ERROR: Function not found or is not callable: '%s'", pycall.function.c_str());
        return;
	}

	PyObject* args_ptr = PyTuple_New(0);

	if(args_ptr == nullptr)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to create new PyTuple object.");
        return;
	}

	/*
		Todo: process arguments of various types from Pawn call.

		Loop args, PyLong_FromLong/String/Array then PyTuple_SetItem, etc.
	*/

	samp_printf("PyObject_CallObject");
	PyObject* result_ptr = PyObject_CallObject(func_ptr, args_ptr);
	Py_DECREF(args_ptr);

	samp_printf("PyObject_CallObject after");
	if(result_ptr == nullptr)
	{
		samp_pyerr();
		samp_printf("ERROR: Python function call failed.");
	}

	long result_val = PyLong_AsLong(result_ptr);
	char result_str[24];
	samp_printf("result: %d", result_val);

	_ltoa_s(result_val, result_str, 10);

		samp_printf("return: '%s'", result_str);

	pycall.returns = _strdup(result_str);
	call_stack.push(pycall);

	return;
}

void Pawpy::amx_tick(AMX* amx)
{
	if(call_stack.empty())
		return;

	samp_printf("amx %x", amx);

	Pawpy::pycall_t call;
	int error = 0;
	int amx_idx = -1;
	cell amx_addr;
	cell amx_ret;
	cell *phys_addr; 

	while(!Pawpy::call_stack.empty())
	{
		call = Pawpy::call_stack.top();

		samp_printf("call: '%s'", call.callback.c_str());

		error = amx_FindPublic(amx, call.callback.c_str(), &amx_idx);

		if(error == AMX_ERR_NONE)
		{
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

			samp_printf("return value: %d", amx_ret);
		}
		else
		{
			samp_printf("ERROR: amx_FindPublic returned %d.", error);
		}

		Pawpy::call_stack.pop();
	}
}
