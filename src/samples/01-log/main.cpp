#include <stdio.h>
#include <stdlib.h>

#include <swf/swf.h>

int main( int /*argc*/, char* /*argv*/[] )
{
	const char* filepath = "image.swf";

	printf( "~~ %s ~~\n", filepath );

	swf_movie movie;
	if( swf_load( "image.swf", &movie ) < 0 )
		return -1;

	printf( "Size: %zd\n", movie.size );

	getchar();
	return 0;
}
