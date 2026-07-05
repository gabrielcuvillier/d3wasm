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
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#ifndef __MATH_SIMD_SSE_H__
#define __MATH_SIMD_SSE_H__

#include "idlib/math/Simd_MMX.h"

/*
===============================================================================

	SSE implementation of idSIMDProcessor

===============================================================================
*/

class idSIMD_SSE : public idSIMD_MMX {
public:
#if defined(__GNUC__) && defined(__SSE__)
	using idSIMD_MMX::Dot;
	using idSIMD_MMX::MinMax;

	virtual const char * VPCALL GetName( void ) const;
	virtual void VPCALL Dot( float *dst,			const idPlane &constant,const idDrawVert *src,	const int count );
	virtual	void VPCALL MinMax( idVec3 &min,		idVec3 &max,			const idDrawVert *src,	const int *indexes,		const int count );
#ifndef __EMSCRIPTEN__
	// This ones creates lots of issues in shadows on Emscripten
	virtual void VPCALL Dot( float *dst,			const idVec3 &constant,	const idPlane *src,		const int count );
#endif
#endif
};

#endif /* !__MATH_SIMD_SSE_H__ */
