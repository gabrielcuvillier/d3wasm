/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.	If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include <float.h>

#include <SDL_cpuinfo.h>

// MSVC header intrin.h uses strcmp and errors out when not set
#define IDSTR_NO_REDIRECT

#include "sys/platform.h"
#include "framework/Common.h"

#include "sys/sys_public.h"

void Sys_FPU_SetDAZ(bool enable) {
}

void Sys_FPU_SetFTZ(bool enable) {
}

/*
================
Sys_GetProcessorId
================
*/
int Sys_GetProcessorId( void ) {
	int flags = CPUID_GENERIC;

#ifndef __EMSCRIPTEN__
	if (SDL_HasMMX())
		flags |= CPUID_MMX;

	if (SDL_HasSSE())
		flags |= CPUID_SSE;

	if (SDL_HasSSE2())
		flags |= CPUID_SSE2;

	if (SDL_HasSSE3()) {
		flags |= CPUID_SSE3;
	}
#else
#if defined(__SSE__) || defined(__SSE2__)
	flags |= CPUID_SSE | CPUID_MMX;
#endif
#if defined(__SSE2__)
	flags |= CPUID_SSE2;
#endif
#endif
	return flags;
}

/*
===============
Sys_FPU_SetPrecision
===============
*/
void Sys_FPU_SetPrecision() {

}
