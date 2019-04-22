#include <stdio.h>
#include <stdlib.h>

#include <swf/swf.h>

int main( int /*argc*/, char* /*argv*/[] )
{
	const char* filepath = "image.swf";

	printf( "~~ %s ~~\n", filepath );

	swf_movie movie;
	if( swf_load( filepath, &movie ) < 0 )
		return -1;

	printf( "Frame size:  %d x %d\n", movie.frameWidth, movie.frameHeight );
	printf( "Frame count: %d\n", movie.frameCount );
	printf( "Frame rate:  %.2f\n", movie.frameRate );
	printf( "Tag count:   %d\n", movie.tagCount );
	printf( "Background:  (%d, %d, %d)\n", movie.backgroundColor.r, movie.backgroundColor.g, movie.backgroundColor.b );

	getchar();
	return 0;
}
