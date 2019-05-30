#include <stdio.h>
#include <stdlib.h>

#include <initializer_list>
#include <string_view>

#include <swf/swf.h>

#include <SDL_main.h>
#include <SDL.h>

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

		SDL_RenderClear( renderer );
		SDL_RenderPresent( renderer );
	}

	SDL_DestroyWindow( window );
	SDL_Quit();

	return 0;
}
