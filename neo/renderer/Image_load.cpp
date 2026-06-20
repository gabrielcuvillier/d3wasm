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
#include "idlib/hashing/MD4.h"
#include "renderer/tr_local.h"

#include "renderer/Image.h"


int MakePowerOfTwo( int num ) {
	int		pot;
	for (pot = 1 ; pot < num ; pot<<=1) {
	}
	return pot;
}

/*
================
BitsForInternalFormat

Used for determining memory utilization
================
*/
int idImage::BitsForInternalFormat( int internalFormat ) const {
	switch ( internalFormat ) {
	case 1:
	case 2:
	case 3:
	case 4:
		return 32;
	case GL_RGBA4:
		return 16;
	case GL_RGB5_A1:
		return 16;
	default:
		common->Error( "R_BitsForInternalFormat: BAD FORMAT:%i", internalFormat );
	}
	return 0;
}

/*
==================
UploadCompressedNormalMap

Create a 256 color palette to be used by compressed normal maps
==================
*/
void idImage::UploadCompressedNormalMap( int width, int height, const byte *rgba, int mipLevel ) {
	byte	*normals;
	const byte	*in;
	byte	*out;
	int		i, j;
	int		x, y, z;
	int		row;

	// OpenGL's pixel packing rule
	row = width < 4 ? 4 : width;

	normals = (byte *)_alloca( row * height );
	if ( !normals ) {
		common->Error( "R_UploadCompressedNormalMap: _alloca failed" );
	}

	in = rgba;
	out = normals;
	for ( i = 0 ; i < height ; i++, out += row, in += width * 4 ) {
		for ( j = 0 ; j < width ; j++ ) {
			x = in[ j * 4 + 0 ];
			y = in[ j * 4 + 1 ];
			z = in[ j * 4 + 2 ];

			int c;
			if ( x == 128 && y == 128 && z == 128 ) {
				// the "nullnormal" color
				c = 255;
			} else {
				c = ( globalImages->originalToCompressed[x] << 4 ) | globalImages->originalToCompressed[y];
				if ( c == 255 ) {
					c = 254;	// don't use the nullnormal color
				}
			}
			out[j] = c;
		}
	}

	if ( mipLevel == 0 ) {
		// Optionally write out the paletized normal map to a .tga
		if ( globalImages->image_writeNormalTGAPalletized.GetBool() ) {
			char filename[MAX_IMAGE_NAME];
			ImageProgramStringToCompressedFileName( imgName, filename );
			char *ext = strrchr(filename, '.');
			if ( ext ) {
				strcpy(ext, "_pal.tga");
				R_WritePalTGA( filename, normals, globalImages->compressedPalette, width, height);
			}
		}
	}
}


//=======================================================================


static byte	mipBlendColors[16][4] = {
	{0,0,0,0},
	{255,0,0,128},
	{0,255,0,128},
	{0,0,255,128},
	{255,0,0,128},
	{0,255,0,128},
	{0,0,255,128},
	{255,0,0,128},
	{0,255,0,128},
	{0,0,255,128},
	{255,0,0,128},
	{0,255,0,128},
	{0,0,255,128},
	{255,0,0,128},
	{0,255,0,128},
	{0,0,255,128},
};

/*
==================
SetImageFilterAndRepeat
==================
*/
void idImage::SetImageFilterAndRepeat() const {
	// set the minimize / maximize filtering
	switch( filter ) {
	case TF_DEFAULT:
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, globalImages->textureMinFilter );
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, globalImages->textureMaxFilter );
		break;
	case TF_LINEAR:
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		break;
	case TF_NEAREST:
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		break;
	default:
		common->FatalError( "R_CreateImage: bad texture filter" );
	}

	if ( glConfig.anisotropicAvailable ) {
		// only do aniso filtering on mip mapped images
		if ( filter == TF_DEFAULT ) {
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, globalImages->textureAnisotropy );
		} else {
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1 );
		}
	}

	// set the wrap/clamp modes
	switch( repeat ) {
	case TR_REPEAT:
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		break;
	case TR_CLAMP_TO_BORDER:
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		// Disabled for OES2
		//qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
		//qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
		break;
	case TR_CLAMP_TO_ZERO:
	case TR_CLAMP_TO_ZERO_ALPHA:
	case TR_CLAMP:
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		break;
	default:
		common->FatalError( "R_CreateImage: bad texture repeat" );
	}
}

