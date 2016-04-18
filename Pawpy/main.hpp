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
		This file includes general libraries and declares functions used
		throughout the plugin.


==============================================================================*/


/*
	Note:
	This tells the Python library that stdint is available, it doesn't work
	without this for some unknown reason.
*/
#ifndef HAVE_STDINT_H
#define HAVE_STDINT_H
#endif

#include <malloc.h>
#include <stdint.h>
#include <stdarg.h>
#include <direct.h>

/*
	Note:
	Wraps the logprintf function. Could have been done with a simple call to
	logprintf where the function pointer is stored globally but this happened.
*/
void samp_printf(const char* message, ...);
void samp_pyerr();

/*
	Note:
	Lazy debug mode. When VS is out of debug mode, the debug calls are ignored
	from compilation completely. This method means there are no #if directives
	littered around the code.
*/
#ifdef _DEBUG
#define debug(message, ...) samp_printf(message, __VA_ARGS__)
#else
#define debug(message, ...)
#endif