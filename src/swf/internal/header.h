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

#ifndef __SWF_HEADER_H__
#define __SWF_HEADER_H__

#include <stdint.h>

#include "internal/fixed_point.h"
#include "internal/rect.h"

typedef struct
{
	uint8_t         signature[ 3 ];
	uint8_t         version;
	uint32_t        fileLength;
	swf_rect        frameSize;
	swf_fixed_point frameRate;
	uint16_t        frameCount;
} swf_header;

extern int swf_header__parse( struct swf_reader* rd, swf_header* outHeader );

#endif