/*
================
idImage::Downsize
helper function that takes the current width/height and might make them smaller
================
*/
void idImage::GetDownsize( int &scaled_width, int &scaled_height ) const {
	int size = 0;

	// perform optional picmip operation to save texture memory
	if ( depth == TD_SPECULAR && globalImages->image_downSizeSpecular.GetInteger() ) {
		size = globalImages->image_downSizeSpecularLimit.GetInteger();
		if ( size == 0 ) {
			size = 64;
		}
	} else if ( depth == TD_BUMP && globalImages->image_downSizeBump.GetInteger() ) {
		size = globalImages->image_downSizeBumpLimit.GetInteger();
		if ( size == 0 ) {
			size = 64;
		}
	} else if ( ( allowDownSize || globalImages->image_forceDownSize.GetBool() ) && globalImages->image_downSize.GetInteger() ) {
		size = globalImages->image_downSizeLimit.GetInteger();
		if ( size == 0 ) {
			size = 256;
		}
	}

	if ( size > 0 ) {
		while ( scaled_width > size || scaled_height > size ) {
			if ( scaled_width > 1 ) {
				scaled_width >>= 1;
			}
			if ( scaled_height > 1 ) {
				scaled_height >>= 1;
			}
		}
	}

	// clamp to minimum size
	if ( scaled_width < 1 ) {
		scaled_width = 1;
	}
	if ( scaled_height < 1 ) {
		scaled_height = 1;
	}

	// clamp size to the hardware specific upper limit
	// scale both axis down equally so we don't have to
	// deal with a half mip resampling
	// This causes a 512*256 texture to sample down to
	// 256*128 on a voodoo3, even though it could be 256*256
	while ( scaled_width > glConfig.maxTextureSize
		|| scaled_height > glConfig.maxTextureSize ) {
		scaled_width >>= 1;
		scaled_height >>= 1;
	}
}

