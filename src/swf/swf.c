/*
* Copyright (c) 2019 Sebastian Kylander https://gaztin.com/
*
* This software is provided 'as-is', without any express or implied warranty. In no event will
* the authors be held liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose, including commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not claim that you wrote the
*    original software. If you use this software in a product, an acknowledgment in the product
*    documentation would be appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be misrepresented as
*    being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "swf.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/tags/tag.h"
#include "internal/color.h"
#include "internal/fixed_point.h"
#include "internal/header.h"
#include "internal/rect.h"

#ifndef _WIN32
static errno_t fopen_s( FILE** stream, const char* filename, const char* mode )
{
	errno = 0;
	*stream = fopen( filename, mode );
	return errno;
}
#endif

int swf_load( const char* filepath, swf_movie* outMovie )
{
	if( outMovie == NULL )
		return -1;

	memset( outMovie, 0, sizeof( swf_movie ) );

	FILE* f = NULL;
	if( fopen_s( &f, filepath, "r" ) != 0 || f == NULL )
		return -1;

	/* Read entire file into buffer */
	fseek( f, 0, SEEK_END );
	long size = ftell( f );
	fseek( f, 0, SEEK_SET );
	swf_reader rd;
	rd.begin     = malloc( size );
	rd.end       = rd.begin + size;
	rd.cur       = rd.begin;
	rd.bit       = 0;
	rd.byteOrder = BO_BigEndian;
	fread( rd.begin, 1, size, f );
	fclose( f );

	swf_header header;
	if( swf_header__parse( &rd, &header ) < 0 )
	{
		free( rd.begin );
		return -1;
	}

	outMovie->frameWidth  = ( header.frameSize.xMax - header.frameSize.xMin );
	outMovie->frameHeight = ( header.frameSize.yMax - header.frameSize.yMin );
	outMovie->frameCount  = header.frameCount;
	outMovie->frameRate   = ( ( float )header.frameRate.integer + ( ( float )header.frameRate.fractional / USHRT_MAX ) );

	/* FileAttributes tag is required in SWF 8 and later */
	if( header.version >= 8 )
	{
		swf_tag fileAttributesTag;
		if( swf_tag__parse( &rd, &fileAttributesTag ) < 0 )
		{
			if( fileAttributesTag.data ) free( fileAttributesTag.data );
			free( rd.begin );
			return -1;
		}

		free( fileAttributesTag.data );

		if( fileAttributesTag.type != TT_FileAttributes )
		{
			free( rd.begin );
			return -1;
		}
	}

	/* Parse tags until end of file */
	while( rd.cur < rd.end )
	{
		swf_tag tag;
		if( swf_tag__parse( &rd, &tag ) < 0 )
		{
			if( tag.data ) free( tag.data );
			free( rd.begin );
			return -1;
		}

		++outMovie->tagCount;

		free( tag.data );
	}

	free( rd.begin );
	return 0;
}
