#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Starts up SDL and creates window
bool init();

// Loads Media
bool loadMedia();

// Frees media and shuts down SDL
void close();

// Loads individual image as texture
SDL_Texture* loadTexture(std::string path);

// The window we'll be rendering to
SDL_Window* gWindow = NULL;

// The surface contained by the window
SDL_Renderer* gRenderer = NULL;

// Current displayed texture
SDL_Texture* gTexture = NULL;

bool init() {
	// Initialization flag
	bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			// Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				// Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				// Initilize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_Image could not initialize! SDL_Image Error: %s\n", IMG_GetError());
					success = false;
				}

			}
		}
	}
	return success;
}

bool loadMedia() {

	// Loading success flag
	bool success = true;

	// Nothing to load
	return success;
}

void close() {
	// Deallocate surface
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;

	// Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gRenderer = NULL;
	gWindow = NULL;

	// Quit SDL subsystems
	SDL_Quit();
	IMG_Quit();
}

SDL_Texture* loadTexture(std::string path) {

	// Load texture at specified path
	SDL_Texture* loadedTexture = IMG_LoadTexture(gRenderer, path.c_str());
	if (loadedTexture == NULL) {
		printf("Unable to load texture %s! SDL_Image Error: %s\n", path.c_str(), IMG_GetError());
	}

	return loadedTexture;

}

int main(int argc, char* args[]) {

	// Start up SDL and create window
	if (!init()) {
		printf("Failed to initialize!\n");
	}
	else {

		//Load media
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		}
		else {

			// Main loop flag
			bool quit = false;

			// Event handler
			SDL_Event e;

			// While application is running
			while (!quit) {

				// Handle events on queue
				while (SDL_PollEvent(&e) != 0) {

					// User requests quit
					if (e.type == SDL_QUIT) {
						quit = true;
					}
				}

				// Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				// Render yellow filled triangle
				SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
				SDL_RenderFillRect(gRenderer, &fillRect);

				// Render green outlined quad 
				SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
				SDL_RenderDrawRect(gRenderer, &outlineRect);

				// Draw blue horizontal line
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
				SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);

				// Draw Vertical line of yellow dots
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);
				for (int i = 0; i < SCREEN_HEIGHT; i+=4){
					SDL_RenderDrawPoint(gRenderer, SCREEN_WIDTH / 2, i);
				}

				// Update the surface
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	// Free resources and close SDL
	close();

	return 0;
}