/*
================
GenerateImage

The alpha channel bytes should be 255 if you don't
want the channel.

We need a material characteristic to ask for specific texture modes.

Designed limitations of flexibility:

No support for texture borders.

No support for texture border color.

No support for texture environment colors or GL_BLEND or GL_DECAL
texture environments, because the automatic optimization to single
or dual component textures makes those modes potentially undefined.

No non-power-of-two images.

No palettized textures.

There is no way to specify separate wrap/clamp values for S and T

There is no way to specify explicit mip map levels

================
*/
void idImage::GenerateImage( const byte *pic, int width, int height,
					   textureFilter_t filterParm, bool allowDownSizeParm,
					   textureRepeat_t repeatParm, textureDepth_t depthParm ) {
	bool	preserveBorder;
	byte		*scaledBuffer;
	int			scaled_width, scaled_height;
	byte		*shrunk;

	PurgeImage();

	filter = filterParm;
	allowDownSize = allowDownSizeParm;
	repeat = repeatParm;
	depth = depthParm;

	// if we don't have a rendering context, just return after we
	// have filled in the parms.  We must have the values set, or
	// an image match from a shader before OpenGL starts would miss
	// the generated texture
	if ( !glConfig.isInitialized ) {
		return;
	}

	// don't let mip mapping smear the texture into the clamped border
	if ( repeat == TR_CLAMP_TO_ZERO ) {
		preserveBorder = true;
	} else {
		preserveBorder = false;
	}

	// make sure it is a power of 2
	scaled_width = MakePowerOfTwo( width );
	scaled_height = MakePowerOfTwo( height );

	if ( scaled_width != width || scaled_height != height ) {
		common->Error( "R_CreateImage: not a power of 2 image" );
	}

	// Optionally modify our width/height based on options/hardware
	GetDownsize( scaled_width, scaled_height );

	scaledBuffer = NULL;

	// generate the texture number
	qglGenTextures( 1, &texnum );

	// select proper internal format before we resample
	internalFormat = GL_RGBA;

	// copy or resample data as appropriate for first MIP level
	if ( ( scaled_width == width ) && ( scaled_height == height ) ) {
		// we must copy even if unchanged, because the border zeroing
		// would otherwise modify const data
		scaledBuffer = (byte *)R_StaticAlloc( sizeof( unsigned ) * scaled_width * scaled_height );
		memcpy (scaledBuffer, pic, width*height*4);
	} else {
		// resample down as needed (FIXME: this doesn't seem like it resamples anymore!)
		// scaledBuffer = R_ResampleTexture( pic, width, height, width >>= 1, height >>= 1 );
		scaledBuffer = R_MipMap( pic, width, height, preserveBorder );
		width >>= 1;
		height >>= 1;
		if ( width < 1 ) {
			width = 1;
		}
		if ( height < 1 ) {
			height = 1;
		}

		while ( width > scaled_width || height > scaled_height ) {
			shrunk = R_MipMap( scaledBuffer, width, height, preserveBorder );
			R_StaticFree( scaledBuffer );
			scaledBuffer = shrunk;

			width >>= 1;
			height >>= 1;
			if ( width < 1 ) {
				width = 1;
			}
			if ( height < 1 ) {
				height = 1;
			}
		}

		// one might have shrunk down below the target size
		scaled_width = width;
		scaled_height = height;
	}

	uploadHeight = scaled_height;
	uploadWidth = scaled_width;
	type = TT_2D;

	// zero the border if desired, allowing clamped projection textures
	// even after picmip resampling or careless artists.
	if ( repeat == TR_CLAMP_TO_ZERO ) {
		byte	rgba[4];

		rgba[0] = rgba[1] = rgba[2] = 0;
		rgba[3] = 255;
		R_SetBorderTexels( (byte *)scaledBuffer, width, height, rgba );
	}
	if ( repeat == TR_CLAMP_TO_ZERO_ALPHA ) {
		byte	rgba[4];

		rgba[0] = rgba[1] = rgba[2] = 255;
		rgba[3] = 0;
		R_SetBorderTexels( (byte *)scaledBuffer, width, height, rgba );
	}

	if ( generatorFunction == NULL && ( (depth == TD_BUMP && globalImages->image_writeNormalTGA.GetBool()) || (depth != TD_BUMP && globalImages->image_writeTGA.GetBool()) ) ) {
		// Optionally write out the texture to a .tga
		char filename[MAX_IMAGE_NAME];
		ImageProgramStringToCompressedFileName( imgName, filename );
		char *ext = strrchr(filename, '.');
		if ( ext ) {
			strcpy( ext, ".tga" );
			// swap the red/alpha for the write
			/*
			if ( depth == TD_BUMP ) {
				for ( int i = 0; i < scaled_width * scaled_height * 4; i += 4 ) {
					scaledBuffer[ i ] = scaledBuffer[ i + 3 ];
					scaledBuffer[ i + 3 ] = 0;
				}
			}
			*/
			R_WriteTGA( filename, scaledBuffer, scaled_width, scaled_height, false );

			// put it back
			/*
			if ( depth == TD_BUMP ) {
				for ( int i = 0; i < scaled_width * scaled_height * 4; i += 4 ) {
					scaledBuffer[ i + 3 ] = scaledBuffer[ i ];
					scaledBuffer[ i ] = 0;
				}
			}
			*/
		}
	}

	// swap the red and alpha for rxgb support
	// do this even on tga normal maps so we only have to use
	// one fragment program
	if ( depth == TD_BUMP ) {
		for ( int i = 0; i < scaled_width * scaled_height * 4; i += 4 ) {
			scaledBuffer[ i + 3 ] = scaledBuffer[ i ];
			scaledBuffer[ i ] = 0;
		}
	}
	// upload the main image level
	Bind();


	qglTexImage2D( GL_TEXTURE_2D, 0, internalFormat, scaled_width, scaled_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scaledBuffer );

	// create and upload the mip map levels, which we do in all cases, even if we don't think they are needed
	int		miplevel;

	miplevel = 0;
	while ( scaled_width > 1 || scaled_height > 1 ) {
		// preserve the border after mip map unless repeating
		shrunk = R_MipMap( scaledBuffer, scaled_width, scaled_height, preserveBorder );
		R_StaticFree( scaledBuffer );
		scaledBuffer = shrunk;

		scaled_width >>= 1;
		scaled_height >>= 1;
		if ( scaled_width < 1 ) {
			scaled_width = 1;
		}
		if ( scaled_height < 1 ) {
			scaled_height = 1;
		}
		miplevel++;

		// this is a visualization tool that shades each mip map
		// level with a different color so you can see the
		// rasterizer's texture level selection algorithm
		// Changing the color doesn't help with lumminance/alpha/intensity formats...
		if ( depth == TD_DIFFUSE && globalImages->image_colorMipLevels.GetBool() ) {
			R_BlendOverTexture( (byte *)scaledBuffer, scaled_width * scaled_height, mipBlendColors[miplevel] );
		}

		// upload the mip map
			qglTexImage2D( GL_TEXTURE_2D, miplevel, internalFormat, scaled_width, scaled_height,
				0, GL_RGBA, GL_UNSIGNED_BYTE, scaledBuffer );
	}

	if ( scaledBuffer != 0 ) {
		R_StaticFree( scaledBuffer );
	}

	SetImageFilterAndRepeat();

	// see if we messed anything up
	GL_CheckErrors();
}

