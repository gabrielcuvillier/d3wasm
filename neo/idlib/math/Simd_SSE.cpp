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

#include "sys/platform.h"
#include "idlib/geometry/DrawVert.h"

#include "idlib/math/Simd_SSE.h"

//===============================================================
//                                                        M
//  SSE implementation of idSIMDProcessor                MrE
//                                                        E
//===============================================================

#define DRAWVERT_SIZE				60
#define DRAWVERT_XYZ_OFFSET			(0*4)
#define DRAWVERT_ST_OFFSET			(3*4)
#define DRAWVERT_NORMAL_OFFSET		(5*4)
#define DRAWVERT_TANGENT0_OFFSET	(8*4)
#define DRAWVERT_TANGENT1_OFFSET	(11*4)
#define DRAWVERT_COLOR_OFFSET		(14*4)

#if defined(__GNUC__) && defined(__SSE__)

#include <xmmintrin.h>

#define SHUFFLEPS( x, y, z, w )		(( (x) & 3 ) << 6 | ( (y) & 3 ) << 4 | ( (z) & 3 ) << 2 | ( (w) & 3 ))
#define R_SHUFFLEPS( x, y, z, w )	(( (w) & 3 ) << 6 | ( (z) & 3 ) << 4 | ( (y) & 3 ) << 2 | ( (x) & 3 ))

/*
============
idSIMD_SSE::GetName
============
*/
const char * idSIMD_SSE::GetName( void ) const {
	return "MMX & SSE";
}

