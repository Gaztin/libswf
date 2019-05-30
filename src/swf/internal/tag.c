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

#include "internal/reader.h"

int swf_tag__parse( swf_reader* rd, swf_tag* outTag )
{
	memset( outTag, 0, sizeof( swf_tag ) );
	uint16_t tagCodeAndLength = 0;
	if( swf_reader__read_bytes( rd, &tagCodeAndLength, sizeof( tagCodeAndLength ) ) < 0 ) return -1;
	outTag->type   = ( tagCodeAndLength & 0xFFC0 ) >> 6;
	outTag->length = ( tagCodeAndLength & 0x3F );
	if( outTag->length == 0x3F )
	{
		outTag->length = 0;
		if( swf_reader__read_bytes( rd, &outTag->length, sizeof( outTag->length ) ) < 0 ) return -1;
	}

	outTag->data = malloc( outTag->length );

	/* TODO: Parse meta-data */
	switch( outTag->type )
	{
		default: /* Skip unsupported tags */
			rd->cur += outTag->length;
			break;
	}

	return 0;
}
