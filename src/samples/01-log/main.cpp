#include <stdio.h>
#include <stdlib.h>

#include <swf/swf.h>

int main( int /*argc*/, char* /*argv*/[] )
{
	const char* filepath = "image.swf";

	printf( "~~ %s ~~\n", filepath );

	swf_movie_t* movie = swf_load( "image.swf" );
	if( movie == nullptr )
		return -1;

	printf( "Size: %d\n", movie->size );

	free( movie );

	return 0;
}
