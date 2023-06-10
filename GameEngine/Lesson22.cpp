#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;

// Starts up SDL and creates window
bool init();

// Loads Media
bool loadMedia();

// Frees media and shuts down SDL
void close();

// Texture wrapper class
class LTexture {
public:
	// Initializes variables
	LTexture();

	// Deallocates memory
	~LTexture();

	// Loads image at specified path
	bool loadFromFile(std::string path);

#if defined(SDL_TTF_MAJOR_VERSION)
	// Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif 
	// Deallocates texture
	void free();

	// Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	// Set blending
	void setBlendMode(SDL_BlendMode blending);

	// Set alpha modulation
	void setAlpha(Uint8 alpha);

	// Renders texture at given point 
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	// Get width and height from texture
	int getWidth();
	int getHeight();

private:
	// The actual hardware texture
	SDL_Texture* mTexture;

	// Image dimensions
	int mWidth;
	int mHeight;
};

// The window we'll be rendering to
SDL_Window* gWindow = NULL;

// The surface contained by the window
SDL_Renderer* gRenderer = NULL;

// Scene texture
LTexture gPromptTextTexture;
LTexture gTimeTextTexture;

#if defined(SDL_TTF_MAJOR_VERSION)
// Globally used font
TTF_Font* gFont = NULL;
#endif


LTexture::LTexture() {
	// Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture() {

	// Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path) {

	// Get rid of preexisting texture
	free();

	// The final texture
	SDL_Texture* newTexture = NULL;

	// Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_Image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else {

		// Color Key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error : %s\n", path.c_str(), SDL_GetError());
		}
		else {

			// Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		// Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	// return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
	// Get rid of preexisting texture
	free();

	// Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface == NULL) {
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else {
		// Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else {
			// Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}
		// Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	// Return success
	return mTexture != NULL;
}
#endif

void LTexture::free() {
	// Free texture if exists
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	// Modulate texture
	SDL_SetTextureColorMod(mTexture, red, green, blue);

}

void LTexture::setBlendMode(SDL_BlendMode blending) {
	// Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
	// Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}
void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {

	// Set Rendering space and render to screen
	SDL_Rect renderQuad = { x, y , mWidth, mHeight };

	// Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	// Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

bool init() {
	// Initialization flag
	bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {

		// Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			// Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				// Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				// Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_Image could not initialize! SDL_Image Error: %s\n", IMG_GetError());
					success = false;
				}

#if defined(SDL_TTF_MAJOR_VERSION)
				// Initialize SDL_ttf
				if (TTF_Init() == -1) {
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
#endif
			}
		}
	}
	return success;
}

bool loadMedia() {

	//Loading success flag
	bool success = true;
	
	// Open the font
	gFont = TTF_OpenFont("22_timing/lazy.ttf", 28);
	if (gFont == NULL) {
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else {
		// Set text color as black
		SDL_Color textColor = { 0,0,0,255 };
		
		// Load prompt texture
		if (!gPromptTextTexture.loadFromRenderedText("Press Enter to reset Start Time.", textColor)) {
			printf("Unable to render prompt texture!\n");
			success = false;
		}
	}
	return success;
}

void close() {
	// Free loaded images
	gPromptTextTexture.free();

#if defined(SDL_TTF_MAJOR_VERSION)
	// Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;
#endif

	// Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gRenderer = NULL;
	gWindow = NULL;

	// Quit SDL subsystems
	SDL_Quit();
	IMG_Quit();
#if defined(SDL_TTF_MAJOR_VERSION)
	TTF_Quit();
#endif
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

			// Set text color as black
			SDL_Color textColor = { 0, 0, 0, 255 };
			
			// Current start time
			Uint32 startTime = 0;

			// In memory text stream
			std::stringstream timeText;

			// Current rendered texture
			LTexture* currentTexture = NULL;

			// Flip type
			SDL_RendererFlip flipType = SDL_FLIP_NONE;

			// While application is running
			while (!quit) {

				// Handle events on queue
				while (SDL_PollEvent(&e) != 0) {

					// User requests quit
					if (e.type == SDL_QUIT) {
						quit = true;
					}
					// Reset start time on return presskey
					else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
						startTime = SDL_GetTicks();
					}
				}

					// Set text to be rendered
					timeText.str( "" );
					timeText << "Milliseconds since start time: " << SDL_GetTicks() - startTime;

					// Render Text
					if (!gTimeTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor)) {
						printf("Unable to render time texture!\n");
					}

					// Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					// Render joystick 8 way angle
					gPromptTextTexture.render((SCREEN_WIDTH - gPromptTextTexture.getWidth())/2, 0);
					gTimeTextTexture.render((SCREEN_WIDTH - gPromptTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gPromptTextTexture.getHeight()) / 2);

					// Update screen
					SDL_RenderPresent(gRenderer);

			}
		}
	}

	// Free resources and close SDL
	close();

	return 0;
}