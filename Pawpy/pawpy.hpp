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
	string callback;
	std::thread::id threadid;
	char* returns;
};

extern stack<Pawpy::pycall_t> call_stack;

int execCall(string module, string callback);
void runCall(pycall_t pycall);

void amxProcessTick(AMX* amx);

}

#endif