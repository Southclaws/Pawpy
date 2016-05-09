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

	Note:
		This file contains the actual Python calling code, thread code and all
		other features specific to the plugin.


==============================================================================*/


#include <string>
#include <vector>
#include <stack>
#include <thread>
#include <mutex>

using std::string;
using std::vector;
using std::stack;
using std::thread;
using std::mutex;

#include "main.hpp"
#include "python_meta.hpp"

#include "pawpy.hpp"
#include <sdk.hpp>


/*
	Note:
	Contains a list of "pycall_t" objects that have finished processing. When a
	thread has finished running a Python script, it will store the return value
	inside the corresponding pycall_t object it is associated with then store
	that object on this stack. When the AMX calls ProcessTick, it will process
	whatever pycall_t objects are stored on it. This is the bread and butter of
	thread-safe SA:MP plugins.
*/
stack<Pawpy::pycall_t> Pawpy::call_stack;

/*
	Note:
	This mutex protects the call_stack from race conditions. Since each Python
	script runs in a thread, two scripts could finished at the same time and try
	to write their processed pycall_t objects into the stack. This is standard
	when working with threads, if you are unaware of mutexes and race conditions
	please go read about these topics before touching threaded applications in
	any language!
*/
mutex Pawpy::call_stack_mutex;


/*
	Note:
	Prepares a pycall_t object to be called on the main thread.
*/
int Pawpy::run_python(string module, string function, string callback, vector<string> arguments)
{
	debug("run_python: %s, %s, %s", module.c_str(), function.c_str(), callback.c_str());

	pycall_t call;

	call.module = module;
	call.function = function;
	call.callback = callback;
	call.arguments = arguments;

	run_call(call);

	return 0;
}

