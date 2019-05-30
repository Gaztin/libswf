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

int swf_fixed_point_8_8__parse( swf_reader* rd, swf_fixed_point_8_8* outFixedPoint )
{
	memset( outFixedPoint, 0, sizeof( swf_fixed_point_8_8 ) );
	if( swf_reader__read_bytes( rd, &outFixedPoint->fractional, sizeof( uint8_t ) ) < 0 ) return -1;
	if( swf_reader__read_bytes( rd, &outFixedPoint->integer,    sizeof( int8_t  ) ) < 0 ) return -1;

	return 0;
}

int swf_fixed_point_16_16__parse( swf_reader* rd, swf_fixed_point_16_16* outFixedPoint )
{
	memset( outFixedPoint, 0, sizeof( swf_fixed_point_16_16 ) );
	if( swf_reader__read_bytes( rd, &outFixedPoint->fractional, sizeof( uint16_t ) ) < 0 ) return -1;
	if( swf_reader__read_bytes( rd, &outFixedPoint->integer,    sizeof( int16_t  ) ) < 0 ) return -1;

	return 0;
}
