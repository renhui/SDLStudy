// SDL.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

extern "C" {
#include "SDL.h"
}

#define POINTS_COUNT 4

static SDL_Point points[POINTS_COUNT] = {
	{320, 200},
	{300, 240},
	{340, 240},
	{320, 200}
};

static SDL_Rect bigrect = { 0,0,540, 380 };

int main(int argc, char* argv[])
{
	int flag = 1;

	SDL_Window *window;                    // Declare a pointer
	SDL_Renderer *renderer;

	SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

	// Create an application window with the following settings:
	window = SDL_CreateWindow(
		"SDL2 Draw Window",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		640,                               // width, in pixels
		480,                               // height, in pixels
		SDL_WINDOW_SHOWN // flags - see below
	);

	// Check that the window was successfully created
	if (window == NULL) {
		// In the case that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	/* We must call SDL_CreateRenderer in order for draw calls to affect this window. */
	renderer = SDL_CreateRenderer(window, -1, 0);

	/* Select the color for drawing. It is set to red here. */
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

	/* Clear the entire screen to our selected color. */
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	
	SDL_RenderDrawLines(renderer, points, POINTS_COUNT);
	
	SDL_Rect rect = { 200, 300, 100, 100 };
	SDL_RenderDrawRect(renderer, &rect);

	SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
	SDL_RenderFillRect(renderer, &rect);

	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	SDL_RenderFillRect(renderer, &bigrect);

	/* Up until now everything was drawn behind the scenes.
	   This will show the new, red contents of the window. */
	SDL_RenderPresent(renderer);

	// The window is open: could enter program loop here (see SDL_PollEvent())
	SDL_Delay(5000);  // Pause execution for 5000 milliseconds, for example

	//destory renderer
	if (renderer) {
		SDL_DestroyRenderer(renderer);
	}

	// Close and destroy the window
	SDL_DestroyWindow(window);

	// Clean up
	SDL_Quit();
	return 0;

}
