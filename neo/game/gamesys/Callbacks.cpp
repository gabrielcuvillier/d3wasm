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
// generated file - see CREATE_EVENT_CODE

	/*******************************************************

		1 args

	*******************************************************/

	case 128 :
		typedef void ( idClass::*eventCallback_i_t )( const intptr_t );
		( this->*( eventCallback_i_t )callback )( data[ 0 ] );
		break;

	case 129 :
		typedef void ( idClass::*eventCallback_f_t )( const float );
		( this->*( eventCallback_f_t )callback )( *( float * )&data[ 0 ] );
		break;

	/*******************************************************

		2 args

	*******************************************************/

	case 256 :
		typedef void ( idClass::*eventCallback_ii_t )( const intptr_t, const intptr_t );
		( this->*( eventCallback_ii_t )callback )( data[ 0 ], data[ 1 ] );
		break;

	case 257 :
		typedef void ( idClass::*eventCallback_fi_t )( const float, const intptr_t );
		( this->*( eventCallback_fi_t )callback )( *( float * )&data[ 0 ], data[ 1 ] );
		break;

	case 258 :
		typedef void ( idClass::*eventCallback_if_t )( const intptr_t, const float );
		( this->*( eventCallback_if_t )callback )( data[ 0 ], *( float * )&data[ 1 ] );
		break;

	case 259 :
		typedef void ( idClass::*eventCallback_ff_t )( const float, const float );
		( this->*( eventCallback_ff_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ] );
		break;

	/*******************************************************

		3 args

	*******************************************************/

	case 512 :
		typedef void ( idClass::*eventCallback_iii_t )( const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_iii_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ] );
		break;

	case 513 :
		typedef void ( idClass::*eventCallback_fii_t )( const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_fii_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ] );
		break;

	case 514 :
		typedef void ( idClass::*eventCallback_ifi_t )( const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_ifi_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ] );
		break;

	case 515 :
		typedef void ( idClass::*eventCallback_ffi_t )( const float, const float, const intptr_t );
		( this->*( eventCallback_ffi_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ] );
		break;

	case 516 :
		typedef void ( idClass::*eventCallback_iif_t )( const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_iif_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ] );
		break;

	case 517 :
		typedef void ( idClass::*eventCallback_fif_t )( const float, const intptr_t, const float );
		( this->*( eventCallback_fif_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ] );
		break;

	case 518 :
		typedef void ( idClass::*eventCallback_iff_t )( const intptr_t, const float, const float );
		( this->*( eventCallback_iff_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ] );
		break;

	case 519 :
		typedef void ( idClass::*eventCallback_fff_t )( const float, const float, const float );
		( this->*( eventCallback_fff_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ] );
		break;

	/*******************************************************

		4 args

	*******************************************************/

	case 1024 :
		typedef void ( idClass::*eventCallback_iiii_t )( const intptr_t, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_iiii_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ] );
		break;

	case 1025 :
		typedef void ( idClass::*eventCallback_fiii_t )( const float, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_fiii_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ] );
		break;

	case 1026 :
		typedef void ( idClass::*eventCallback_ifii_t )( const intptr_t, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_ifii_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ] );
		break;

	case 1027 :
		typedef void ( idClass::*eventCallback_ffii_t )( const float, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_ffii_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ] );
		break;

	case 1028 :
		typedef void ( idClass::*eventCallback_iifi_t )( const intptr_t, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_iifi_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ] );
		break;

	case 1029 :
		typedef void ( idClass::*eventCallback_fifi_t )( const float, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_fifi_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ] );
		break;

	case 1030 :
		typedef void ( idClass::*eventCallback_iffi_t )( const intptr_t, const float, const float, const intptr_t );
		( this->*( eventCallback_iffi_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ] );
		break;

	case 1031 :
		typedef void ( idClass::*eventCallback_fffi_t )( const float, const float, const float, const intptr_t );
		( this->*( eventCallback_fffi_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ] );
		break;

	case 1032 :
		typedef void ( idClass::*eventCallback_iiif_t )( const intptr_t, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_iiif_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ] );
		break;

	case 1033 :
		typedef void ( idClass::*eventCallback_fiif_t )( const float, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_fiif_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ] );
		break;

	case 1034 :
		typedef void ( idClass::*eventCallback_ifif_t )( const intptr_t, const float, const intptr_t, const float );
		( this->*( eventCallback_ifif_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ] );
		break;

	case 1035 :
		typedef void ( idClass::*eventCallback_ffif_t )( const float, const float, const intptr_t, const float );
		( this->*( eventCallback_ffif_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ] );
		break;

	case 1036 :
		typedef void ( idClass::*eventCallback_iiff_t )( const intptr_t, const intptr_t, const float, const float );
		( this->*( eventCallback_iiff_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ] );
		break;

	case 1037 :
		typedef void ( idClass::*eventCallback_fiff_t )( const float, const intptr_t, const float, const float );
		( this->*( eventCallback_fiff_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ] );
		break;

	case 1038 :
		typedef void ( idClass::*eventCallback_ifff_t )( const intptr_t, const float, const float, const float );
		( this->*( eventCallback_ifff_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ] );
		break;

	case 1039 :
		typedef void ( idClass::*eventCallback_ffff_t )( const float, const float, const float, const float );
		( this->*( eventCallback_ffff_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ] );
		break;

	/*******************************************************

		5 args

	*******************************************************/

	case 2048 :
		typedef void ( idClass::*eventCallback_iiiii_t )( const intptr_t, const intptr_t, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_iiiii_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ] );
		break;

	case 2049 :
		typedef void ( idClass::*eventCallback_fiiii_t )( const float, const intptr_t, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_fiiii_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ] );
		break;

	case 2050 :
		typedef void ( idClass::*eventCallback_ifiii_t )( const intptr_t, const float, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_ifiii_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ] );
		break;

	case 2051 :
		typedef void ( idClass::*eventCallback_ffiii_t )( const float, const float, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_ffiii_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ] );
		break;

	case 2052 :
		typedef void ( idClass::*eventCallback_iifii_t )( const intptr_t, const intptr_t, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_iifii_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ] );
		break;

	case 2053 :
		typedef void ( idClass::*eventCallback_fifii_t )( const float, const intptr_t, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_fifii_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ] );
		break;

	case 2054 :
		typedef void ( idClass::*eventCallback_iffii_t )( const intptr_t, const float, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_iffii_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ] );
		break;

	case 2055 :
		typedef void ( idClass::*eventCallback_fffii_t )( const float, const float, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_fffii_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ] );
		break;

	case 2056 :
		typedef void ( idClass::*eventCallback_iiifi_t )( const intptr_t, const intptr_t, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_iiifi_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ] );
		break;

	case 2057 :
		typedef void ( idClass::*eventCallback_fiifi_t )( const float, const intptr_t, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_fiifi_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ] );
		break;

	case 2058 :
		typedef void ( idClass::*eventCallback_ififi_t )( const intptr_t, const float, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_ififi_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ] );
		break;

	case 2059 :
		typedef void ( idClass::*eventCallback_ffifi_t )( const float, const float, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_ffifi_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ] );
		break;

	case 2060 :
		typedef void ( idClass::*eventCallback_iiffi_t )( const intptr_t, const intptr_t, const float, const float, const intptr_t );
		( this->*( eventCallback_iiffi_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ] );
		break;

	case 2061 :
		typedef void ( idClass::*eventCallback_fiffi_t )( const float, const intptr_t, const float, const float, const intptr_t );
		( this->*( eventCallback_fiffi_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ] );
		break;

	case 2062 :
		typedef void ( idClass::*eventCallback_ifffi_t )( const intptr_t, const float, const float, const float, const intptr_t );
		( this->*( eventCallback_ifffi_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ] );
		break;

	case 2063 :
		typedef void ( idClass::*eventCallback_ffffi_t )( const float, const float, const float, const float, const intptr_t );
		( this->*( eventCallback_ffffi_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ] );
		break;

	case 2064 :
		typedef void ( idClass::*eventCallback_iiiif_t )( const intptr_t, const intptr_t, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_iiiif_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2065 :
		typedef void ( idClass::*eventCallback_fiiif_t )( const float, const intptr_t, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_fiiif_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2066 :
		typedef void ( idClass::*eventCallback_ifiif_t )( const intptr_t, const float, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_ifiif_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2067 :
		typedef void ( idClass::*eventCallback_ffiif_t )( const float, const float, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_ffiif_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2068 :
		typedef void ( idClass::*eventCallback_iifif_t )( const intptr_t, const intptr_t, const float, const intptr_t, const float );
		( this->*( eventCallback_iifif_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2069 :
		typedef void ( idClass::*eventCallback_fifif_t )( const float, const intptr_t, const float, const intptr_t, const float );
		( this->*( eventCallback_fifif_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2070 :
		typedef void ( idClass::*eventCallback_iffif_t )( const intptr_t, const float, const float, const intptr_t, const float );
		( this->*( eventCallback_iffif_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2071 :
		typedef void ( idClass::*eventCallback_fffif_t )( const float, const float, const float, const intptr_t, const float );
		( this->*( eventCallback_fffif_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2072 :
		typedef void ( idClass::*eventCallback_iiiff_t )( const intptr_t, const intptr_t, const intptr_t, const float, const float );
		( this->*( eventCallback_iiiff_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2073 :
		typedef void ( idClass::*eventCallback_fiiff_t )( const float, const intptr_t, const intptr_t, const float, const float );
		( this->*( eventCallback_fiiff_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2074 :
		typedef void ( idClass::*eventCallback_ififf_t )( const intptr_t, const float, const intptr_t, const float, const float );
		( this->*( eventCallback_ififf_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2075 :
		typedef void ( idClass::*eventCallback_ffiff_t )( const float, const float, const intptr_t, const float, const float );
		( this->*( eventCallback_ffiff_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2076 :
		typedef void ( idClass::*eventCallback_iifff_t )( const intptr_t, const intptr_t, const float, const float, const float );
		( this->*( eventCallback_iifff_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2077 :
		typedef void ( idClass::*eventCallback_fifff_t )( const float, const intptr_t, const float, const float, const float );
		( this->*( eventCallback_fifff_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2078 :
		typedef void ( idClass::*eventCallback_iffff_t )( const intptr_t, const float, const float, const float, const float );
		( this->*( eventCallback_iffff_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ] );
		break;

	case 2079 :
		typedef void ( idClass::*eventCallback_fffff_t )( const float, const float, const float, const float, const float );
		( this->*( eventCallback_fffff_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ] );
		break;

	/*******************************************************

		6 args

	*******************************************************/

	case 4096 :
		typedef void ( idClass::*eventCallback_iiiiii_t )( const intptr_t, const intptr_t, const intptr_t, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_iiiiii_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4097 :
		typedef void ( idClass::*eventCallback_fiiiii_t )( const float, const intptr_t, const intptr_t, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_fiiiii_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4098 :
		typedef void ( idClass::*eventCallback_ifiiii_t )( const intptr_t, const float, const intptr_t, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_ifiiii_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4099 :
		typedef void ( idClass::*eventCallback_ffiiii_t )( const float, const float, const intptr_t, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_ffiiii_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4100 :
		typedef void ( idClass::*eventCallback_iifiii_t )( const intptr_t, const intptr_t, const float, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_iifiii_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4101 :
		typedef void ( idClass::*eventCallback_fifiii_t )( const float, const intptr_t, const float, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_fifiii_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4102 :
		typedef void ( idClass::*eventCallback_iffiii_t )( const intptr_t, const float, const float, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_iffiii_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4103 :
		typedef void ( idClass::*eventCallback_fffiii_t )( const float, const float, const float, const intptr_t, const intptr_t, const intptr_t );
		( this->*( eventCallback_fffiii_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4104 :
		typedef void ( idClass::*eventCallback_iiifii_t )( const intptr_t, const intptr_t, const intptr_t, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_iiifii_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4105 :
		typedef void ( idClass::*eventCallback_fiifii_t )( const float, const intptr_t, const intptr_t, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_fiifii_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4106 :
		typedef void ( idClass::*eventCallback_ififii_t )( const intptr_t, const float, const intptr_t, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_ififii_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4107 :
		typedef void ( idClass::*eventCallback_ffifii_t )( const float, const float, const intptr_t, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_ffifii_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4108 :
		typedef void ( idClass::*eventCallback_iiffii_t )( const intptr_t, const intptr_t, const float, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_iiffii_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4109 :
		typedef void ( idClass::*eventCallback_fiffii_t )( const float, const intptr_t, const float, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_fiffii_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4110 :
		typedef void ( idClass::*eventCallback_ifffii_t )( const intptr_t, const float, const float, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_ifffii_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4111 :
		typedef void ( idClass::*eventCallback_ffffii_t )( const float, const float, const float, const float, const intptr_t, const intptr_t );
		( this->*( eventCallback_ffffii_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], data[ 5 ] );
		break;

	case 4112 :
		typedef void ( idClass::*eventCallback_iiiifi_t )( const intptr_t, const intptr_t, const intptr_t, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_iiiifi_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4113 :
		typedef void ( idClass::*eventCallback_fiiifi_t )( const float, const intptr_t, const intptr_t, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_fiiifi_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4114 :
		typedef void ( idClass::*eventCallback_ifiifi_t )( const intptr_t, const float, const intptr_t, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_ifiifi_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4115 :
		typedef void ( idClass::*eventCallback_ffiifi_t )( const float, const float, const intptr_t, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_ffiifi_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4116 :
		typedef void ( idClass::*eventCallback_iififi_t )( const intptr_t, const intptr_t, const float, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_iififi_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4117 :
		typedef void ( idClass::*eventCallback_fififi_t )( const float, const intptr_t, const float, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_fififi_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4118 :
		typedef void ( idClass::*eventCallback_iffifi_t )( const intptr_t, const float, const float, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_iffifi_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4119 :
		typedef void ( idClass::*eventCallback_fffifi_t )( const float, const float, const float, const intptr_t, const float, const intptr_t );
		( this->*( eventCallback_fffifi_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4120 :
		typedef void ( idClass::*eventCallback_iiiffi_t )( const intptr_t, const intptr_t, const intptr_t, const float, const float, const intptr_t );
		( this->*( eventCallback_iiiffi_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4121 :
		typedef void ( idClass::*eventCallback_fiiffi_t )( const float, const intptr_t, const intptr_t, const float, const float, const intptr_t );
		( this->*( eventCallback_fiiffi_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4122 :
		typedef void ( idClass::*eventCallback_ififfi_t )( const intptr_t, const float, const intptr_t, const float, const float, const intptr_t );
		( this->*( eventCallback_ififfi_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4123 :
		typedef void ( idClass::*eventCallback_ffiffi_t )( const float, const float, const intptr_t, const float, const float, const intptr_t );
		( this->*( eventCallback_ffiffi_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4124 :
		typedef void ( idClass::*eventCallback_iifffi_t )( const intptr_t, const intptr_t, const float, const float, const float, const intptr_t );
		( this->*( eventCallback_iifffi_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4125 :
		typedef void ( idClass::*eventCallback_fifffi_t )( const float, const intptr_t, const float, const float, const float, const intptr_t );
		( this->*( eventCallback_fifffi_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4126 :
		typedef void ( idClass::*eventCallback_iffffi_t )( const intptr_t, const float, const float, const float, const float, const intptr_t );
		( this->*( eventCallback_iffffi_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4127 :
		typedef void ( idClass::*eventCallback_fffffi_t )( const float, const float, const float, const float, const float, const intptr_t );
		( this->*( eventCallback_fffffi_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], data[ 5 ] );
		break;

	case 4128 :
		typedef void ( idClass::*eventCallback_iiiiif_t )( const intptr_t, const intptr_t, const intptr_t, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_iiiiif_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4129 :
		typedef void ( idClass::*eventCallback_fiiiif_t )( const float, const intptr_t, const intptr_t, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_fiiiif_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4130 :
		typedef void ( idClass::*eventCallback_ifiiif_t )( const intptr_t, const float, const intptr_t, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_ifiiif_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4131 :
		typedef void ( idClass::*eventCallback_ffiiif_t )( const float, const float, const intptr_t, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_ffiiif_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4132 :
		typedef void ( idClass::*eventCallback_iifiif_t )( const intptr_t, const intptr_t, const float, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_iifiif_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4133 :
		typedef void ( idClass::*eventCallback_fifiif_t )( const float, const intptr_t, const float, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_fifiif_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4134 :
		typedef void ( idClass::*eventCallback_iffiif_t )( const intptr_t, const float, const float, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_iffiif_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4135 :
		typedef void ( idClass::*eventCallback_fffiif_t )( const float, const float, const float, const intptr_t, const intptr_t, const float );
		( this->*( eventCallback_fffiif_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4136 :
		typedef void ( idClass::*eventCallback_iiifif_t )( const intptr_t, const intptr_t, const intptr_t, const float, const intptr_t, const float );
		( this->*( eventCallback_iiifif_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4137 :
		typedef void ( idClass::*eventCallback_fiifif_t )( const float, const intptr_t, const intptr_t, const float, const intptr_t, const float );
		( this->*( eventCallback_fiifif_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4138 :
		typedef void ( idClass::*eventCallback_ififif_t )( const intptr_t, const float, const intptr_t, const float, const intptr_t, const float );
		( this->*( eventCallback_ififif_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4139 :
		typedef void ( idClass::*eventCallback_ffifif_t )( const float, const float, const intptr_t, const float, const intptr_t, const float );
		( this->*( eventCallback_ffifif_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4140 :
		typedef void ( idClass::*eventCallback_iiffif_t )( const intptr_t, const intptr_t, const float, const float, const intptr_t, const float );
		( this->*( eventCallback_iiffif_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4141 :
		typedef void ( idClass::*eventCallback_fiffif_t )( const float, const intptr_t, const float, const float, const intptr_t, const float );
		( this->*( eventCallback_fiffif_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4142 :
		typedef void ( idClass::*eventCallback_ifffif_t )( const intptr_t, const float, const float, const float, const intptr_t, const float );
		( this->*( eventCallback_ifffif_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4143 :
		typedef void ( idClass::*eventCallback_ffffif_t )( const float, const float, const float, const float, const intptr_t, const float );
		( this->*( eventCallback_ffffif_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4144 :
		typedef void ( idClass::*eventCallback_iiiiff_t )( const intptr_t, const intptr_t, const intptr_t, const intptr_t, const float, const float );
		( this->*( eventCallback_iiiiff_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4145 :
		typedef void ( idClass::*eventCallback_fiiiff_t )( const float, const intptr_t, const intptr_t, const intptr_t, const float, const float );
		( this->*( eventCallback_fiiiff_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4146 :
		typedef void ( idClass::*eventCallback_ifiiff_t )( const intptr_t, const float, const intptr_t, const intptr_t, const float, const float );
		( this->*( eventCallback_ifiiff_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4147 :
		typedef void ( idClass::*eventCallback_ffiiff_t )( const float, const float, const intptr_t, const intptr_t, const float, const float );
		( this->*( eventCallback_ffiiff_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4148 :
		typedef void ( idClass::*eventCallback_iififf_t )( const intptr_t, const intptr_t, const float, const intptr_t, const float, const float );
		( this->*( eventCallback_iififf_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4149 :
		typedef void ( idClass::*eventCallback_fififf_t )( const float, const intptr_t, const float, const intptr_t, const float, const float );
		( this->*( eventCallback_fififf_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4150 :
		typedef void ( idClass::*eventCallback_iffiff_t )( const intptr_t, const float, const float, const intptr_t, const float, const float );
		( this->*( eventCallback_iffiff_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4151 :
		typedef void ( idClass::*eventCallback_fffiff_t )( const float, const float, const float, const intptr_t, const float, const float );
		( this->*( eventCallback_fffiff_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4152 :
		typedef void ( idClass::*eventCallback_iiifff_t )( const intptr_t, const intptr_t, const intptr_t, const float, const float, const float );
		( this->*( eventCallback_iiifff_t )callback )( data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4153 :
		typedef void ( idClass::*eventCallback_fiifff_t )( const float, const intptr_t, const intptr_t, const float, const float, const float );
		( this->*( eventCallback_fiifff_t )callback )( *( float * )&data[ 0 ], data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4154 :
		typedef void ( idClass::*eventCallback_ififff_t )( const intptr_t, const float, const intptr_t, const float, const float, const float );
		( this->*( eventCallback_ififff_t )callback )( data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4155 :
		typedef void ( idClass::*eventCallback_ffifff_t )( const float, const float, const intptr_t, const float, const float, const float );
		( this->*( eventCallback_ffifff_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4156 :
		typedef void ( idClass::*eventCallback_iiffff_t )( const intptr_t, const intptr_t, const float, const float, const float, const float );
		( this->*( eventCallback_iiffff_t )callback )( data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4157 :
		typedef void ( idClass::*eventCallback_fiffff_t )( const float, const intptr_t, const float, const float, const float, const float );
		( this->*( eventCallback_fiffff_t )callback )( *( float * )&data[ 0 ], data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4158 :
		typedef void ( idClass::*eventCallback_ifffff_t )( const intptr_t, const float, const float, const float, const float, const float );
		( this->*( eventCallback_ifffff_t )callback )( data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

	case 4159 :
		typedef void ( idClass::*eventCallback_ffffff_t )( const float, const float, const float, const float, const float, const float );
		( this->*( eventCallback_ffffff_t )callback )( *( float * )&data[ 0 ], *( float * )&data[ 1 ], *( float * )&data[ 2 ], *( float * )&data[ 3 ], *( float * )&data[ 4 ], *( float * )&data[ 5 ] );
		break;