/*
====================
GenerateCubeImage

Non-square cube sides are not allowed
====================
*/
void idImage::GenerateCubeImage( const byte *pic[6], int size,
					   textureFilter_t filterParm, bool allowDownSizeParm,
					   textureDepth_t depthParm ) {
	int			scaled_width, scaled_height;
	int			width, height;
	int			i;

	PurgeImage();

	filter = filterParm;
	allowDownSize = allowDownSizeParm;
	depth = depthParm;

	type = TT_CUBIC;

	// if we don't have a rendering context, just return after we
	// have filled in the parms.  We must have the values set, or
	// an image match from a shader before OpenGL starts would miss
	// the generated texture
	if ( !glConfig.isInitialized ) {
		return;
	}

	width = height = size;

	// generate the texture number
	qglGenTextures( 1, &texnum );

	// select proper internal format before we resample
	internalFormat = GL_RGBA;

	// don't bother with downsample for now
	scaled_width = width;
	scaled_height = height;

	uploadHeight = scaled_height;
	uploadWidth = scaled_width;

	Bind();

	// no other clamp mode makes sense
	qglTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	qglTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// set the minimize / maximize filtering
	switch( filter ) {
	case TF_DEFAULT:
		qglTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, globalImages->textureMinFilter );
		qglTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, globalImages->textureMaxFilter );
		break;
	case TF_LINEAR:
		qglTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		qglTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		break;
	case TF_NEAREST:
		qglTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		qglTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		break;
	default:
		common->FatalError( "R_CreateImage: bad texture filter" );
	}

	// upload the base level
	// FIXME: support GL_COLOR_INDEX8_EXT?
	for ( i = 0 ; i < 6 ; i++ ) {
		qglTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, internalFormat, scaled_width, scaled_height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, pic[i] );
	}


	// create and upload the mip map levels
	int		miplevel;
	byte	*shrunk[6];

	for ( i = 0 ; i < 6 ; i++ ) {
		shrunk[i] = R_MipMap( pic[i], scaled_width, scaled_height, false );
	}

	miplevel = 1;
	while ( scaled_width > 1 ) {
		for ( i = 0 ; i < 6 ; i++ ) {
			byte	*shrunken;

			qglTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, miplevel, internalFormat,
				scaled_width / 2, scaled_height / 2, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, shrunk[i] );

			if ( scaled_width > 2 ) {
				shrunken = R_MipMap( shrunk[i], scaled_width/2, scaled_height/2, false );
			} else {
				shrunken = NULL;
			}

			R_StaticFree( shrunk[i] );
			shrunk[i] = shrunken;
		}

		scaled_width >>= 1;
		scaled_height >>= 1;
		miplevel++;
	}

	// see if we messed anything up
	GL_CheckErrors();
}


/*
================
ImageProgramStringToFileCompressedFileName
================
*/
void idImage::ImageProgramStringToCompressedFileName( const char *imageProg, char *fileName ) const {
	const char	*s;
	char	*f;

	strcpy( fileName, "dds/" );
	f = fileName + strlen( fileName );

	int depth = 0;

	// convert all illegal characters to underscores
	// this could conceivably produce a duplicated mapping, but we aren't going to worry about it
	for ( s = imageProg ; *s ; s++ ) {
		if ( *s == '/' || *s == '\\' || *s == '(') {
			if ( depth < 4 ) {
				*f = '/';
				depth ++;
			} else {
				*f = ' ';
			}
			f++;
		} else if ( *s == '<' || *s == '>' || *s == ':' || *s == '|' || *s == '"' || *s == '.' ) {
			*f = '_';
			f++;
		} else if ( *s == ' ' && *(f-1) == '/' ) {	// ignore a space right after a slash
		} else if ( *s == ')' || *s == ',' ) {		// always ignore these
		} else {
			*f = *s;
			f++;
		}
	}
	*f++ = 0;
	strcat( fileName, ".dds" );
}

