/*==============================================================================

	Pawpy - Python Utility for Pawn
	Southclaw
	2016

==============================================================================*/


// platform specifics
#ifndef HAVE_STDINT_H
#define HAVE_STDINT_H
#endif

#include <malloc.h>
#include <stdint.h>
#include <stdarg.h>

void samp_printf(const char* message, ...);
