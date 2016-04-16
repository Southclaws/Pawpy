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


#ifndef PAWPY_H
#define PAWPY_H

#include <string>
#include <stack>
#include <thread>

using std::string;
using std::stack;
using std::thread;

#include "main.hpp"
#include <sdk.hpp>

namespace Pawpy
{

struct pycall_t
{
	string module;
	string function;
	string callback;
	std::thread::id threadid;
	char* returns;
};

extern stack<Pawpy::pycall_t> call_stack;

int run_python(string module, string function, string callback);
int run_python_threaded(string module, string function, string callback);

void run_call_thread(pycall_t pycall);
long run_call(pycall_t pycall);

void amx_tick(AMX* amx);

}

#endif
