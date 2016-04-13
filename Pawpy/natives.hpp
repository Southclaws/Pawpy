/*==============================================================================

	Pawpy - Python Utility for Pawn
	Southclaw
	2016

==============================================================================*/


#ifndef PAWPY_NATIVES_H
#define PAWPY_NATIVES_H

#include "main.hpp"
#include <sdk.hpp>


namespace Native 
{
	cell AMX_NATIVE_CALL PawpyExec(AMX *amx, cell *params);
};

#endif