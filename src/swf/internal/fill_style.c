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

#include "fill_style.h"

#include "internal/color.h"
#include "internal/reader.h"

#include <stdlib.h>
#include <string.h>

int swf_fill_style__parse( swf_reader* rd, swf_shape_version shapeVersion, swf_fill_style* outFillStyle )
{
	memset( outFillStyle, 0, sizeof( swf_fill_style ) );

	if( swf_reader__read_bytes( rd, &outFillStyle->type, 1 ) < 0 )
		return -1;

	if( outFillStyle->type == SWF_FillStyle_Solid )
	{
		switch( shapeVersion )
		{
			case SWF_SHAPE1:
			case SWF_SHAPE2:
			{
				swf_rgb rgb;
				if( swf_rgb__parse( rd, &rgb ) < 0 )
					return -1;

				outFillStyle->color = 0xFF000000 | ( uint32_t )rgb.b << 16 | ( uint32_t )rgb.g << 8 | ( uint32_t )rgb.r;

			} break;

			case SWF_SHAPE3:
			case SWF_SHAPE4:
			{
				swf_rgba rgba;
				if( swf_rgba__parse( rd, &rgba ) < 0 )
					return -1;

				outFillStyle->color = ( uint32_t )rgba.a << 24 | ( uint32_t )rgba.b << 16 | ( uint32_t )rgba.g << 8 | ( uint32_t )rgba.r;

			} break;

			default:
			{
				return -1;
			}
		}
	}
	else
	{
		swf_rgb color;
		if( swf_rgb__parse( rd, &color ) < 0 )
			return -1;

		outFillStyle->color = 0xFF000000 | ( uint32_t )color.b << 16 | ( uint32_t )color.g << 8 | ( uint32_t )color.r;
	}

	switch( outFillStyle->type )
	{
		case SWF_FillStyle_LinearGradient:
		case SWF_FillStyle_RadialGradient:
		{
			if( swf_matrix__parse( rd, &outFillStyle->metaData.gradients.gradientMatrix ) < 0 )
				return -1;

			if( swf_gradient__parse( rd, shapeVersion, &outFillStyle->metaData.gradients.subType.gradient ) < 0 )
				return -1;

		} break;

		case SWF_FillStyle_FocalRadialGradient:
		{
			if( swf_matrix__parse( rd, &outFillStyle->metaData.gradients.gradientMatrix ) < 0 )
				return -1;

			if( swf_focal_gradient__parse( rd, shapeVersion, &outFillStyle->metaData.gradients.subType.focalGradient ) < 0 )
				return -1;

		} break;

		case SWF_FillStyle_RepeatingBitmap:
		case SWF_FillStyle_ClippedBitmap:
		case SWF_FillStyle_NonSmoothedRepeatingBitmap:
		case SWF_FillStyle_NonSmoothedClippedBitmap:
		{
			if( swf_reader__read_bytes( rd, &outFillStyle->metaData.bitmaps.bitmapId, 2 ) < 0 )
				return -1;

			if( swf_matrix__parse( rd, &outFillStyle->metaData.bitmaps.bitmapMatrix ) < 0 )
				return -1;

		} break;
	}

	return 0;
}

int swf_fill_style_array__parse( swf_reader* rd, swf_shape_version shapeVersion, swf_fill_style_array* outFillStyleArray )
{
	memset( outFillStyleArray, 0, sizeof( swf_fill_style_array ) );

	if( swf_reader__read_bytes( rd, &outFillStyleArray->styleCount, 1 ) < 0 )
		return -1;

	if( outFillStyleArray->styleCount == 0xFF )
	{
		if( swf_reader__read_bytes( rd, &outFillStyleArray->styleCount, 2 ) < 0 )
			return -1;
	}

	outFillStyleArray->styles = malloc( outFillStyleArray->styleCount * sizeof( swf_fill_style ) );

	for( size_t i = 0; i < outFillStyleArray->styleCount; ++i )
	{
		if( swf_fill_style__parse( rd, shapeVersion, &outFillStyleArray->styles[ i ] ) < 0 )
			return -1;
	}

	return 0;
}

void swf_fill_style_array__destroy( swf_fill_style_array* fillStyleArray )
{
	if( fillStyleArray->styles != NULL )
	{
		free( fillStyleArray->styles );
	}
}
