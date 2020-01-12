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

#include "gradient.h"

#include "internal/color.h"
#include "internal/reader.h"

#include <stdlib.h>
#include <string.h>

int swf_gradient_record__parse( swf_reader* rd, swf_shape_version shapeVersion, swf_gradient_record* outGradientRecord )
{
	if( swf_reader__read_bytes( rd, &outGradientRecord->ratio, 1 ) < 0 )
		return -1;

	switch( shapeVersion )
	{
		case SWF_SHAPE1:
		case SWF_SHAPE2:
		{
			swf_rgb rgb;
			if( swf_rgb__parse( rd, &rgb ) < 0 )
				return -1;

			outGradientRecord->color = 0xFF000000 | ( uint32_t )rgb.b << 16 | ( uint32_t )rgb.g << 8 | ( uint32_t )rgb.r;

		} break;

		case SWF_SHAPE3:
		{
			swf_rgba rgba;
			if( swf_rgba__parse( rd, &rgba ) < 0 )
				return -1;

			outGradientRecord->color = ( uint32_t )rgba.a << 24 | ( uint32_t )rgba.b << 16 | ( uint32_t )rgba.g << 8 | ( uint32_t )rgba.r;

		} break;

		default:
		{
			return -1;
		}
	}

	return 0;
}

int swf_gradient__parse( swf_reader* rd, swf_shape_version shapeVersion, swf_gradient* outGradient )
{
	memset( outGradient, 0, sizeof( swf_gradient ) );

	if( swf_reader__read_bits( rd, &outGradient->spreadMode, 2 ) < 0 )
		return -1;

	if( swf_reader__read_bits( rd, &outGradient->interpolationMode, 2 ) < 0 )
		return -1;

	if( swf_reader__read_bits( rd, &outGradient->recordCount, 4 ) < 0 )
		return -1;

	outGradient->records = malloc( outGradient->recordCount * sizeof( swf_gradient_record ) );

	for( size_t i = 0; i < outGradient->recordCount; ++i )
	{
		if( swf_gradient_record__parse( rd, shapeVersion, &outGradient->records[ i ] ) < 0 )
			return -1;
	}

	return 0;
}

int swf_focal_gradient__parse( swf_reader* rd, swf_shape_version shapeVersion, swf_focal_gradient* outFocalGradient )
{
	if( swf_gradient__parse( rd, shapeVersion, &outFocalGradient->super ) < 0 )
		return -1;

	if( swf_fixed_point__parse( rd, 8, &outFocalGradient->focalPoint ) < 0 )
		return -1;

	return 0;
}
