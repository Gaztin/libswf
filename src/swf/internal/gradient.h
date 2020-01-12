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

#ifndef __GRADIENT_H__
#define __GRADIENT_H__

#include "internal/fixed_point.h"
#include "internal/shape_version.h"

#include <stdint.h>

typedef struct swf_reader swf_reader;

typedef enum swf_gradient_spread_mode
{
	SWF_GradientSpreadMode_Pad     = 0,
	SWF_GradientSpreadMode_Reflect = 1,
	SWF_GradientSpreadMode_Repeat  = 2,

} swf_gradient_spread_mode;

typedef enum swf_gradient_interpolation_mode
{
	SWF_GradientInterpolationMode_NormalRGB = 0,
	SWF_GradientInterpolationMode_LinearRGB = 1,

} swf_gradient_interpolation_mode;

typedef struct swf_gradient_record
{
	uint8_t  ratio;
	uint32_t color;
	
} swf_gradient_record;

typedef struct swf_gradient
{
	swf_gradient_spread_mode        spreadMode;
	swf_gradient_interpolation_mode interpolationMode;

	uint32_t             recordCount;
	swf_gradient_record* records;
	
} swf_gradient;

typedef struct swf_focal_gradient
{
	swf_gradient    super;
	swf_fixed_point focalPoint;
	
} swf_focal_gradient;

extern int swf_gradient_record__parse( swf_reader* rd, swf_shape_version shapeVersion, swf_gradient_record* outGradientRecord );
extern int swf_gradient__parse       ( swf_reader* rd, swf_shape_version shapeVersion, swf_gradient* outGradient );
extern int swf_focal_gradient__parse ( swf_reader* rd, swf_shape_version shapeVersion, swf_focal_gradient* outFocalGradient );

#endif
