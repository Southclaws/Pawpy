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
		function,
		callback;

	module = amx_GetCppString(amx, params[1]);
	function = amx_GetCppString(amx, params[2]);
	callback = amx_GetCppString(amx, params[3]);

	Pawpy::thread_call(module, function, callback);

	return 0;
}
