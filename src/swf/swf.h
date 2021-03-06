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

#ifndef __SWF_H__
#define __SWF_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct swf_tag swf_tag;

typedef struct
{
	uint32_t frameWidth;
	uint32_t frameHeight;
	uint16_t frameCount;
	float    frameRate;

	uint32_t tagCount;
	swf_tag* tags;

} swf_movie;

extern int      swf_load   ( const char* filepath, swf_movie* outMovie );
extern swf_tag* swf_tag_at ( swf_movie* movie, size_t index );
extern void     swf_free   ( swf_movie* movie );

#ifdef __cplusplus
}
#endif

#endif