/*
============
idSIMD_SSE::Dot

  dst[i] = constant.Normal() * src[i].xyz + constant[3];
============
*/
void VPCALL idSIMD_SSE::Dot( float *dst, const idPlane &constant, const idDrawVert *src, const int count ) {
	// 0,  1,  2
	// 3,  4,  5
	// 6,  7,  8
	// 9, 10, 11

	/*
		mov			eax, count
		mov			edi, constant
		mov			edx, eax
		mov			esi, src
		mov			ecx, dst
	*/
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;	// Declare 8 xmm registers.
	int count_l4 = count;                                   // count_l4 = eax
	int count_l1 = count;                                   // count_l1 = edx
	char *constant_p = (char *)&constant;                   // constant_p = edi
	char *src_p = (char *) src;                             // src_p = esi
	char *dst_p = (char *) dst;                             // dst_p = ecx

	assert( sizeof( idDrawVert ) == DRAWVERT_SIZE );
	assert( ptrdiff_t(&src->xyz) - ptrdiff_t(src) == DRAWVERT_XYZ_OFFSET );

	/*
		and			eax, ~3
		movss		xmm4, [edi+0]
		shufps		xmm4, xmm4, R_SHUFFLEPS( 0, 0, 0, 0 )
		movss		xmm5, [edi+4]
		shufps		xmm5, xmm5, R_SHUFFLEPS( 0, 0, 0, 0 )
		movss		xmm6, [edi+8]
		shufps		xmm6, xmm6, R_SHUFFLEPS( 0, 0, 0, 0 )
		movss		xmm7, [edi+12]
		shufps		xmm7, xmm7, R_SHUFFLEPS( 0, 0, 0, 0 )
	*/
	count_l4 = count_l4 & ~3;
	xmm4 = _mm_load_ss((float *) (constant_p));
	xmm4 = _mm_shuffle_ps(xmm4, xmm4, R_SHUFFLEPS( 0, 0, 0, 0 ));
	xmm5 = _mm_load_ss((float *) (constant_p + 4));
	xmm5 = _mm_shuffle_ps(xmm5, xmm5, R_SHUFFLEPS( 0, 0, 0, 0 ));
	xmm6 = _mm_load_ss((float *) (constant_p + 8));
	xmm6 = _mm_shuffle_ps(xmm6, xmm6, R_SHUFFLEPS( 0, 0, 0, 0 ));
	xmm7 = _mm_load_ss((float *) (constant_p + 12));
	xmm7 = _mm_shuffle_ps(xmm7, xmm7, R_SHUFFLEPS( 0, 0, 0, 0 ));

	/*
		jz			startVert1
	*/
	if(count_l4 != 0) {
	/*
		imul		eax, DRAWVERT_SIZE
		add			esi, eax
		neg			eax
	*/
		count_l4 = count_l4 * DRAWVERT_SIZE;
		src_p = src_p + count_l4;
		count_l4 = -count_l4;
	/*
	loopVert4:
	*/
		do {
	/*
		movss		xmm0, [esi+eax+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  3,  X,  X,  X
		movss		xmm2, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]	//  2,  X,  X,  X
		movhps		xmm0, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  3,  X,  0,  1
		movaps		xmm1, xmm0												//  3,  X,  0,  1
	*/
			xmm0 = _mm_load_ss((float *) (src_p+count_l4+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0));        // 3,  X,  X,  X
			xmm2 = _mm_load_ss((float *) (src_p+count_l4+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8));        // 2,  X,  X,  X
			xmm0 = _mm_loadh_pi(xmm0, (__m64 *) (src_p+count_l4+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0)); // 3,  X,  0,  1
			xmm1 = xmm0;							                                                    // 3,  X,  0,  1

	/*
		movlps		xmm1, [esi+eax+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]	//  4,  5,  0,  1
		shufps		xmm2, xmm1, R_SHUFFLEPS( 0, 1, 0, 1 )					//  2,  X,  4,  5
	*/
			xmm1 = _mm_loadl_pi(xmm1, (__m64 *) (src_p+count_l4+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4)); // 4,  5,  0,  1
			xmm2 = _mm_shuffle_ps(xmm2, xmm1, R_SHUFFLEPS( 0, 1, 0, 1 ));                               // 2,  X,  4,  5

	/*
		movss		xmm3, [esi+eax+3*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  9,  X,  X,  X
		movhps		xmm3, [esi+eax+2*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  9,  X,  6,  7
		shufps		xmm0, xmm3, R_SHUFFLEPS( 2, 0, 2, 0 )					//  0,  3,  6,  9
	*/
			xmm3 = _mm_load_ss((float *) (src_p+count_l4+3*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0));        // 9,  X,  X,  X
			xmm3 = _mm_loadh_pi(xmm3, (__m64 *) (src_p+count_l4+2*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0)); // 9,  X,  6,  7
			xmm0 = _mm_shuffle_ps(xmm0, xmm3, R_SHUFFLEPS( 2, 0, 2, 0 ));                               // 0,  3,  6,  9
	/*
		movlps		xmm3, [esi+eax+3*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]	// 10, 11,  6,  7
		shufps		xmm1, xmm3, R_SHUFFLEPS( 3, 0, 3, 0 )					//  1,  4,  7, 10
	*/
			xmm3 = _mm_loadl_pi(xmm3, (__m64 *)(src_p+count_l4+3*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4));  // 10, 11, 6,  7
			xmm1 = _mm_shuffle_ps(xmm1, xmm3, R_SHUFFLEPS( 3, 0, 3, 0 ));                               // 1,  4,  7,  10
	/*
		movhps		xmm3, [esi+eax+2*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]	// 10, 11,  8,  X
		shufps		xmm2, xmm3, R_SHUFFLEPS( 0, 3, 2, 1 )					//  2,  5,  8, 11
	*/
			xmm3 = _mm_loadh_pi(xmm3, (__m64 *)(src_p+count_l4+2*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8));  // 10, 11, 8,  X
			xmm2 = _mm_shuffle_ps(xmm2, xmm3, R_SHUFFLEPS( 0, 3, 2, 1 ));                               // 2,  5,  8,  11

	/*
		add			ecx, 16
		add			eax, 4*DRAWVERT_SIZE
	*/
			dst_p = dst_p + 16;
			count_l4 = count_l4 + 4*DRAWVERT_SIZE;

	/*
		mulps		xmm0, xmm4
		mulps		xmm1, xmm5
		mulps		xmm2, xmm6
		addps		xmm0, xmm7
		addps		xmm0, xmm1
		addps		xmm0, xmm2
	*/
			xmm0 = _mm_mul_ps(xmm0, xmm4);
			xmm1 = _mm_mul_ps(xmm1, xmm5);
			xmm2 = _mm_mul_ps(xmm2, xmm6);
			xmm0 = _mm_add_ps(xmm0, xmm7);
			xmm0 = _mm_add_ps(xmm0, xmm1);
			xmm0 = _mm_add_ps(xmm0, xmm2);

	/*
		movlps		[ecx-16+0], xmm0
		movhps		[ecx-16+8], xmm0
		jl			loopVert4
	*/
			_mm_storel_pi((__m64 *) (dst_p-16+0), xmm0);
			_mm_storeh_pi((__m64 *) (dst_p-16+8), xmm0);
		} while(count_l4 < 0);
	}

	/*
	startVert1:
		and			edx, 3
		jz			done
	*/
	count_l1 = count_l1 & 3;
	if(count_l1 != 0) {
	/*
		loopVert1:
		movss		xmm0, [esi+eax+DRAWVERT_XYZ_OFFSET+0]
		movss		xmm1, [esi+eax+DRAWVERT_XYZ_OFFSET+4]
		movss		xmm2, [esi+eax+DRAWVERT_XYZ_OFFSET+8]
		mulss		xmm0, xmm4
		mulss		xmm1, xmm5
		mulss		xmm2, xmm6
		addss		xmm0, xmm7
		add			ecx, 4
		addss		xmm0, xmm1
		add			eax, DRAWVERT_SIZE
		addss		xmm0, xmm2
		dec			edx
		movss		[ecx-4], xmm0
		jnz			loopVert1
	*/
		do {
			xmm0 = _mm_load_ss((float *) (src_p+count_l4+DRAWVERT_XYZ_OFFSET+0));
			xmm1 = _mm_load_ss((float *) (src_p+count_l4+DRAWVERT_XYZ_OFFSET+4));
			xmm2 = _mm_load_ss((float *) (src_p+count_l4+DRAWVERT_XYZ_OFFSET+8));
			xmm0 = _mm_mul_ss(xmm0, xmm4);
			xmm1 = _mm_mul_ss(xmm1, xmm5);
			xmm2 = _mm_mul_ss(xmm2, xmm6);
			xmm0 = _mm_add_ss(xmm0, xmm7);
			dst_p = dst_p + 4;
			xmm0 = _mm_add_ss(xmm0, xmm1);
			count_l4 = count_l4 + DRAWVERT_SIZE;
			xmm0 = _mm_add_ss(xmm0, xmm2);
			count_l1 = count_l1 - 1;
			_mm_store_ss((float *) (dst_p-4), xmm0);
		} while( count_l1 != 0);
	}
	/*
		done:
	*/
}

