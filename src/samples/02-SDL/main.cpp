#include <stdio.h>
#include <stdlib.h>

#include <initializer_list>
#include <string_view>

#include <swf/swf.h>
#include <swf/tags.h>

#include <SDL_main.h>
#include <SDL.h>

static void draw_swf_tag( SDL_Renderer* renderer, swf_tag* tag )
{
	switch( swf_get_tag_type( tag ) )
	{
		case SWF_TT_SetBackgroundColor:
		{
			auto tagData = static_cast< swf_tag_SetBackgroundColor* >( swf_get_tag_data( tag ) );
			SDL_SetRenderDrawColor( renderer, tagData->r, tagData->g, tagData->b, 0xff );
			SDL_RenderClear( renderer );

			break;
		}
	}
}

extern "C" int SDL_main( int /*argc*/, char* /*argv*/[] )
{
	/* Initialize SDL */
	if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
	{
		printf( "SDL_Init Error: %s\n", SDL_GetError() );
		return -1;
	}

	/* Create window */
	SDL_Window* window = SDL_CreateWindow( "libswf Sample: 02-SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_RESIZABLE );
	if( window == nullptr )
	{
		printf( "SDL_CreateWindow Error: %s\n", SDL_GetError() );
		return -1;
	}

	/* Create renderer */
	SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_SOFTWARE );
	if( renderer == nullptr )
	{
		printf( "SDL_CreateRenderer Error: %s\n", SDL_GetError() );
		return -1;
	}

	/* Load SWF */
	swf_movie movie;
	if( swf_load( "image.swf", &movie ) < 0 )
	{
		printf( "swf_load Error\n" );
		return -1;
	}

	/* Window settings */
	SDL_SetWindowSize( window, movie.frameWidth / 20, movie.frameHeight / 20 );
	SDL_SetWindowResizable( window, SDL_FALSE );

	/* Draw SWF tags */
	for( size_t i = 0; i < movie.tagCount; ++i )
	{
		draw_swf_tag( renderer, swf_tag_at( &movie, i ) );
	}

	/* Main loop */
	bool      quit = false;
	SDL_Event event;
	while( !quit )
	{
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT )
				quit = true;
		}

		SDL_RenderPresent( renderer );
	}

	/* Clean up */
	swf_free( &movie );
	SDL_DestroyWindow( window );
	SDL_Quit();

	return 0;
}
