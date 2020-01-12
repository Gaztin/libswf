/*
* Copyright (c) 2020 Sebastian Kylander https://gaztin.com/
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

#include "matrix.h"

#include "internal/fixed_point.h"
#include "internal/reader.h"

#include <stdint.h>
#include <string.h>

int swf_matrix__parse( swf_reader* rd, swf_matrix* outMatrix )
{
	memset( outMatrix, 0, sizeof( swf_matrix ) );

	uint8_t hasScale = 0;
	if( swf_reader__read_bits( rd, &hasScale, 1 ) < 0 )
		return -1;

	if( hasScale == 1 )
	{
		uint8_t nScaleBits = 0;
		if( swf_reader__read_bits( rd, &nScaleBits, 5 ) < 0 )
			return -1;

		swf_fixed_point scaleX;
		if( swf_fixed_point__parse( rd, nScaleBits, &scaleX ) < 0 )
			return -1;

		swf_fixed_point scaleY;
		if( swf_fixed_point__parse( rd, nScaleBits, &scaleY ) < 0 )
			return -1;
	}

	uint8_t hasRotate = 0;
	if( swf_reader__read_bits( rd, &hasRotate, 1 ) < 0 )
		return -1;

	if( hasRotate == 1 )
	{
		uint8_t nRotateBits = 0;
		if( swf_reader__read_bits( rd, &nRotateBits, 5 ) < 0 )
			return -1;

		swf_fixed_point rotateSkew0;
		if( swf_fixed_point__parse( rd, nRotateBits, &rotateSkew0 ) < 0 )
			return -1;

		swf_fixed_point rotateSkew1;
		if( swf_fixed_point__parse( rd, nRotateBits, &rotateSkew1 ) < 0 )
			return -1;
	}

	uint8_t translateBits = 0;
	if( swf_reader__read_bits( rd, &translateBits, 5 ) < 0 )
		return -1;

	int32_t translateX = 0;
	if( swf_reader__read_bits( rd, &translateX, translateBits ) < 0 )
		return -1;

	int32_t translateY = 0;
	if( swf_reader__read_bits( rd, &translateY, translateBits ) < 0 )
		return -1;

	swf_reader__byte_align( rd );

	/* TODO: Calculate matrix elements */

	return 0;
}

void swf_matrix__identity( swf_matrix* matrix )
{
	memset( matrix->elements, 0, sizeof( float ) * 6 );

	matrix->elements[ 0 ] = 1.0f;
	matrix->elements[ 3 ] = 1.0f;
}
