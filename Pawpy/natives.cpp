/*==============================================================================

	Pawpy - Python Utility for Pawn
	Southclaw
	2016

==============================================================================*/


#include <string>

using std::string;

#include "natives.hpp"
#include "pawpy.hpp"


cell AMX_NATIVE_CALL Native::PawpyExec(AMX * amx, cell * params)
{
	string
		module,
		callback;

	module = amx_GetCppString(amx, params[1]);
	callback = amx_GetCppString(amx, params[2]);

	Pawpy::execCall(module, callback);

	return 0;
}
