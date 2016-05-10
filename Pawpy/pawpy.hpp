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
		This header and it's counterpart .cpp are the only two files that don't
		really contain generic SA:MP plugin boilerplate code. See the .cpp for
		implementation details.


==============================================================================*/


#ifndef PAWPY_H
#define PAWPY_H

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
#include <sdk.hpp>


namespace Pawpy
{

struct pycall_t
{
	string module;
	string function;
	string callback;
	vector<string> arguments;
	std::thread::id threadid;
	char* returns;
};

extern stack<Pawpy::pycall_t> call_stack;
extern mutex call_stack_mutex;

pycall_t prepare(string module, string function, string callback, vector<string> arguments);

int run_python_threaded(pycall_t call);
void python_thread(pycall_t pycall);

char* run_python(pycall_t pycall);

void amx_tick(AMX* amx);

}

#endif
