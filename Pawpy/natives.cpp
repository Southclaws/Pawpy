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
		This file contains the actual definitions of the functions exported to
		the SA:MP server. When those exported functions are called from a Pawn
		script (AMX instance actually) this is the code that will run. I've done
		what most logical programmers would do and separated my actual
		implementation code into another module so all these functions do is
		process the input and call the implementation functions with the
		sanitised and processed input.

		Talking about input, parameters come in from Pawn as a simple pointer
		which must be "decoded" into the original intended parameters and their
		types (as much as Pawn has "types"...) which isn't much different to
		extracting parameters from **argv if you're familiar with that.

		Luckily, the SDK has provided some helper functions to extract the data
		from parameters. It's important the "native" declarations in Pawn match
		the parameter extraction perfectly, if you specify too little parameters
		in a "native" declaration, the code here will extract garbage data and
		potentially crash horribly. If you try to send too many parameters, it
		should be freed fine and not cause issue but is very pointless!


==============================================================================*/


#include <string>
#include <vector>

using std::string;
using std::vector;

#include "natives.hpp"
#include "pawpy.hpp"


cell Native::RunPython(AMX* amx, cell* params)
{
	debug("Native::RunPython called");

	string
		module,
		function,
		argformat,
		callback;

	size_t numargs = static_cast<cell>(params[0] / sizeof(cell));

	vector<string> arguments;
	arguments.reserve(numargs - 3);
	uint8_t arg_count = 0;
	cell *addr_ptr = nullptr;
	cell arg_value;
	float arg_value_float;
	cell *addr_ptr_arr = nullptr;
	string array_string;

	module = amx_GetCppString(amx, params[1]);
	function = amx_GetCppString(amx, params[2]);
	argformat = amx_GetCppString(amx, params[3]);
	callback = "";

	if(argformat.length() != numargs - 3)
	{
		samp_printf("ERROR: Argument length (%d) does not match format specifier count (%d).", numargs - 3, argformat.length());
		return 0;
	}

	for(char c : argformat)
	{
		switch(c)
		{
		case 'd':
		case 'i':
			amx_GetAddr(amx, params[arg_count + 4], &addr_ptr);
			arg_count++;
			arg_value = *addr_ptr;

			if(addr_ptr_arr != nullptr)
			{
				if(arg_value <= 0)
				{
					samp_printf("ERROR: Invalid array size found in int parameter following array parameter.");
					return 0;
				}

				array_string = "[" + std::to_string(addr_ptr_arr[0]);
				for(int i = 1; i < arg_value; i++)
				{
					array_string += ", " + std::to_string(addr_ptr_arr[i]);
				}
				array_string += "]";

				arguments.push_back(array_string);

				addr_ptr_arr = nullptr;
				debug("[arg %d] found parameter of type int: %d as size for array %s", arg_count, arg_value, array_string.c_str());
			}
			else
			{
				debug("[arg %d] found parameter of type int: %d", arg_count, arg_value);
			}

			arguments.push_back(std::to_string(arg_value));
			break;

		case 'f':
			amx_GetAddr(amx, params[arg_count + 4], &addr_ptr);
			arg_value_float = *((float*)addr_ptr);
			arguments.push_back(std::to_string(arg_value_float));
			arg_count++;

			debug("[arg %d] found parameter of type float: %f", arg_count, arg_value_float);
			break;

		case 's':
			arguments.push_back(amx_GetCppString(amx, params[arg_count + 4]));
			arg_count++;

			debug("[arg %d] found parameter of type string: %s", arg_count, amx_GetCppString(amx, params[arg_count + 4]).c_str());
			break;

		case 'a':
			amx_GetAddr(amx, params[arg_count + 4], &addr_ptr_arr);
			arg_count++;

			debug("[arg %d] found parameter of type array (detailed in next d argument)", arg_count);
			break;

		default:
			samp_printf("ERROR: Invalid format specifier: '%c' in RunPython", c);
		}
	}

	for(string s : arguments)
	{
		debug("Arg: '%s'", s.c_str());
	}

	return Pawpy::run_python(module, function, callback);
}

cell Native::RunPythonThreaded(AMX* amx, cell* params)
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
