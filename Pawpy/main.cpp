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


// globals
#include "main.hpp"
#include "python_meta.hpp"

// platform includes
#include <set>

using std::set;

// sdk related
#include <sdk.hpp>

// project related
#include "natives.hpp"
#include "pawpy.hpp"


/*==============================================================================

	Load/Unload and AMX management

==============================================================================*/


extern void	*pAMXFunctions;
logprintf_t logprintf;

set<AMX*> amx_list;


PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) 
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	
	Py_SetProgramName(L"Pawpy");
	Py_Initialize();
	PyEval_InitThreads();
	PyEval_ReleaseLock();

	samp_printf("\n");
	samp_printf("Pawpy - Python utility for Pawn by Southclaw");
	samp_printf("\n");

	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() 
{
	Py_Finalize();

	samp_printf("Pawpy unloaded.");
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	for(auto i : amx_list)
	{
		Pawpy::amx_tick(i);
	}
}

void samp_printf(const char* message, ...)
{
	unsigned int len = 256;
	char* result = new char[len];

	va_list args;
	va_start(args, message);

	vsprintf_s(result, len, message, args);
	logprintf(result);

	va_end(args);
}

void samp_pyerr()
{
	PyObject* type;
	PyObject* value;
	PyObject* trace;
	PyObject* result;
	char* ctype = nullptr;
	char* cvalue = nullptr;
	char* ctrace = nullptr;

	PyErr_Fetch(&type, &value, &trace);

	if(type != nullptr)
	{
		result = PyUnicode_AsEncodedString(type, "ASCII", "strict");

		if(result != nullptr)
		{
			ctype = PyBytes_AS_STRING(result);
			ctype = _strdup(ctype);
			Py_DECREF(result);
		}
	}

	if(value != nullptr)
	{
		result = PyUnicode_AsEncodedString(value, "ASCII", "strict");

		if(result != nullptr)
		{
			cvalue = PyBytes_AS_STRING(result);
			cvalue = _strdup(cvalue);
			Py_DECREF(result);
		}
	}

	if(trace != nullptr)
	{
		result = PyUnicode_AsEncodedString(trace, "ASCII", "strict");

		if(result != nullptr)
		{
			ctrace = PyBytes_AS_STRING(result);
			ctrace = _strdup(ctrace);
			Py_DECREF(result);
		}
	}

	samp_printf("Python error report:");
	samp_printf(ctype == nullptr ? "no ctype" : ctype);
	samp_printf(cvalue == nullptr ? "no cvalue" : cvalue);
	samp_printf(ctrace == nullptr ? "no ctrace" : ctrace);
}


/*==============================================================================

	Export stuff

==============================================================================*/


extern "C" const AMX_NATIVE_INFO native_list[] = 
{
	{"RunPython", Native::RunPython},
	{"RunPythonThreaded", Native::RunPythonThreaded},
	{NULL, NULL}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{
	amx_list.insert(amx);
	return amx_Register(amx, native_list, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	amx_list.erase(amx);
	return AMX_ERR_NONE;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK; 
}
