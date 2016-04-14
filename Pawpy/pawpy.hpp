/*==============================================================================

	Pawpy - Python Utility for Pawn
	Southclaw
	2016

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

int thread_call(string module, string function, string callback);
void run_call(pycall_t pycall);

void amx_tick(AMX* amx);

}

#endif