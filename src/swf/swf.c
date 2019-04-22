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

#include <zlib.h>

#ifndef _WIN32
static errno_t fopen_s( FILE** stream, const char* filename, const char* mode )
{
	errno = 0;
	*stream = fopen( filename, mode );
	return errno;
}
#endif

#define memclr( Dst, Size ) memset( Dst, 0, Size )

static int flip_bits( void* buf, size_t nbits )
{
	if( nbits > ( sizeof( size_t ) * 8 ) )
		return -1;

	size_t byte = ( ( size_t* )buf )[ 0 ];
	for( size_t i = 0; i < nbits / 2; ++i )
	{
		size_t lo = ( byte >> i ) & 1;
		size_t hi = ( byte >> ( nbits - i - 1 ) ) & 1;
		( ( size_t* )buf )[ 0 ] &= ~( 1  << i );
		( ( size_t* )buf )[ 0 ] |=  ( hi << i );
		( ( size_t* )buf )[ 0 ] &= ~( 1  << ( nbits - i - 1 ) );
		( ( size_t* )buf )[ 0 ] |=  ( lo << ( nbits - i - 1 ) );
	}

	return 0;
}

typedef enum
{
	BO_LittleEndian = 0,
	BO_BigEndian    = 1,
} byte_order;

typedef struct
{
	uint8_t*   begin;
	uint8_t*   end;
	uint8_t*   cur;
	uint8_t    bit;
	byte_order byteOrder;
} reader;

static uint8_t read_bit( reader* rd )
{
	uint8_t bit = ( *rd->cur ) >> rd->bit;
	rd->bit = ( rd->bit + 1 ) & 7;
	return bit;
}

static int read_bits( reader* rd, void* dst, size_t nbits )
{
	if( rd->bit + nbits <= 8 )
	{
		if( rd->cur == rd->end )
			return -1;

		size_t byte;
		if( rd->byteOrder == BO_BigEndian )
		{
			byte = ( rd->cur[ 0 ] >> ( 8 - nbits - rd->bit ) ) & ( ( 1 << nbits ) - 1 );
		}
		else
		{
			byte = ( rd->cur[ 0 ] >> rd->bit ) & ( ( 1 << nbits ) - 1 );
		}

		memcpy( dst, &byte, 1 );
		rd->bit += ( uint8_t )nbits;
		if( rd->bit == 8 )
		{
			rd->bit = 0;
			++rd->cur;
		}
		return 0;
	}
	else
	{
		if( rd->cur >= rd->end )
			return -1;

		size_t bitsRead = ( 8 - rd->bit );

		size_t byte;
		if( rd->byteOrder == BO_BigEndian )
		{
			byte = ( rd->cur[ 0 ] & ( ( 1 << bitsRead ) - 1 ) );
			flip_bits( &byte, bitsRead );
		}
		else
		{
			byte = ( rd->cur[ 0 ] >> rd->bit ) & ( ( 1 << bitsRead ) - 1 );
		}


		while( bitsRead < nbits )
		{
			++rd->cur;
			if( rd->cur >= rd->end )
				return -1;

			size_t have = min( 8, ( nbits - bitsRead ) );
			size_t b    = rd->cur[ 0 ];

			if( rd->byteOrder == BO_BigEndian )
				flip_bits( &b, 8 );

			b          &= ( ( 1 << have ) - 1 );
			byte       |= ( b << bitsRead );
			bitsRead   += have;
		}

		if( rd->byteOrder == BO_BigEndian )
			flip_bits( &byte, bitsRead );

		rd->bit = ( rd->bit + bitsRead ) & 7;

		memcpy( dst, &byte, ( nbits + 7 ) / 8 );
		return 0;
	}
}

static size_t read_bytes( reader* rd, void* dst, size_t size )
{
	for( size_t i = 0; i < size; ++i )
	{
		if( read_bits( rd, ( uint8_t* )dst + i, 8 ) < 0 )
			return i;
	}
	return size;
}

static int reader_byte_align( reader* rd )
{
	if( rd->bit > 0 )
	{
		++rd->cur;
		rd->bit = 0;

		if( rd->cur > rd->end )
		{
			rd->cur = rd->end;
			return -1;
		}
	}

	return 0;
}

