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

using std::string;

#include "natives.hpp"
#include "pawpy.hpp"


cell AMX_NATIVE_CALL Native::RunPython(AMX * amx, cell * params)
{
	debug("Native::RunPython called");

	string
		module,
		function,
		callback;

	module = amx_GetCppString(amx, params[1]);
	function = amx_GetCppString(amx, params[2]);
	callback = "";

	Pawpy::run_python(module, function, callback);

	return 0;
}

cell AMX_NATIVE_CALL Native::RunPythonThreaded(AMX * amx, cell * params)
{
	debug("Native::RunPythonThreaded called");

	string
		module,
		function,
		callback;

	module = amx_GetCppString(amx, params[1]);
	function = amx_GetCppString(amx, params[2]);
	callback = amx_GetCppString(amx, params[3]);
	debug("Native::RunPythonThreaded optained parameters");

	Pawpy::run_python_threaded(module, function, callback);
	debug("Native::RunPythonThreaded finished");

	return 0;
}