/*
	Note:
	Prepares a pycall_t object and creates a thread to run it.
*/
int Pawpy::run_python_threaded(string module, string function, string callback, vector<string> arguments)
{
	debug("run_python_threaded: %s, %s, %s", module.c_str(), function.c_str(), callback.c_str());

	pycall_t call;
	thread* t = nullptr;

	call.module = module;
	call.function = function;
	call.callback = callback;
	call.arguments = arguments;

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

/*
	Note:
	This function runs inside a thread created from run_python_threaded. It
	calls the Python main calling function and processes the result. When the
	result is ready, it locks the call_stack and pushes the pycall_t object
	ready for the next ProcessTick to call into the AMX with the result.
*/
void Pawpy::run_call_thread(pycall_t pycall)
{
	debug("run_call_thread: %s, %s, %s", pycall.module.c_str(), pycall.function.c_str(), pycall.callback.c_str());

	char* result_val = run_call(pycall);

	pycall.returns = _strdup(result_val);

	std::lock_guard<std::mutex> lock(call_stack_mutex);
	call_stack.push(pycall);
}

/*
	Note:
	This function takes a pycall_t object and runs the actual Python module it
	specifies. It returns the result as a long but this will probably change.
	The code is quite daunting and most of it is converting and validating types
	from C to Python.
*/
char* Pawpy::run_call(pycall_t pycall)
{
	debug("run_call: %s, %s, %s", pycall.module.c_str(), pycall.function.c_str(), pycall.callback.c_str());

	PyGILState_STATE gstate = PyGILState_Ensure();
	debug("run_call: locked GIL state");

	PyObject* name_ptr = PyUnicode_FromString(pycall.module.c_str());

	if(name_ptr == nullptr)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to convert module name to PyUnicode object.");
		return 0;
	}

	/*
		Note:
		This gets the "cwd" (current working directory) and appends it to the
		Python module search path (sys.path) so that .py files in the SA:MP
		server directory are found. Because of Python's module abstraction,
		scripts in subdirectories are specified by . as the directory delimiter
		instead of a / character. I was going to hard-code a ./scripts/
		directory since most users would probably want their scripts organised
		in some way but I'll leave that up to users to decide.
	*/
	char* cwd;

	cwd = _getcwd(NULL, 0);

	PyObject* sysPath = PySys_GetObject((char*)"path");
	PyObject* programName = PyUnicode_FromString(cwd);
	PyList_Append(sysPath, programName);
	Py_DECREF(programName);

	free(cwd);

	/*
		Note:
		Imports the module specified by pycall into the interpreter.
	*/
	PyObject* module_ptr = PyImport_Import(name_ptr);

	if(module_ptr == nullptr)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to load module: '%s'", pycall.module.c_str());
        return 0;
    }

	Py_DECREF(name_ptr);
	debug("run_call: imported module '%s'", pycall.module.c_str());

	/*
		Note:
		Checks if the function specified in pycall exists in the loaded module.
	*/
	name_ptr = PyUnicode_FromString(pycall.function.c_str());

	if(PyObject_HasAttr(module_ptr, name_ptr) == NULL)
	{
        samp_pyerr();
        samp_printf("ERROR: Module has no attribute: '%s'", pycall.function.c_str());
        return 0;
	}

	Py_DECREF(name_ptr);
	debug("run_call: scanned for attribute '%s'", pycall.function.c_str());

	/*
		Note:
		Loads the function specified in pycall into a PyObject ready to call.
	*/
	PyObject* func_ptr = nullptr;

	func_ptr = PyObject_GetAttr(module_ptr, name_ptr);

	if(func_ptr == nullptr)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to convert function name to function object: '%s'", pycall.function.c_str());
        return 0;
	}

	/*
		Note:
		Checks if this is a "callable" object. Everything is an object in Python
		so the "attribute" we loaded earlier could actually be a class or a
		variable so this check ensures it's something we can "call".
	*/
	if(!PyCallable_Check(func_ptr))
	{
        samp_pyerr();
        samp_printf("ERROR: Function not found or is not callable: '%s'", pycall.function.c_str());
        return 0;
	}

	debug("run_call: checked for function existence and callability '%s'", pycall.function.c_str());

	/*
		Note:
		This tuple contains the arguments we're passing to the Python function.
	*/
	PyObject* args_ptr = PyTuple_New(pycall.arguments.size());

	if(args_ptr == nullptr)
	{
        samp_pyerr();
        samp_printf("ERROR: Failed to create new PyTuple object.");
        return 0;
	}

	PyObject* arg_string_ptr;

	for(unsigned int i = 0; i < pycall.arguments.size(); ++i)
	{
		arg_string_ptr = PyUnicode_FromString(pycall.arguments[i].c_str());
		PyTuple_SET_ITEM(args_ptr, i, arg_string_ptr);
	}

	debug("run_call: created argument tuple");

	/*
		Todo: process arguments of various types from Pawn call.

		Loop args, PyLong_FromLong/String/Array then PyTuple_SetItem, etc.
	*/

	debug("run_call: calling into Python module '%s' at function '%s'", pycall.module.c_str(), pycall.function.c_str());

	/*
		Note:
		The actual function call. The function and arguments that were prepared
		and validated earlier are passed in and everything should run smoothly.
	*/
	PyObject* result_ptr = PyObject_CallObject(func_ptr, args_ptr);
	Py_DECREF(args_ptr);

	debug("run_call: finished running Python module");

	if(result_ptr == nullptr)
	{
		samp_pyerr();
		samp_printf("ERROR: Python function call result is null.");
        return 0;
	}

	/*
		Note:
		Gets the return value of the result from the call. Return value must be
		a string, simply because type conversion is easier when there's only one
		type to deal with.
	*/
	PyObject* result_str_ptr = PyUnicode_AsASCIIString(result_ptr);
	if(result_str_ptr == nullptr)
	{
		samp_pyerr();
		samp_printf("ERROR: Python function call result is not a string.");
        return 0;
	}

	char* result_str_char;// = PyByteArray_AsString(result_str_ptr);
	Py_ssize_t result_str_len;
	PyBytes_AsStringAndSize(result_str_ptr, &result_str_char, &result_str_len);
	Py_DECREF(result_str_ptr);

	if(result_str_char == nullptr)
	{
		samp_pyerr();
		samp_printf("ERROR: result_str_char is null.");
        return 0;
	}

	debug("run_call: optained module result value '%s' and returning", result_str_char);

	PyGILState_Release(gstate);
	debug("run_call: released GIL state");

	return result_str_char;
}

/*
	Note:
	This is a ProcessTick function called for each AMX instance (see main.cpp)
	The call_stack is checked for contents; when call_stack contains objects,
	they are looped and processed in the order they were pushed. For each pycall
	the AMX code searches for the public callback function, pushes the result
	stored in the pycall object from the end of run_call onto the parameters and
	calls the function in Pawn, the circle is complete!
*/
void Pawpy::amx_tick(AMX* amx)
{
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

			/*
				Note:
				Callback parameters are pushed in reverse order. So in this case
				call.returns is the last parameter in the Pawn native, but here
				it is pushed first.
				The callback parameter format is: (string[], len)
			*/
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
}
