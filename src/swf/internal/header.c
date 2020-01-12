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

#include "header.h"

#include <stdlib.h>
#include <memory.h>

#include <zlib.h>

#include "internal/reader.h"

static int decompress_zlib( swf_reader* rd )
{
	z_stream strm;
	int32_t  ret;
	uint32_t have;
	uint8_t  out[ 128 * 1024 ]; /* 128kB */
	uint8_t* newbuf = NULL;
	uint8_t* tmp    = NULL;

	/* Initialize inflation */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	if( ( ret = inflateInit( &strm ) ) != Z_OK )
		return -1;

	do
	{
		size_t sizeIn = min( ( rd->end - rd->cur ), sizeof( out ) );
		strm.next_in  = rd->cur;
		strm.avail_in = ( uInt )sizeIn;
		if( strm.avail_in == 0 )
			break;

		do
		{
			strm.next_out  = out;
			strm.avail_out = sizeof( out );

			ret = inflate( &strm, Z_NO_FLUSH );
			if( ret < 0 )
			{
				inflateEnd( &strm );
				if( newbuf ) free( newbuf );
				if( tmp    ) free( tmp    );
				return -1;
			}
			if( ret == Z_OK )
			{
				have = ( sizeof( out ) - strm.avail_out );
				if( newbuf == NULL || have == strm.total_out )
				{
					newbuf = realloc( newbuf, strm.total_out );
				}
				else
				{
					tmp    = realloc( tmp, ( strm.total_out - have ) );
					memcpy( tmp, newbuf, ( strm.total_out - have ) );
					newbuf = realloc( newbuf, ( strm.total_out - have ) );
					memcpy( newbuf, tmp, ( strm.total_out - have ) );
				}
				memcpy( newbuf + ( strm.total_out - have ), out, have );
				rd->cur += ( sizeIn - strm.avail_in );
			}

		} while( strm.avail_out == 0 );

	} while( ret != Z_STREAM_END );
	
	inflateEnd( &strm );

	/* Swap buffer data */
	free( rd->begin );
	rd->begin = newbuf;
	rd->end   = rd->begin + strm.total_out;
	rd->cur   = rd->begin;
	rd->bit   = 0;

	return 0;
}

int swf_header__parse( swf_reader* rd, swf_header* outHeader )
{
	if( rd->cur + 2 >= rd->end )
		return -1;

	memset( outHeader, 0, sizeof( swf_header ) );

	/* Read signature and determine byte order */
	if( rd->cur[ 1 ] == 'W' && rd->cur[ 2 ] == 'S' )
	{
		rd->byteOrder = BO_LittleEndian;
	}
	else if( rd->cur[ 1 ] == 234 && rd->cur[ 2 ] == 202 )
	{
		rd->byteOrder = BO_BigEndian;
	}
	else
	{
		return -1;
	}

	if( swf_reader__read_bytes( rd, &outHeader->signature, sizeof( outHeader->signature ) ) < 0 )
		return -1;

	if( swf_reader__read_bytes( rd, &outHeader->version, sizeof( outHeader->version ) ) < 0 )
		return -1;

	if( swf_reader__read_bytes( rd, &outHeader->fileLength, sizeof( outHeader->fileLength ) ) < 0 )
		return -1;

	switch( outHeader->signature[ 0 ] )
	{
		/* No compression */
		case 'F':
		{
		} break;

		/* ZLib compression */
		case 'C':
		{
			if( decompress_zlib( rd ) < 0 )
				return -1;

		} break;

		/* LZMA compression */
		case 'Z':
		{
			return -1; /* Not supported */
		}

		default:
		{
			return -1;
		}
	}

	if( swf_rect__parse( rd, &outHeader->frameSize ) < 0 )
		return -1;

	if( swf_fixed_point__parse( rd, 8, &outHeader->frameRate ) < 0 )
		return -1;

	if( swf_reader__read_bytes( rd, &outHeader->frameCount, sizeof( outHeader->frameCount ) ) < 0 )
		return -1;

	return 0;
}
