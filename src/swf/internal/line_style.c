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

#include "line_style.h"

#include "internal/color.h"
#include "internal/fill_style.h"
#include "internal/reader.h"

#include <stdlib.h>

int swf_line_style__parse( swf_reader* rd, swf_shape_version shapeVersion, swf_line_style* outLineStyle )
{
	if( swf_reader__read_bytes( rd, &outLineStyle->width, 2 ) < 0 )
		return -1;

	switch( shapeVersion )
	{
		case SWF_SHAPE1:
		case SWF_SHAPE2:
		{
			swf_rgb rgb;
			if( swf_rgb__parse( rd, &rgb ) < 0 )
				return -1;

			outLineStyle->color = 0xFF000000 | ( uint32_t )rgb.b << 16 | ( uint32_t )rgb.g << 8 | ( uint32_t )rgb.r;

		} break;

		case SWF_SHAPE3:
		{
			swf_rgba rgba;
			if( swf_rgba__parse( rd, &rgba ) < 0 )
				return -1;

			outLineStyle->color = ( uint32_t )rgba.a << 24 | ( uint32_t )rgba.b << 16 | ( uint32_t )rgba.g << 8 | ( uint32_t )rgba.r;

		} break;

		default:
		{
			return -1;
		}
	}

	return 0;
}

int swf_line_style2__parse( swf_reader* rd, swf_line_style* outLineStyle )
{
	if( swf_reader__read_bytes( rd, &outLineStyle->width, 2 ) < 0 )
		return -1;

	uint8_t startCapStyle[ 2 ];
	if( swf_reader__read_bytes( rd, startCapStyle, 2 ) < 0 )
		return -1;

	uint8_t joinStyle[ 2 ];
	if( swf_reader__read_bytes( rd, joinStyle, 2 ) < 0 )
		return -1;

	uint8_t hasFillFlag;
	if( swf_reader__read_bytes( rd, &hasFillFlag, 1 ) < 0 )
		return -1;

	uint8_t noHScaleFlag;
	if( swf_reader__read_bytes( rd, &noHScaleFlag, 1 ) < 0 )
		return -1;

	uint8_t noVScaleFlag;
	if( swf_reader__read_bytes( rd, &noVScaleFlag, 1 ) < 0 )
		return -1;

	uint8_t pixelHintingFlag;
	if( swf_reader__read_bytes( rd, &pixelHintingFlag, 1 ) < 0 )
		return -1;

	uint8_t reserved[ 5 ];
	if( swf_reader__read_bytes( rd, reserved, 5 ) < 0 )
		return -1;

	uint8_t noClose;
	if( swf_reader__read_bytes( rd, &noClose, 1 ) < 0 )
		return -1;

	uint8_t endCapStyle[ 2 ];
	if( swf_reader__read_bytes( rd, endCapStyle, 2 ) < 0 )
		return -1;

	if( joinStyle[ 0 ] == 2 )
	{
		uint16_t miterLimitFactor;
		if( swf_reader__read_bytes( rd, &miterLimitFactor, 2 ) < 0 )
			return -1;
	}

	if( hasFillFlag == 0 )
	{
		swf_rgba rgba;
		if( swf_rgba__parse( rd, &rgba ) < 0 )
			return -1;

		outLineStyle->color = ( uint32_t )rgba.a << 24 | ( uint32_t )rgba.b << 16 | ( uint32_t )rgba.g << 8 | ( uint32_t )rgba.r;
	}
	else if( hasFillFlag == 1 )
	{
		swf_fill_style fillType;
		if( swf_fill_style__parse( rd, &fillType ) < 0 )
			return -1;

		outLineStyle->color = fillType.color;
	}

	return 0;
}

int swf_line_style_array__parse( swf_reader* rd, swf_shape_version shapeVersion, swf_line_style_array* outLineStyleArray )
{
	uint8_t lineStyleCount;
	if( swf_reader__read_bytes( rd, &lineStyleCount, 1 ) < 0 )
		return -1;

	if( lineStyleCount == 0xFF )
	{
		uint16_t lineStyleCountExtended;
		if( swf_reader__read_bytes( rd, &lineStyleCountExtended, 2 ) < 0 )
			return -1;

		outLineStyleArray->styleCount = lineStyleCountExtended;
	}
	else
	{
		outLineStyleArray->styleCount = ( uint16_t )lineStyleCount;
	}

	outLineStyleArray->styles = malloc( outLineStyleArray->styleCount * sizeof( swf_line_style ) );

	for( size_t i = 0; i < outLineStyleArray->styleCount; ++i )
	{
		if( shapeVersion == SWF_SHAPE4 )
		{
			if( swf_line_style2__parse( rd, &outLineStyleArray->styles[ i ] ) < 0 )
				return -1;
		}
		else
		{
			if( swf_line_style__parse( rd, shapeVersion, &outLineStyleArray->styles[ i ] ) < 0 )
				return -1;
		}
	}

	return 0;
}

void swf_line_style_array__destroy( swf_line_style_array* lineStyleArray )
{
	if( lineStyleArray->styles != NULL )
	{
		free( lineStyleArray->styles );
	}
}