/*
==================
NumLevelsForImageSize
==================
*/
int	idImage::NumLevelsForImageSize( int width, int height ) const {
	int	numLevels = 1;

	while ( width > 1 || height > 1 ) {
		numLevels++;
		width >>= 1;
		height >>= 1;
	}

	return numLevels;
}

/*
===============
ActuallyLoadImage

Absolutely every image goes through this path
On exit, the idImage will have a valid OpenGL texture number that can be bound
===============
*/
void	idImage::ActuallyLoadImage( bool fromBackEnd ) {
	int		width, height;
	byte	*pic;

	// this is the ONLY place generatorFunction will ever be called
	if ( generatorFunction ) {
		generatorFunction( this );
		return;
	}

	//
	// load the image from disk
	//
	if ( cubeFiles != CF_2D ) {
		byte	*pics[6];

		// we don't check for pre-compressed cube images currently
		R_LoadCubeImages( imgName, cubeFiles, pics, &width, &timestamp );

		if ( pics[0] == NULL ) {
			common->Warning( "Couldn't load cube image: %s", imgName.c_str() );
			MakeDefault();
			return;
		}

		GenerateCubeImage( (const byte **)pics, width, filter, allowDownSize, depth );

		for ( int i = 0 ; i < 6 ; i++ ) {
			if ( pics[i] ) {
				R_StaticFree( pics[i] );
			}
		}
	} else {
		// see if we have a pre-generated image file that is
		// already image processed and compressed

		R_LoadImageProgram( imgName, &pic, &width, &height, &timestamp, &depth );

		if ( pic == NULL ) {
			common->Warning( "Couldn't load image: %s", imgName.c_str() );
			MakeDefault();
			return;
		}

		// build a hash for checking duplicate image files
		// NOTE: takes about 10% of image load times (SD)
		// may not be strictly necessary, but some code uses it, so let's leave it in
		imageHash = MD4_BlockChecksum( pic, width * height * 4 );

		GenerateImage( pic, width, height, filter, allowDownSize, repeat, depth );
		timestamp = timestamp;

		R_StaticFree( pic );
	}
}

//=========================================================================================================

/*
===============
PurgeImage
===============
*/
void idImage::PurgeImage() {
	if ( texnum != TEXTURE_NOT_LOADED ) {
		qglDeleteTextures( 1, &texnum );	// this should be the ONLY place it is ever called!
		texnum = TEXTURE_NOT_LOADED;
	}
}

/*
==============
Bind

Automatically enables 2D mapping, cube mapping, or 3D texturing if needed
==============
*/
void idImage::Bind() {
	// load the image if necessary (FIXME: not SMP safe!)
	if ( texnum == TEXTURE_NOT_LOADED ) {
#ifdef __EMSCRIPTEN__
		return;
#else
		// load the image on demand here, which isn't our normal game operating mode
		ActuallyLoadImage( true );
#endif
	}


	// bump our statistic counters
	frameUsed = backEnd.frameCount;
	bindCount++;

	// bind the texture
	if ( type == TT_2D ) {
		qglBindTexture( GL_TEXTURE_2D, texnum );
	} else if ( type == TT_CUBIC ) {
		qglBindTexture( GL_TEXTURE_CUBE_MAP, texnum );
	}
}

/*
==============
BindFragment

Fragment programs explicitly say which type of map they want, so we don't need to
do any enable / disable changes
==============
*/
void idImage::BindFragment() {

	// load the image if necessary (FIXME: not SMP safe!)
	if ( texnum == TEXTURE_NOT_LOADED ) {
#ifdef __EMSCRIPTEN__
		return;
#else
		// load the image on demand here, which isn't our normal game operating mode
		ActuallyLoadImage( true );
#endif
	}


	// bump our statistic counters
	frameUsed = backEnd.frameCount;
	bindCount++;

	// bind the texture
	if ( type == TT_2D ) {
		qglBindTexture( GL_TEXTURE_2D, texnum );
	} else if ( type == TT_CUBIC ) {
		qglBindTexture( GL_TEXTURE_CUBE_MAP, texnum );
	}
}


