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

#ifndef __SWF_LINE_STYLE_H__
#define __SWF_LINE_STYLE_H__

#include "internal/shape_version.h"

#include <stdint.h>

typedef struct swf_reader swf_reader;

typedef struct swf_line_style
{
	uint16_t width;
	uint32_t color;

} swf_line_style;

typedef struct swf_line_style_array
{
	uint16_t        styleCount;
	swf_line_style* styles;

} swf_line_style_array;

extern int  swf_line_style__parse        ( swf_reader* rd, swf_shape_version shapeVersion, swf_line_style* outLineStyle );
extern int  swf_line_style2__parse       ( swf_reader* rd, swf_line_style* outLineStyle );
extern int  swf_line_style_array__parse  ( swf_reader* rd, swf_shape_version shapeVersion, swf_line_style_array* outLineStyleArray );
extern void swf_line_style_array__destroy( swf_line_style_array* lineStyleArray );

#endif
