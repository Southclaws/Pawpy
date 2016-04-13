/*==============================================================================

	Pawpy - Python Utility for Pawn
	Southclaw
	2016

==============================================================================*/


// globals
#include "main.hpp"

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
	
	samp_printf("\n");
	samp_printf("Pawpy - Python utility for Pawn by Southclaw");
	samp_printf("\n");

	return true;
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

PLUGIN_EXPORT void PLUGIN_CALL Unload() 
{
	samp_printf("Pawpy unloaded.");
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	for(auto i : amx_list)
	{
		Pawpy::amxProcessTick(i);
	}
}


/*==============================================================================

	Export stuff

==============================================================================*/


extern "C" const AMX_NATIVE_INFO native_list[] = 
{
	{"PawpyExec", Native::PawpyExec},
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
