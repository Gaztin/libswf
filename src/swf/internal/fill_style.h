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

#ifndef __SWF_FILL_STYLE_H__
#define __SWF_FILL_STYLE_H__

#include "internal/gradient.h"
#include "internal/matrix.h"
#include "internal/shape_version.h"

#include <stdint.h>

typedef enum swf_fill_style_type
{
	SWF_FillStyle_Solid                      = 0x00,
	SWF_FillStyle_LinearGradient             = 0x10,
	SWF_FillStyle_RadialGradient             = 0x12,
	SWF_FillStyle_FocalRadialGradient        = 0x13,
	SWF_FillStyle_RepeatingBitmap            = 0x40,
	SWF_FillStyle_ClippedBitmap              = 0x41,
	SWF_FillStyle_NonSmoothedRepeatingBitmap = 0x42,
	SWF_FillStyle_NonSmoothedClippedBitmap   = 0x43,

} swf_fill_style_type;

typedef struct swf_fill_style
{
	swf_fill_style_type type;
	uint32_t            color;

	union
	{
		struct
		{
			swf_matrix gradientMatrix;

			union 
			{
				swf_gradient       gradient;
				swf_focal_gradient focalGradient;

			} subType;

		} gradients;

		struct
		{
			uint16_t   bitmapId;
			swf_matrix bitmapMatrix;

		} bitmaps;

	} metaData;

} swf_fill_style;

typedef struct swf_fill_style_array
{
	uint16_t        styleCount;
	swf_fill_style* styles;

} swf_fill_style_array;

extern int  swf_fill_style__parse        ( swf_reader* rd, swf_shape_version shapeVersion, swf_fill_style* outFillStyle );
extern int  swf_fill_style_array__parse  ( swf_reader* rd, swf_shape_version shapeVersion, swf_fill_style_array* outFillStyleArray );
extern void swf_fill_style_array__destroy( swf_fill_style_array* fillStyleArray );

#endif
