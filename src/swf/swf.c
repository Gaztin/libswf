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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	uint8_t* begin;
	uint8_t* end;
	uint8_t*       cur;
} buffer;

#ifndef _WIN32
static errno_t fopen_s( FILE** stream, const char* filename, const char* mode )
{
	errno = 0;
	*stream = fopen( filename, mode );
	return errno;
}
#endif

static int buffer_read( buffer* buf, void* dst, size_t size )
{
	if( ( buf->end - buf->cur ) < ( ptrdiff_t )size )
		return -1;

	memcpy( dst, buf->cur, size );
	buf->cur += size;
	return 0;
}

typedef struct
{
	uint8_t  nbits;
	uint32_t xMin;
	uint32_t xMax;
	uint32_t yMin;
	uint32_t yMax;
} swf_rect;

static int parse_rect( buffer* buf, swf_rect* outRect )
{
	if( buffer_read( buf, &outRect->nbits, sizeof( outRect->nbits ) ) < 0 ) return -1;
	size_t nbytes = ( ( size_t )outRect->nbits + 7 ) / 8;
	if( buffer_read( buf, &outRect->xMin, nbytes ) < 0 ) return -1;
	if( buffer_read( buf, &outRect->xMax, nbytes ) < 0 ) return -1;
	if( buffer_read( buf, &outRect->yMin, nbytes ) < 0 ) return -1;
	if( buffer_read( buf, &outRect->yMax, nbytes ) < 0 ) return -1;

	return 0;
}

typedef struct
{
	uint8_t  signature[ 3 ];
	uint8_t  version;
	uint32_t fileLength;
	swf_rect frameSize;
	uint16_t frameRate;
	uint16_t frameCount;
} swf_header;

static int parse_header( buffer* buf, swf_header* outHeader )
{
	if( buffer_read( buf, &outHeader->signature,  sizeof( outHeader->signature  ) ) < 0 ) return -1;
	if( buffer_read( buf, &outHeader->version,    sizeof( outHeader->version    ) ) < 0 ) return -1;
	if( buffer_read( buf, &outHeader->fileLength, sizeof( outHeader->fileLength ) ) < 0 ) return -1;

	switch( outHeader->signature[ 0 ] )
	{
		/* ZLib compression */
		case 'C':
		{
			break;
		}

		/* LZMA compression */
		case 'Z':
		{
			break;
		}

		default:
			break;
	}

	if( parse_rect( buf, &outHeader->frameSize ) < 0 ) return -1;
	if( buffer_read( buf, &outHeader->frameRate,  sizeof( outHeader->frameRate  ) ) < 0 ) return -1;
	if( buffer_read( buf, &outHeader->frameCount, sizeof( outHeader->frameCount ) ) < 0 ) return -1;

	return 0;
}

int swf_load( const char* filepath, swf_movie* outMovie )
{
	if( outMovie == NULL )
		return -1;

	FILE* f = NULL;
	if( fopen_s( &f, filepath, "r" ) !=0 || f == NULL )
		return -1;

	/* Read entire file into buffer */
	fseek( f, 0, SEEK_END );
	long size = ftell( f );
	fseek( f, 0, SEEK_SET );
	buffer buf;
	buf.begin = ( uint8_t* )malloc( size );
	buf.end   = buf.begin + size;
	buf.cur   = buf.begin;
	fread( buf.begin, 1, size, f );
	fclose( f );

	swf_header header;
	if( parse_header( &buf, &header ) < 0 )
	{
		free( buf.begin );
		return -1;
	}

	outMovie->size = ( buf.end - buf.begin );

	free( buf.begin );
	return 0;
}
