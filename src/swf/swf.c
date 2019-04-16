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

#include "swf.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
static errno_t fopen_s( FILE** stream, const char* filename, const char* mode )
{
	errno = 0;
	*stream = fopen( filename, mode );
	return errno;
}
#endif

swf_movie_t* swf_load( const char* filepath )
{
	FILE* f = NULL;
	if( fopen_s( &f, filepath, "r" ) !=0 || f == NULL )
		return NULL;

	swf_movie_t* movie = ( swf_movie_t* )malloc( sizeof( swf_movie_t ) );
	fseek(f, 0, SEEK_END);
	movie->size = ftell(f);
	fseek(f, 0, SEEK_SET);
	fclose( f );
	return movie;
}
