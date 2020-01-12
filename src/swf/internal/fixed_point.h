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

#ifndef __SWF_FIXED_POINT_H__
#define __SWF_FIXED_POINT_H__

#include <stdint.h>

typedef struct swf_reader swf_reader;

typedef struct swf_fixed_point
{
	int16_t  integer;
	uint16_t fractional;

} swf_fixed_point;

extern int swf_fixed_point__parse( swf_reader* rd, uint8_t nbits, swf_fixed_point* outFixedPoint );

#endif
