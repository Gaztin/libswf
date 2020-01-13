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

#include "fixed_point.h"

#include <memory.h>

#include "internal/reader.h"

int swf_fixed_point_8__parse( swf_reader* rd, swf_fixed_point* outFixedPoint )
{
	memset( outFixedPoint, 0, sizeof( swf_fixed_point ) );

	swf_reader__byte_align( rd );

	if( swf_reader__read_bytes( rd, &outFixedPoint->fractional, 1 ) < 0 )
		return -1;

	if( swf_reader__read_bytes( rd, &outFixedPoint->integer, 1 ) < 0 )
		return -1;

	return 0;
}

int swf_fixed_point_16__parse( swf_reader* rd, swf_fixed_point* outFixedPoint )
{
	memset( outFixedPoint, 0, sizeof( swf_fixed_point ) );

	swf_reader__byte_align( rd );

	if( swf_reader__read_bytes( rd, &outFixedPoint->fractional, 2 ) < 0 )
		return -1;

	if( swf_reader__read_bytes( rd, &outFixedPoint->integer, 2 ) < 0 )
		return -1;

	return 0;
}

int swf_fixed_point_bit_value__parse( swf_reader* rd, uint8_t nbits, swf_fixed_point* outFixedPoint )
{
	memset( outFixedPoint, 0, sizeof( swf_fixed_point ) );

	uint32_t bitValue = 0;
	if( swf_reader__read_bits( rd, &bitValue, nbits ) < 0 )
		return -1;

	outFixedPoint->fractional = ( uint16_t )( ( bitValue & 0x0000FFFF ) );
	outFixedPoint->integer    = (  int16_t )( ( bitValue & 0xFFFF0000 ) >> 16 );

	return 0;
}
