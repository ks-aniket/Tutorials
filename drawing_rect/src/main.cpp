#include <SDL2/SDL.h>
#include <emscripten.h>
#include <iostream>

SDL_Window* pWindow{ nullptr };
SDL_Renderer* pRenderer{ nullptr };

// Start Position
int boxX{ 250 }, boxY{ 250 };

bool InitSDL()
{
	std::cout << "Initializing SDL...\n";
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
		return false;
	}
	
	// Create the window 
	pWindow = SDL_CreateWindow(
		"SDL2 and Emscripten Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN );
		
	if (!pWindow)
	{
		std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
		return false;
	}
	
	// Create the renderer
	pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!pRenderer) 
	{
        std::cerr << "Renderer Creation Failed: " << SDL_GetError() << std::endl;
        return false;
    }
	
	std::cout << "SDL Initialized Successfully!" << std::endl;
    return true;
}

void CleanUp()
{
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();	
}

void GameLoop()
{
	SDL_Event event;
    while (SDL_PollEvent(&event)) 
	{
        if (event.type == SDL_QUIT) 
		{
			// Exit the loop
            emscripten_cancel_main_loop(); 
        }
    }
	
	// Do Rendering
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderClear( pRenderer );

    SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
    SDL_Rect box {boxX, boxY, 50, 50};
    SDL_RenderFillRect(pRenderer, &box);

    SDL_RenderPresent(pRenderer);
}

int main()
{
	std::cout << "Starting game...\n";
    if (!InitSDL()) 
		return -1;
    
	std::cout << "Entering game loop...\n";
    emscripten_set_main_loop(GameLoop, 0, 1);

    CleanUp();
	
    return 0;
}
