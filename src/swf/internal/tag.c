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

#include "tag.h"

#include <memory.h>
#include <stdlib.h>

#include "internal/color.h"
#include "internal/reader.h"
#include "internal/rect.h"
#include "internal/shape_with_style.h"
#include "tags.h"

int swf_tag__parse( swf_reader* rd, swf_tag* outTag )
{
	memset( outTag, 0, sizeof( swf_tag ) );

	uint16_t tagCodeAndLength = 0;
	if( swf_reader__read_bytes( rd, &tagCodeAndLength, 2 ) < 0 )
		return -1;

	outTag->type   = ( tagCodeAndLength & 0xFFC0 ) >> 6;
	outTag->length = ( tagCodeAndLength & 0x3F );

	if( outTag->length == 0x3F )
	{
		outTag->length = 0;
		if( swf_reader__read_bytes( rd, &outTag->length, 4 ) < 0 )
			return -1;
	}

	/* TODO: Parse meta-data */
	switch( outTag->type )
	{
		case SWF_TT_SetBackgroundColor:
		{
			outTag->data = malloc( sizeof( swf_tag_SetBackgroundColor ) );

			swf_rgb rgb;
			if( swf_rgb__parse( rd, &rgb ) < 0 )
				return -1;

			( ( swf_tag_SetBackgroundColor* )outTag->data )->r = rgb.r;
			( ( swf_tag_SetBackgroundColor* )outTag->data )->g = rgb.g;
			( ( swf_tag_SetBackgroundColor* )outTag->data )->b = rgb.b;

		} break;

		case SWF_TT_DefineShape:
		case SWF_TT_DefineShape2:
		case SWF_TT_DefineShape3:
		case SWF_TT_DefineShape4:
		{
			outTag->data = malloc( sizeof( swf_tag_DefineShape ) );

			uint16_t shapeId;
			if( swf_reader__read_bytes( rd, &shapeId, 2 ) < 0 )
				return -1;

			swf_rect shapeBounds;
			if( swf_rect__parse( rd, &shapeBounds ) < 0 )
				return -1;

			swf_shape_with_style shape;
			switch( outTag->type )
			{
				case SWF_TT_DefineShape:
				{
					if( swf_shape_with_style__parse( rd, SWF_SHAPE1, &shape ) < 0 )
						return -1;

				} break;

				case SWF_TT_DefineShape2:
				{
					if( swf_shape_with_style__parse( rd, SWF_SHAPE2, &shape ) < 0 )
						return -1;

				} break;

				case SWF_TT_DefineShape3:
				{
					if( swf_shape_with_style__parse( rd, SWF_SHAPE3, &shape ) < 0 )
						return -1;

				} break;

				case SWF_TT_DefineShape4:
				{
					swf_rect edgeBounds;
					if( swf_rect__parse( rd, &edgeBounds ) < 0 )
						return -1;

					uint8_t reserved = 0;
					if( swf_reader__read_bits( rd, &reserved, 5 ) < 0 )
						return -1;

					uint8_t usesFillWindingRule = 0;
					if( swf_reader__read_bits( rd, &usesFillWindingRule, 1 ) < 0 )
						return -1;

					uint8_t usesNonScalingStrokes = 0;
					if( swf_reader__read_bits( rd, &usesNonScalingStrokes, 1 ) < 0 )
						return -1;

					uint8_t usesScalingStrokes = 0;
					if( swf_reader__read_bits( rd, &usesScalingStrokes, 1 ) < 0 )
						return -1;

					if( swf_shape_with_style__parse( rd, SWF_SHAPE4, &shape ) < 0 )
						return -1;

				} break;

				default:
				{
					return -1;
				}
			}

			( ( swf_tag_DefineShape* )outTag->data )->id     = shapeId;
			( ( swf_tag_DefineShape* )outTag->data )->x      = shapeBounds.xMin;
			( ( swf_tag_DefineShape* )outTag->data )->y      = shapeBounds.yMin;
			( ( swf_tag_DefineShape* )outTag->data )->width  = shapeBounds.xMax - shapeBounds.xMin;
			( ( swf_tag_DefineShape* )outTag->data )->height = shapeBounds.yMax - shapeBounds.yMin;

		} break;

		default: /* Skip unsupported and empty tags */
		{
			rd->cur += outTag->length;

		} break;
	}

	return 0;
}
