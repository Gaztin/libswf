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

#ifndef __SWF_COLOR_H__
#define __SWF_COLOR_H__

#include <stdint.h>

typedef struct swf_reader swf_reader;

typedef struct swf_rgb
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} swf_rgb;

typedef struct swf_rgba
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} swf_rgba;

typedef struct swf_argb
{
	uint8_t a;
	uint8_t r;
	uint8_t g;
	uint8_t b;
} swf_argb;

extern int swf_rgb__parse  ( swf_reader* rd, swf_rgb* outRgb );
extern int swf_rgba__parse ( swf_reader* rd, swf_rgba* outRgba );
extern int swf_argb__parse ( swf_reader* rd, swf_argb* outArgb );

#endif