/*
============
idSIMD_SSE::MinMax
============
*/
void VPCALL idSIMD_SSE::MinMax( idVec3 &min, idVec3 &max, const idDrawVert *src, const int *indexes, const int count ) {

	assert( sizeof( idDrawVert ) == DRAWVERT_SIZE );
	assert( ptrdiff_t(&src->xyz) - ptrdiff_t(src) == DRAWVERT_XYZ_OFFSET );

	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	char *indexes_p;
	char *src_p;
	int count_l;
	int edx;
	char *min_p;
	char *max_p;

	/*
		movss		xmm0, idMath::INFINITY
		xorps		xmm1, xmm1
		shufps		xmm0, xmm0, R_SHUFFLEPS( 0, 0, 0, 0 )
		subps		xmm1, xmm0
		movaps		xmm2, xmm0
		movaps		xmm3, xmm1
	*/
		xmm0 = _mm_load_ss(&idMath::INFINITY);
		// To satisfy the compiler use xmm0 instead.
		xmm1 = _mm_xor_ps(xmm0, xmm0);
		xmm0 = _mm_shuffle_ps(xmm0, xmm0, R_SHUFFLEPS( 0, 0, 0, 0 ));
		xmm1 = _mm_sub_ps(xmm1, xmm0);
		xmm2 = xmm0;
		xmm3 = xmm1;

	/*
		mov			edi, indexes
		mov			esi, src
		mov			eax, count
		and			eax, ~3
		jz			done4
	*/
		indexes_p = (char *) indexes;
		src_p = (char *) src;
		count_l = count;
		count_l = count_l & ~3;
		if(count_l != 0) {
	/*
		shl			eax, 2
		add			edi, eax
		neg			eax
	*/
			count_l = count_l << 2;
			indexes_p = indexes_p + count_l;
			count_l = -count_l;
	/*
	loop4:
//		prefetchnta	[edi+128]
//		prefetchnta	[esi+4*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET]
	*/
		do {
	/*
		mov			edx, [edi+eax+0]
		imul		edx, DRAWVERT_SIZE
		movss		xmm4, [esi+edx+DRAWVERT_XYZ_OFFSET+8]
		movhps		xmm4, [esi+edx+DRAWVERT_XYZ_OFFSET+0]
		minps		xmm0, xmm4
		maxps		xmm1, xmm4
	*/
			edx = *((int*)(indexes_p+count_l+0));
			edx = edx * DRAWVERT_SIZE;
			xmm4 = _mm_load_ss((float *) (src_p+edx+DRAWVERT_XYZ_OFFSET+8));
			xmm4 = _mm_loadh_pi(xmm4, (__m64 *) (src_p+edx+DRAWVERT_XYZ_OFFSET+0) );
			xmm0 = _mm_min_ps(xmm0, xmm4);
			xmm1 = _mm_max_ps(xmm1, xmm4);

	/*
		mov			edx, [edi+eax+4]
		imul		edx, DRAWVERT_SIZE
		movss		xmm5, [esi+edx+DRAWVERT_XYZ_OFFSET+0]
		movhps		xmm5, [esi+edx+DRAWVERT_XYZ_OFFSET+4]
		minps		xmm2, xmm5
		maxps		xmm3, xmm5
	*/
			edx = *((int*)(indexes_p+count_l+4));
			edx = edx * DRAWVERT_SIZE;
			xmm5 = _mm_load_ss((float *) (src_p+edx+DRAWVERT_XYZ_OFFSET+0));
			xmm5 = _mm_loadh_pi(xmm5, (__m64 *) (src_p+edx+DRAWVERT_XYZ_OFFSET+4) );
			xmm2 = _mm_min_ps(xmm2, xmm5);
			xmm3 = _mm_max_ps(xmm3, xmm5);

	/*
		mov			edx, [edi+eax+8]
		imul		edx, DRAWVERT_SIZE
		movss		xmm6, [esi+edx+DRAWVERT_XYZ_OFFSET+8]
		movhps		xmm6, [esi+edx+DRAWVERT_XYZ_OFFSET+0]
		minps		xmm0, xmm6
		maxps		xmm1, xmm6
	*/
			edx = *((int*)(indexes_p+count_l+8));
			edx = edx * DRAWVERT_SIZE;
			xmm6 = _mm_load_ss((float *) (src_p+edx+DRAWVERT_XYZ_OFFSET+8));
			xmm6 = _mm_loadh_pi(xmm6, (__m64 *) (src_p+edx+DRAWVERT_XYZ_OFFSET+0) );
			xmm0 = _mm_min_ps(xmm0, xmm6);
			xmm1 = _mm_max_ps(xmm1, xmm6);

	/*
		mov			edx, [edi+eax+12]
		imul		edx, DRAWVERT_SIZE
		movss		xmm7, [esi+edx+DRAWVERT_XYZ_OFFSET+0]
		movhps		xmm7, [esi+edx+DRAWVERT_XYZ_OFFSET+4]
		minps		xmm2, xmm7
		maxps		xmm3, xmm7
	*/
			edx = *((int*)(indexes_p+count_l+12));
			edx = edx * DRAWVERT_SIZE;
			xmm7 = _mm_load_ss((float *) (src_p+edx+DRAWVERT_XYZ_OFFSET+0));
			xmm7 = _mm_loadh_pi(xmm7, (__m64 *) (src_p+edx+DRAWVERT_XYZ_OFFSET+4) );
			xmm2 = _mm_min_ps(xmm2, xmm7);
			xmm3 = _mm_max_ps(xmm3, xmm7);

	/*
		add			eax, 4*4
		jl			loop4
	*/
			count_l = count_l + 4*4;
		} while (count_l < 0);
	}
	/*
	done4:
		mov			eax, count
		and			eax, 3
		jz			done1
	*/
	count_l = count;
	count_l = count_l & 3;
	if(count_l != 0) {
	/*
		shl			eax, 2
		add			edi, eax
		neg			eax
	*/
		count_l = count_l << 2;
		indexes_p = indexes_p + count_l;
		count_l = -count_l;
	/*
	loop1:
	*/
		do{
	/*
		mov			edx, [edi+eax+0]
		imul		edx, DRAWVERT_SIZE;
		movss		xmm4, [esi+edx+DRAWVERT_XYZ_OFFSET+8]
		movhps		xmm4, [esi+edx+DRAWVERT_XYZ_OFFSET+0]
		minps		xmm0, xmm4
		maxps		xmm1, xmm4
	*/
			edx = *((int*)(indexes_p+count_l+0));
			edx = edx * DRAWVERT_SIZE;
			xmm4 = _mm_load_ss((float *) (src_p+edx+DRAWVERT_XYZ_OFFSET+8));
			xmm4 = _mm_loadh_pi(xmm4, (__m64 *) (src_p+edx+DRAWVERT_XYZ_OFFSET+0) );
			xmm0 = _mm_min_ps(xmm0, xmm4);
			xmm1 = _mm_max_ps(xmm1, xmm4);

	/*
		add			eax, 4
		jl			loop1
	*/
			count_l = count_l + 4;
		} while (count_l < 0);

	}

	/*
	done1:
		shufps		xmm2, xmm2, R_SHUFFLEPS( 3, 1, 0, 2 )
		shufps		xmm3, xmm3, R_SHUFFLEPS( 3, 1, 0, 2 )
		minps		xmm0, xmm2
		maxps		xmm1, xmm3
		mov			esi, min
		movhps		[esi], xmm0
		movss		[esi+8], xmm0
		mov			edi, max
		movhps		[edi], xmm1
		movss		[edi+8], xmm1
	*/
	xmm2 = _mm_shuffle_ps(xmm2, xmm2, R_SHUFFLEPS( 3, 1, 0, 2 ));
	xmm3 = _mm_shuffle_ps(xmm3, xmm3, R_SHUFFLEPS( 3, 1, 0, 2 ));
	xmm0 = _mm_min_ps(xmm0, xmm2);
	xmm1 = _mm_max_ps(xmm1, xmm3);
	min_p = (char *) &min;
	_mm_storeh_pi((__m64 *)(min_p), xmm0);
	_mm_store_ss((float *)(min_p+8), xmm0);
	max_p = (char *) &max;
	_mm_storeh_pi((__m64 *)(max_p), xmm1);
	_mm_store_ss((float *)(max_p+8), xmm1);
}