static int decompress_zlib( reader* rd )
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
					newbuf = ( uint8_t* )realloc( newbuf, strm.total_out );
				}
				else
				{
					tmp    = ( uint8_t* )realloc( tmp, ( strm.total_out - have ) );
					memcpy( tmp, newbuf, ( strm.total_out - have ) );
					newbuf = ( uint8_t* )realloc( newbuf, ( strm.total_out - have ) );
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

typedef struct
{
	int8_t  integer;
	uint8_t fractional;
} swf_fixed_point_8_8;

static int parse_fixed_point_8_8( reader* rd, swf_fixed_point_8_8* outFixedPoint )
{
	memclr( outFixedPoint, sizeof( swf_fixed_point_8_8 ) );
	if( read_bytes( rd, &outFixedPoint->fractional, sizeof( uint8_t ) ) < 0 ) return -1;
	if( read_bytes( rd, &outFixedPoint->integer,    sizeof( int8_t  ) ) < 0 ) return -1;

	return 0;
}

typedef struct
{
	int16_t  integer;
	uint16_t fractional;
} swf_fixed_point_16_16;

static int parse_fixed_point_16_16( reader* rd, swf_fixed_point_16_16* outFixedPoint )
{
	memclr( outFixedPoint, sizeof( swf_fixed_point_16_16 ) );
	if( read_bytes( rd, &outFixedPoint->fractional, sizeof( uint16_t ) ) < 0 ) return -1;
	if( read_bytes( rd, &outFixedPoint->integer,    sizeof( int16_t  ) ) < 0 ) return -1;

	return 0;
}

static int parse_rgb( reader* rd, swf_rgb* outRgb )
{
	memclr( outRgb, sizeof( swf_rgb ) );
	if( read_bytes( rd, &outRgb->r, 1 ) < 0 ) return -1;
	if( read_bytes( rd, &outRgb->g, 1 ) < 0 ) return -1;
	if( read_bytes( rd, &outRgb->b, 1 ) < 0 ) return -1;
	return 0;
}

static int parse_rgba( reader* rd, swf_rgba* outRgba )
{
	memclr( outRgba, sizeof( swf_rgba ) );
	if( read_bytes( rd, &outRgba->r, 1 ) < 0 ) return -1;
	if( read_bytes( rd, &outRgba->g, 1 ) < 0 ) return -1;
	if( read_bytes( rd, &outRgba->b, 1 ) < 0 ) return -1;
	return 0;
}

static int parse_argb( reader* rd, swf_argb* outArgb )
{
	memclr( outArgb, sizeof( swf_argb ) );
	if( read_bytes( rd, &outArgb->r, 1 ) < 0 ) return -1;
	if( read_bytes( rd, &outArgb->g, 1 ) < 0 ) return -1;
	if( read_bytes( rd, &outArgb->b, 1 ) < 0 ) return -1;
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

static int parse_rect( reader* rd, swf_rect* outRect )
{
	rd->byteOrder ^= 1;
	memclr( outRect, sizeof( swf_rect ) );
	if( read_bits( rd, &outRect->nbits, 5 ) < 0 ) return -1;
	if( read_bits( rd, &outRect->xMin, outRect->nbits ) < 0 ) return -1;
	if( read_bits( rd, &outRect->xMax, outRect->nbits ) < 0 ) return -1;
	if( read_bits( rd, &outRect->yMin, outRect->nbits ) < 0 ) return -1;
	if( read_bits( rd, &outRect->yMax, outRect->nbits ) < 0 ) return -1;
	reader_byte_align( rd );
	rd->byteOrder ^= 1;

	return 0;
}

typedef struct
{
	uint8_t             signature[ 3 ];
	uint8_t             version;
	uint32_t            fileLength;
	swf_rect            frameSize;
	swf_fixed_point_8_8 frameRate;
	uint16_t            frameCount;
} swf_header;

static int parse_header( reader* rd, swf_header* outHeader )
{
	if( rd->cur + 2 >= rd->end )
		return -1;

	memclr( outHeader, sizeof( swf_header ) );

	/* Read signature and determine byte order */
	if( rd->cur[ 1 ] == 'W' && rd->cur[ 2 ] == 'S' )
		rd->byteOrder = BO_LittleEndian;
	else if( rd->cur[ 1 ] == 234 && rd->cur[ 2 ] == 202 )
		rd->byteOrder = BO_BigEndian;
	else
		return -1;

	if( read_bytes( rd, &outHeader->signature,  sizeof( outHeader->signature  ) ) < 0 ) return -1;
	if( read_bytes( rd, &outHeader->version,    sizeof( outHeader->version    ) ) < 0 ) return -1;
	if( read_bytes( rd, &outHeader->fileLength, sizeof( outHeader->fileLength ) ) < 0 ) return -1;

	switch( outHeader->signature[ 0 ] )
	{
		/* No compression */
		case 'F':
			break;

		/* ZLib compression */
		case 'C':
			if( decompress_zlib( rd ) < 0 ) return -1;
			break;

		/* LZMA compression */
		case 'Z':
			return -1; /* Not supported */

		default:
			return -1;
	}

	if( parse_rect( rd, &outHeader->frameSize ) < 0 ) return -1;
	if( parse_fixed_point_8_8( rd, &outHeader->frameRate ) < 0 ) return -1;
	if( read_bytes( rd, &outHeader->frameCount, sizeof( outHeader->frameCount ) ) < 0 ) return -1;

	return 0;
}

typedef enum
{
	TT_End                = 0,
	TT_SetBackgroundColor = 9,
} tag_type;

typedef struct
{
	tag_type type;
	uint32_t length;
	uint8_t* data;
} swf_tag;

typedef struct
{
	swf_rgb rgb;
} tag_data_set_background_color;

static int parse_tag( reader* rd, swf_tag* outTag )
{
	memclr( outTag, sizeof( swf_tag ) );
	uint16_t tagCodeAndLength = 0;
	if( read_bytes( rd, &tagCodeAndLength, sizeof( tagCodeAndLength ) ) < 0 ) return -1;
	outTag->type   = ( tagCodeAndLength & 0xFFC0 ) >> 6;
	outTag->length = ( tagCodeAndLength & 0x3F );
	if( outTag->length == 0x3F )
	{
		outTag->length = 0;
		if( read_bytes( rd, &outTag->length, sizeof( outTag->length ) ) < 0 ) return -1;
	}

	outTag->data = ( uint8_t* )malloc( outTag->length );

	switch( outTag->type )
	{
		case TT_SetBackgroundColor:
		{
			parse_rgb( rd, &( ( tag_data_set_background_color* )outTag->data )->rgb );
			break;
		}

		default: /* Skip unsupported tags */
			rd->cur += outTag->length;
			break;
	}

	return 0;
}

int swf_load( const char* filepath, swf_movie* outMovie )
{
	if( outMovie == NULL )
		return -1;

	memclr( outMovie, sizeof( swf_movie ) );

	FILE* f = NULL;
	if( fopen_s( &f, filepath, "r" ) != 0 || f == NULL )
		return -1;

	/* Read entire file into buffer */
	fseek( f, 0, SEEK_END );
	long size = ftell( f );
	fseek( f, 0, SEEK_SET );
	reader rd;
	rd.begin     = ( uint8_t* )malloc( size );
	rd.end       = rd.begin + size;
	rd.cur       = rd.begin;
	rd.bit       = 0;
	rd.byteOrder = BO_BigEndian;
	fread( rd.begin, 1, size, f );
	fclose( f );

	swf_header header;
	if( parse_header( &rd, &header ) < 0 )
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
		if( parse_tag( &rd, &fileAttributesTag ) < 0 )
		{
			if( fileAttributesTag.data ) free( fileAttributesTag.data );
			free( rd.begin );
			return -1;
		}

		free( fileAttributesTag.data );
	}

	/* Parse tags until end of file */
	while( rd.cur < rd.end )
	{
		swf_tag tag;
		if( parse_tag( &rd, &tag ) < 0 )
		{
			if( tag.data ) free( tag.data );
			free( rd.begin );
			return -1;
		}

		++outMovie->tagCount;

		switch( tag.type )
		{
			case TT_SetBackgroundColor:
				outMovie->backgroundColor = ( ( tag_data_set_background_color* )tag.data )->rgb;
				break;
		}

		free( tag.data );
	}

	free( rd.begin );
	return 0;
}
