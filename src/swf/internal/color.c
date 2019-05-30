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

#include "color.h"

#include <memory.h>

int swf_rgb__parse( swf_reader* rd, swf_rgb* outRgb )
{
	memset( outRgb, 0, sizeof( swf_rgb ) );
	if( swf_reader__read_bytes( rd, &outRgb->r, 1 ) < 0 ) return -1;
	if( swf_reader__read_bytes( rd, &outRgb->g, 1 ) < 0 ) return -1;
	if( swf_reader__read_bytes( rd, &outRgb->b, 1 ) < 0 ) return -1;
	return 0;
}

int swf_rgba__parse( swf_reader* rd, swf_rgba* outRgba )
{
	memset( outRgba, 0, sizeof( swf_rgba ) );
	if( swf_reader__read_bytes( rd, &outRgba->r, 1 ) < 0 ) return -1;
	if( swf_reader__read_bytes( rd, &outRgba->g, 1 ) < 0 ) return -1;
	if( swf_reader__read_bytes( rd, &outRgba->b, 1 ) < 0 ) return -1;
	return 0;
}

int swf_argb__parse( swf_reader* rd, swf_argb* outArgb )
{
	memset( outArgb, 0, sizeof( swf_argb ) );
	if( swf_reader__read_bytes( rd, &outArgb->r, 1 ) < 0 ) return -1;
	if( swf_reader__read_bytes( rd, &outArgb->g, 1 ) < 0 ) return -1;
	if( swf_reader__read_bytes( rd, &outArgb->b, 1 ) < 0 ) return -1;
	return 0;
}