#ifndef __EMSCRIPTEN__
/*
============
idSIMD_SSE::Dot

  dst[i] = constant * src[i].Normal() + src[i][3];
============
*/
void VPCALL idSIMD_SSE::Dot( float *dst, const idVec3 &constant, const idPlane *src, const int count ) {
	int count_l4;
	int count_l1;
	char *constant_p;
	char *src_p;
	char *dst_p;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;

	/*
		mov			eax, count
		mov			edi, constant
		mov			edx, eax
		mov			esi, src
		mov			ecx, dst
		and			eax, ~3
	*/
	count_l4 = count;
	constant_p = (char *) &constant;
	count_l1 = count_l4;
	src_p = (char *) src;
	dst_p = (char *) dst;
	count_l4 = count_l4 & ~3;

	/*
		movss		xmm5, [edi+0]
		shufps		xmm5, xmm5, R_SHUFFLEPS( 0, 0, 0, 0 )
		movss		xmm6, [edi+4]
		shufps		xmm6, xmm6, R_SHUFFLEPS( 0, 0, 0, 0 )
		movss		xmm7, [edi+8]
		shufps		xmm7, xmm7, R_SHUFFLEPS( 0, 0, 0, 0 )
	*/
	xmm5 = _mm_load_ss((float *) (constant_p+0));
	xmm5 = _mm_shuffle_ps(xmm5, xmm5, R_SHUFFLEPS( 0, 0, 0, 0 ));
	xmm6 = _mm_load_ss((float *) (constant_p+4));
	xmm6 = _mm_shuffle_ps(xmm6, xmm6, R_SHUFFLEPS( 0, 0, 0, 0 ));
	xmm7 = _mm_load_ss((float *) (constant_p+8));
	xmm7 = _mm_shuffle_ps(xmm7, xmm7, R_SHUFFLEPS( 0, 0, 0, 0 ));

	/*
		jz			startVert1
	*/
	if (count_l4 != 0) {
	/*
		imul		eax, 16
		add			esi, eax
		neg			eax
	*/
		count_l4 = count_l4 * 16;
		src_p = src_p + count_l4;
		count_l4 = -count_l4;
	/*
	loopVert4:
	*/
		do {
	/*
		movlps		xmm1, [esi+eax+ 0]
		movlps		xmm3, [esi+eax+ 8]
		movhps		xmm1, [esi+eax+16]
		movhps		xmm3, [esi+eax+24]
		movlps		xmm2, [esi+eax+32]
		movlps		xmm4, [esi+eax+40]
		movhps		xmm2, [esi+eax+48]
		movhps		xmm4, [esi+eax+56]
		movaps		xmm0, xmm1
		shufps		xmm0, xmm2, R_SHUFFLEPS( 0, 2, 0, 2 )
		shufps		xmm1, xmm2, R_SHUFFLEPS( 1, 3, 1, 3 )
		movaps		xmm2, xmm3
		shufps		xmm2, xmm4, R_SHUFFLEPS( 0, 2, 0, 2 )
		shufps		xmm3, xmm4, R_SHUFFLEPS( 1, 3, 1, 3 )
	*/
			xmm1 = _mm_loadl_pi(xmm1, (__m64 *)(src_p+count_l4+ 0));
			xmm3 = _mm_loadl_pi(xmm3, (__m64 *)(src_p+count_l4+ 8));
			xmm1 = _mm_loadh_pi(xmm1, (__m64 *)(src_p+count_l4+16));
			xmm3 = _mm_loadh_pi(xmm3, (__m64 *)(src_p+count_l4+24));
			xmm2 = _mm_loadl_pi(xmm2, (__m64 *)(src_p+count_l4+32));
			xmm4 = _mm_loadl_pi(xmm4, (__m64 *)(src_p+count_l4+40));
			xmm2 = _mm_loadh_pi(xmm2, (__m64 *)(src_p+count_l4+48));
			xmm4 = _mm_loadh_pi(xmm4, (__m64 *)(src_p+count_l4+56));

			xmm0 = xmm1;
			xmm0 = _mm_shuffle_ps(xmm0, xmm2, R_SHUFFLEPS( 0, 2, 0, 2 ));
			xmm1 = _mm_shuffle_ps(xmm1, xmm2, R_SHUFFLEPS( 1, 3, 1, 3 ));
			xmm2 = xmm3;
			xmm2 = _mm_shuffle_ps(xmm2, xmm4, R_SHUFFLEPS( 0, 2, 0, 2 ));
			xmm3 = _mm_shuffle_ps(xmm3, xmm4, R_SHUFFLEPS( 1, 3, 1, 3 ));

	/*
		add			ecx, 16
		add			eax, 4*16
	*/
			dst_p = dst_p + 16;
			count_l4 = count_l4 + 4*16;

	/*
		mulps		xmm0, xmm5
		mulps		xmm1, xmm6
		mulps		xmm2, xmm7
		addps		xmm0, xmm3
		addps		xmm0, xmm1
		addps		xmm0, xmm2
	*/
			xmm0 = _mm_mul_ps(xmm0, xmm5);
			xmm1 = _mm_mul_ps(xmm1, xmm6);
			xmm2 = _mm_mul_ps(xmm2, xmm7);
			xmm0 = _mm_add_ps(xmm0, xmm3);
			xmm0 = _mm_add_ps(xmm0, xmm1);
			xmm0 = _mm_add_ps(xmm0, xmm2);

	/*
		movlps		[ecx-16+0], xmm0
		movhps		[ecx-16+8], xmm0
		jl			loopVert4
	*/
			_mm_storel_pi((__m64 *) (dst_p-16+0), xmm0);
			_mm_storeh_pi((__m64 *) (dst_p-16+8), xmm0);
		} while (count_l4 < 0);
	}

	/*
	startVert1:
		and			edx, 3
		jz			done
	*/
	count_l1 = count_l1 & 3;

	if(count_l1 != 0) {
	/*
	loopVert1:
	*/
		do {
	/*
		movss		xmm0, [esi+eax+0]
		movss		xmm1, [esi+eax+4]
		movss		xmm2, [esi+eax+8]
		mulss		xmm0, xmm5
		mulss		xmm1, xmm6
		mulss		xmm2, xmm7
		addss		xmm0, [esi+eax+12]
		add			ecx, 4
		addss		xmm0, xmm1
		add			eax, 16
		addss		xmm0, xmm2
		dec			edx
		movss		[ecx-4], xmm0
		jnz			loopVert1
	*/
			xmm0 = _mm_load_ss((float *) (src_p+count_l4+ 0));
			xmm1 = _mm_load_ss((float *) (src_p+count_l4+ 4));
			xmm2 = _mm_load_ss((float *) (src_p+count_l4+ 8));
			xmm3 = _mm_load_ss((float *) (src_p+count_l4+12));

			xmm0 = _mm_mul_ss(xmm0, xmm5);
			xmm1 = _mm_mul_ss(xmm1, xmm6);
			xmm2 = _mm_mul_ss(xmm2, xmm7);

			xmm0 = _mm_add_ss(xmm0, xmm3);
			dst_p = dst_p + 4;
			xmm0 = _mm_add_ss(xmm0, xmm1);
			count_l4 = count_l4 + 16;
			xmm0 = _mm_add_ss(xmm0, xmm2);
			count_l1 = count_l1 - 1;
			_mm_store_ss((float *) (dst_p-4), xmm0);
		} while (count_l1 != 0);
	}
	/*
	done:
	*/
}
#endif
#endif
