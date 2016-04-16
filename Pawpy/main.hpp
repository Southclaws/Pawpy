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


// platform specifics
#ifndef HAVE_STDINT_H
#define HAVE_STDINT_H
#endif

#include <malloc.h>
#include <stdint.h>
#include <stdarg.h>
#include <direct.h>

void samp_printf(const char* message, ...);
void samp_pyerr();

#ifdef _DEBUG
#define debug(message, ...) samp_printf(message, __VA_ARGS__)
#else
#define debug(message, ...)
#endif