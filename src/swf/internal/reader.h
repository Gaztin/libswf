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

#ifndef __SWF_READER_H__
#define __SWF_READER_H__

#include <stdint.h>

typedef enum byte_order
{
	BO_LittleEndian = 0,
	BO_BigEndian    = 1,
} byte_order;

typedef struct swf_reader
{
	uint8_t*   begin;
	uint8_t*   end;
	uint8_t*   cur;
	uint8_t    bit;
	byte_order byteOrder;
} swf_reader;

extern uint8_t swf_reader__read_bit   ( swf_reader* rd );
extern int     swf_reader__read_bits  ( swf_reader* rd, void* dst, size_t nbits );
extern size_t  swf_reader__read_bytes ( swf_reader* rd, void* dst, size_t size );
extern int     swf_reader__byte_align ( swf_reader* rd );

#endif
