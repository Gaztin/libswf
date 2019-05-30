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

#include "reader.h"

#include <memory.h>
#include <stdlib.h>

static int flip_bits( void* buf, size_t nbits )
{
	if( nbits > ( sizeof( size_t ) * 8 ) )
		return -1;

	const size_t byte = ( ( size_t* )buf )[ 0 ];
	for( size_t i = 0; i < nbits / 2; ++i )
	{
		const size_t lo = ( byte >> i ) & 1;
		const size_t hi = ( byte >> ( nbits - i - 1 ) ) & 1;
		( ( size_t* )buf )[ 0 ] &= ~( 1  << i );
		( ( size_t* )buf )[ 0 ] |=  ( hi << i );
		( ( size_t* )buf )[ 0 ] &= ~( 1  << ( nbits - i - 1 ) );
		( ( size_t* )buf )[ 0 ] |=  ( lo << ( nbits - i - 1 ) );
	}

	return 0;
}

uint8_t swf_reader__read_bit( swf_reader* rd )
{
	uint8_t bit = ( *rd->cur ) >> rd->bit;
	rd->bit = ( rd->bit + 1 ) & 7;
	return bit;
}

int swf_reader__read_bits( swf_reader* rd, void* dst, size_t nbits )
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

size_t swf_reader__read_bytes( swf_reader* rd, void* dst, size_t size )
{
	for( size_t i = 0; i < size; ++i )
	{
		if( swf_reader__read_bits( rd, ( uint8_t* )dst + i, 8 ) < 0 )
			return i;
	}
	return size;
}

int swf_reader__byte_align( swf_reader* rd )
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
