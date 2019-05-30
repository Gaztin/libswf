#include <stdio.h>
#include <stdlib.h>

#include <initializer_list>
#include <string_view>

#include <swf/swf.h>

int main( int /*argc*/, char* /*argv*/[] )
{
	for( const std::string_view filepath : { "image.swf" } ) // <-- Add file paths here
	{
		printf( "~~ %s ~~\n", filepath.data() );

		swf_movie movie;
		if( swf_load( filepath.data(), &movie ) < 0 )
		{
			printf( "  FAILED TO LOAD\n" );
			return -1;
		}

		printf( "  Frame size:  %d x %d\n", movie.frameWidth, movie.frameHeight );
		printf( "  Frame count: %d\n", movie.frameCount );
		printf( "  Frame rate:  %.2f\n", movie.frameRate );
		printf( "  Tag count:   %d\n", movie.tagCount );
		printf( "\n" );
	}

	getchar();
	return 0;
}
