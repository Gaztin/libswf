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

int swf_fixed_point__parse( swf_reader* rd, uint8_t nbits, swf_fixed_point* outFixedPoint )
{
	memset( outFixedPoint, 0, sizeof( swf_fixed_point ) );

	if( swf_reader__read_bits( rd, &outFixedPoint->fractional, nbits ) < 0 )
		return -1;

	if( swf_reader__read_bits( rd, &outFixedPoint->integer, nbits ) < 0 )
		return -1;

	return 0;
}