/*
====================
CopyFramebuffer
====================
*/
void idImage::CopyFramebuffer( int x, int y, int imageWidth, int imageHeight, bool useOversizedBuffer ) {
	Bind();

	if ( cvarSystem->GetCVarBool( "g_lowresFullscreenFX" ) ) {
		imageWidth = 512;
		imageHeight = 512;
	}

	// if the size isn't a power of 2, the image must be increased in size
	int	potWidth, potHeight;

	potWidth = MakePowerOfTwo( imageWidth );
	potHeight = MakePowerOfTwo( imageHeight );

	GetDownsize( imageWidth, imageHeight );
	GetDownsize( potWidth, potHeight );

	//Disabled for OES2
	//qglReadBuffer( GL_BACK );

	// only resize if the current dimensions can't hold it at all,
	// otherwise subview renderings could thrash this
	if ( ( useOversizedBuffer && ( uploadWidth < potWidth || uploadHeight < potHeight ) )
		|| ( !useOversizedBuffer && ( uploadWidth != potWidth || uploadHeight != potHeight ) ) ) {
		uploadWidth = potWidth;
		uploadHeight = potHeight;
		if ( potWidth == imageWidth && potHeight == imageHeight ) {
			qglCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, x, y, imageWidth, imageHeight, 0 );
		} else {
			byte	*junk;
			// we need to create a dummy image with power of two dimensions,
			// then do a qglCopyTexSubImage2D of the data we want
			// this might be a 16+ meg allocation, which could fail on _alloca
			junk = (byte *)Mem_Alloc( potWidth * potHeight * 4 );
			memset( junk, 0, potWidth * potHeight * 4 );		//!@#
			qglTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, potWidth, potHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, junk );

			Mem_Free( junk );

			qglCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, x, y, imageWidth, imageHeight );
		}
	} else {
		// otherwise, just subimage upload it so that drivers can tell we are going to be changing
		// it and don't try and do a texture compression or some other silliness
		qglCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, x, y, imageWidth, imageHeight );
	}

	// if the image isn't a full power of two, duplicate an extra row and/or column to fix bilerps
	if ( imageWidth != potWidth ) {
		qglCopyTexSubImage2D( GL_TEXTURE_2D, 0, imageWidth, 0, x+imageWidth-1, y, 1, imageHeight );
	}
	if ( imageHeight != potHeight ) {
		qglCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, imageHeight, x, y+imageHeight-1, imageWidth, 1 );
	}

	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	backEnd.c_copyFrameBuffer++;
}

/*
====================
CopyDepthbuffer

This should just be part of copyFramebuffer once we have a proper image type field
====================
*/
void idImage::CopyDepthbuffer( int x, int y, int imageWidth, int imageHeight ) {
	Bind();

	// if the size isn't a power of 2, the image must be increased in size
	int	potWidth, potHeight;

	potWidth = MakePowerOfTwo( imageWidth );
	potHeight = MakePowerOfTwo( imageHeight );

	if ( uploadWidth != potWidth || uploadHeight != potHeight ) {
		uploadWidth = potWidth;
		uploadHeight = potHeight;
		if ( potWidth == imageWidth && potHeight == imageHeight ) {
			qglCopyTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, x, y, imageWidth, imageHeight, 0 );
		} else {
			// we need to create a dummy image with power of two dimensions,
			// then do a qglCopyTexSubImage2D of the data we want
			qglTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, potWidth, potHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL );
			qglCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, x, y, imageWidth, imageHeight );
		}
	} else {
		// otherwise, just subimage upload it so that drivers can tell we are going to be changing
		// it and don't try and do a texture compression or some other silliness
		qglCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, x, y, imageWidth, imageHeight );
	}

//	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
//	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
}

