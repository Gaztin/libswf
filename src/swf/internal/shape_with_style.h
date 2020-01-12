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

#ifndef __SWF_SHAPE_WITH_STYLE_H__
#define __SWF_SHAPE_WITH_STYLE_H__

#include "internal/fill_style.h"
#include "internal/line_style.h"

#include <stdint.h>

typedef struct swf_shape_record swf_shape_record;

typedef struct swf_shape_with_style
{
	swf_fill_style_array fillStyles;
	swf_line_style_array lineStyles;

	uint32_t          recordCount;
	swf_shape_record* records;

} swf_shape_with_style;

extern int swf_shape_with_style__parse( swf_reader* rd, swf_shape_version shapeVersion, swf_shape_with_style* outRect );

#endif
