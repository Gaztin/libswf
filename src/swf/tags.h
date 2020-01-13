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

#ifndef __SWF_TAGS_H__
#define __SWF_TAGS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct swf_tag swf_tag;

typedef enum swf_tag_type
{
	SWF_TT_End                =  0,
	SWF_TT_ShowFrame          =  1,
	SWF_TT_DefineShape        =  2,
	SWF_TT_SetBackgroundColor =  9,
	SWF_TT_DefineShape2       =  22,
	SWF_TT_DefineShape3       =  32,
	SWF_TT_DefineShape4       =  83,
	SWF_TT_FileAttributes     = 69,
} swf_tag_type;

typedef struct swf_tag_SetBackgroundColor
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} swf_tag_SetBackgroundColor;

typedef struct swf_tag_DefineShape
{
	uint16_t id;
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;

} swf_tag_DefineShape;

extern swf_tag_type swf_get_tag_type( swf_tag* tag );
extern void*        swf_get_tag_data( swf_tag* tag );

#ifdef __cplusplus
}
#endif

#endif
