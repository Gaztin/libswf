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

#include "rect.h"

#include <memory.h>

#include "internal/reader.h"

int swf_rect__parse( swf_reader* rd, swf_rect* outRect )
{
	memset( outRect, 0, sizeof( swf_rect ) );

	rd->byteOrder ^= 1;

	uint8_t nbits = 0;
	if( swf_reader__read_bits( rd, &nbits, 5 ) < 0 )
		return -1;

	if( swf_reader__read_bits( rd, &outRect->xMin, nbits ) < 0 )
		return -1;

	if( swf_reader__read_bits( rd, &outRect->xMax, nbits ) < 0 )
		return -1;

	if( swf_reader__read_bits( rd, &outRect->yMin, nbits ) < 0 )
		return -1;

	if( swf_reader__read_bits( rd, &outRect->yMax, nbits ) < 0 )
		return -1;

	swf_reader__byte_align( rd );

	rd->byteOrder ^= 1;

	return 0;
}
