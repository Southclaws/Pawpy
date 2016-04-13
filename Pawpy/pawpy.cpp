/*==============================================================================

	Pawpy - Python Utility for Pawn
	Southclaw
	2016

==============================================================================*/


#include <string>
#include <thread>

using std::thread;

#include "main.hpp"

#include "pawpy.hpp"
#include <sdk.hpp>


stack<Pawpy::pycall_t> Pawpy::call_stack;


int Pawpy::execCall(string module, string callback)
{
	pycall_t call;
	thread* t = nullptr;

	call.module = module;
	call.callback = callback;

	t = new thread(runCall, call);

	if(t == nullptr)
	{
		// raise
	}

	call.threadid = t->get_id();

	t->detach();

	delete t;

	return 0;
}

void Pawpy::runCall(pycall_t pycall)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	pycall.returns = "return";
	call_stack.push(pycall);
}

void Pawpy::amxProcessTick(AMX* amx)
{
	if(call_stack.empty())
		return;

	samp_printf("amx %x", amx);

	Pawpy::pycall_t call;
	int error = 0;
	int amx_idx = -1;
	cell amx_addr;
	cell amx_ret;
	cell *phys_addr; 

	while(!Pawpy::call_stack.empty())
	{
		call = Pawpy::call_stack.top();

		samp_printf("call: '%s'", call.callback.c_str());

		error = amx_FindPublic(amx, call.callback.c_str(), &amx_idx);

		if(error == AMX_ERR_NONE)
		{
			// callback format: (string[], len)
			amx_Push(amx, strlen(call.returns));
			amx_PushString(amx, &amx_addr, &phys_addr, call.returns, 0, 0);
			amx_PushString(amx, &amx_addr, &phys_addr, call.module.c_str(), 0, 0);

			amx_Exec(amx, &amx_ret, amx_idx);
			amx_Release(amx, amx_addr);

			switch(amx_ret)
			{
			case 0:
				break;
			case 1:
				continue;
			}

			samp_printf("return value: %d", amx_ret);
		}
		else
		{
			samp_printf("ERROR: amx_FindPublic returned %d.", error);
		}

		Pawpy::call_stack.pop();
	}
}