/*
=============
RB_UploadScratchImage

if rows = cols * 6, assume it is a cube map animation
=============
*/
void idImage::UploadScratch( const byte *data, int cols, int rows ) {
	int			i;

	// if rows = cols * 6, assume it is a cube map animation
	if ( rows == cols * 6 ) {
		if ( type != TT_CUBIC ) {
			type = TT_CUBIC;
			uploadWidth = -1;	// for a non-sub upload
		}

		Bind();

		rows /= 6;
		// if the scratchImage isn't in the format we want, specify it as a new texture
		if ( cols != uploadWidth || rows != uploadHeight ) {
			uploadWidth = cols;
			uploadHeight = rows;

			// upload the base level
			for ( i = 0 ; i < 6 ; i++ ) {
				qglTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA, cols, rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, data + cols*rows*4*i );
			}
		} else {
			// otherwise, just subimage upload it so that drivers can tell we are going to be changing
			// it and don't try and do a texture compression
			for ( i = 0 ; i < 6 ; i++ ) {
				qglTexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, 0, 0, cols, rows,
					GL_RGBA, GL_UNSIGNED_BYTE, data + cols*rows*4*i );
			}
		}
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		// no other clamp mode makes sense
		qglTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		qglTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	} else {
		// otherwise, it is a 2D image
		if ( type != TT_2D ) {
			type = TT_2D;
			uploadWidth = -1;	// for a non-sub upload
		}

		Bind();

		// if the scratchImage isn't in the format we want, specify it as a new texture
		if ( cols != uploadWidth || rows != uploadHeight ) {
			uploadWidth = cols;
			uploadHeight = rows;
			qglTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, cols, rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
		} else {
			// otherwise, just subimage upload it so that drivers can tell we are going to be changing
			// it and don't try and do a texture compression
			qglTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, cols, rows, GL_RGBA, GL_UNSIGNED_BYTE, data );
		}
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		// these probably should be clamp, but we have a lot of issues with editor
		// geometry coming out with texcoords slightly off one side, resulting in
		// a smear across the entire polygon
#if 1
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
#else
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
#endif
	}
}


void idImage::SetClassification( int tag ) {
	classification = tag;
}

/*
==================
StorageSize
==================
*/
int idImage::StorageSize() const {
	int		baseSize;

	if ( texnum == TEXTURE_NOT_LOADED ) {
		return 0;
	}

	switch ( type ) {
	default:
	case TT_2D:
		baseSize = uploadWidth*uploadHeight;
		break;
	case TT_CUBIC:
		baseSize = 6 * uploadWidth*uploadHeight;
		break;
	}

	baseSize *= BitsForInternalFormat( internalFormat );

	baseSize /= 8;

	// account for mip mapping
	baseSize = baseSize * 4 / 3;

	return baseSize;
}

/*
==================
Print
==================
*/
void idImage::Print() const {
	if ( generatorFunction ) {
		common->Printf( "F" );
	} else {
		common->Printf( " " );
	}

	switch ( type ) {
	case TT_2D:
		common->Printf( " " );
		break;
	case TT_CUBIC:
		common->Printf( "C" );
		break;
	case TT_RECT:
		common->Printf( "R" );
		break;
	default:
		common->Printf( "<BAD TYPE:%i>", type );
		break;
	}

	common->Printf( "%4i %4i ",	uploadWidth, uploadHeight );

	switch( filter ) {
	case TF_DEFAULT:
		common->Printf( "dflt " );
		break;
	case TF_LINEAR:
		common->Printf( "linr " );
		break;
	case TF_NEAREST:
		common->Printf( "nrst " );
		break;
	default:
		common->Printf( "<BAD FILTER:%i>", filter );
		break;
	}

	switch ( internalFormat ) {
	case 1:
	case 2:
	case 3:
	case 4:
		common->Printf( "RGBA  " );
		break;
	case GL_RGBA4:
		common->Printf( "RGBA4 " );
		break;
	case GL_RGB5_A1:
	    common->Printf( "RGB5_A1  " );
		break;
	case 0:
		common->Printf( "      " );
		break;
	default:
		common->Printf( "<BAD FORMAT:%i>", internalFormat );
		break;
	}

	switch ( repeat ) {
	case TR_REPEAT:
		common->Printf( "rept " );
		break;
	case TR_CLAMP_TO_ZERO:
		common->Printf( "zero " );
		break;
	case TR_CLAMP_TO_ZERO_ALPHA:
		common->Printf( "azro " );
		break;
	case TR_CLAMP:
		common->Printf( "clmp " );
		break;
	default:
		common->Printf( "<BAD REPEAT:%i>", repeat );
		break;
	}

	common->Printf( "%4ik ", StorageSize() / 1024 );

	common->Printf( " %s\n", imgName.c_str() );
}
