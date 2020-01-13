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

#include "shape_with_style.h"

#include "internal/reader.h"

#include <string.h>

int swf_shape_with_style__parse( swf_reader* rd, swf_shape_version shapeVersion, swf_shape_with_style* outRect )
{
	memset( outRect, 0, sizeof( swf_shape_with_style ) );

	if( swf_fill_style_array__parse( rd, shapeVersion, &outRect->fillStyles ) < 0 )
		return -1;

	if( swf_line_style_array__parse( rd, shapeVersion, &outRect->lineStyles ) < 0 )
		return -1;

	uint8_t numFillBits = 0;
	if( swf_reader__read_bits( rd, &numFillBits, 4 ) < 0 )
		return -1;

	uint8_t numLineBits = 0;
	if( swf_reader__read_bits( rd, &numLineBits, 4 ) < 0 )
		return -1;

	for( ;; )
	{
		uint8_t typeFlag = 0;
		if( swf_reader__read_bits( rd, &typeFlag, 1 ) < 0 )
			return -1;

		if( typeFlag == 0 )
		{
			uint8_t flagInformation = 0;
			if( swf_reader__read_bits( rd, &flagInformation, 5 ) < 0 )
				return -1;

			/* EndShapeRecord */
			if( flagInformation == 0 )
				break;

			/* StyleChangeRecord */

			/* StateMoveTo */
			if( flagInformation & 0x10 )
			{
				uint8_t moveBits = 0;
				if( swf_reader__read_bits( rd, &moveBits, 5 ) < 0 )
					return -1;

				int32_t moveDeltaX = 0;
				if( swf_reader__read_bits( rd, &moveDeltaX, moveBits ) < 0 )
					return -1;

				int32_t moveDeltaY = 0;
				if( swf_reader__read_bits( rd, &moveDeltaY, moveBits ) < 0 )
					return -1;
			}

			/* StateFillStyle0 */
			if( flagInformation & 0x08 )
			{
				uint32_t fillStyle0 = 0;
				if( swf_reader__read_bits( rd, &fillStyle0, numFillBits ) < 0 )
					return -1;
			}

			/* StateFillStyle1 */
			if( flagInformation & 0x04 )
			{
				uint32_t fillStyle1 = 0;
				if( swf_reader__read_bits( rd, &fillStyle1, numFillBits ) < 0 )
					return -1;
			}

			/* StateLineStyle */
			if( flagInformation & 0x02 )
			{
				uint32_t lineStyle = 0;
				if( swf_reader__read_bits( rd, &lineStyle, numLineBits ) < 0 )
					return -1;
			}

			/* StateNewStyles */
			if( flagInformation & 0x01 )
			{
				swf_fill_style_array fillStyles;
				if( swf_fill_style_array__parse( rd, shapeVersion, &fillStyles ) < 0 )
					return -1;

				swf_line_style_array lineStyles;
				if( swf_line_style_array__parse( rd, shapeVersion, &lineStyles ) < 0 )
					return -1;

				uint8_t numFillBits2 = 0;
				if( swf_reader__read_bits( rd, &numFillBits2, 4 ) < 0 )
					return -1;

				uint8_t numLineBits2 = 0;
				if( swf_reader__read_bits( rd, &numLineBits2, 4 ) < 0 )
					return -1;
			}
		}
		else
		{
			uint8_t straightFlag = 0;
			if( swf_reader__read_bits( rd, &straightFlag, 1 ) < 0 )
				return -1;

			uint8_t numBits = 0;
			if( swf_reader__read_bits( rd, &numBits, 4 ) < 0 )
				return -1;

			numBits -= 2;

			/* StraightEdgeRecord */
			if( straightFlag == 1 )
			{
				uint8_t generalLineFlag = 0;
				if( swf_reader__read_bits( rd, &generalLineFlag, 1 ) < 0 )
					return -1;

				if( generalLineFlag == 0 )
				{
					int8_t vertLineFlag = 0;
					if( swf_reader__read_bits( rd, &vertLineFlag, 1 ) < 0 )
						return -1;

					int64_t delta = 0;
					if( swf_reader__read_bits( rd, &delta, numBits + 2 ) < 0 )
						return -1;
				}
				else
				{
					int64_t deltaX = 0;
					if( swf_reader__read_bits( rd, &deltaX, numBits + 2 ) < 0 )
						return -1;

					int64_t deltaY = 0;
					if( swf_reader__read_bits( rd, &deltaY, numBits + 2 ) < 0 )
						return -1;
				}
			}

			/* CurvedEdgeRecord */
			else
			{
				int64_t controlDeltaX = 0;
				if( swf_reader__read_bits( rd, &controlDeltaX, numBits + 2 ) < 0 )
					return -1;

				int64_t controlDeltaY = 0;
				if( swf_reader__read_bits( rd, &controlDeltaY, numBits + 2 ) < 0 )
					return -1;

				int64_t anchorDeltaX = 0;
				if( swf_reader__read_bits( rd, &anchorDeltaX, numBits + 2 ) < 0 )
					return -1;

				int64_t anchorDeltaY = 0;
				if( swf_reader__read_bits( rd, &anchorDeltaY, numBits + 2 ) < 0 )
					return -1;
			}
		}
	}

	return 0;
}
