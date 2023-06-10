#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>

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
LTexture gArrowTexture;

#if defined(SDL_TTF_MAJOR_VERSION)
// Globally used font
TTF_Font* gFont = NULL;
#endif

// Game Controller 1 handler
SDL_Joystick* gGameController = NULL;

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
	if (SDL_Init(SDL_INIT_VIDEO || SDL_INIT_JOYSTICK) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {

		// Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}

		// Check for joysticks
		if (SDL_NumJoysticks() < 1) {
			printf("Warning: No joysticks connected!\n");
		}
		else {
			// Load joystick
			gGameController = SDL_JoystickOpen(0);
			if (gGameController == NULL) {
				printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
			}
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

				// Initilize PNG loading
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

	//Load arrow texture
	if (!gArrowTexture.loadFromFile("19_gamepads_and_joysticks/arrow.png"))
	{
		printf("Failed to load press texture!\n");
		success = false;
	}

	return success;

	return success;
}

void close() {
	// Free loaded images
	gArrowTexture.free();

#if defined(SDL_TTF_MAJOR_VERSION)
	// Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;
#endif

	// Close game controller
	SDL_JoystickClose(gGameController);
	gGameController = NULL;

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

			// Normalized direction
			int xDir = 0;
			int yDir = 0;

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
					else if (e.type == SDL_JOYAXISMOTION) {

						// Motion on controller 0
						if (e.jaxis.which == 0) {

							// X axis motion
							if (e.jaxis.axis == 0) {

								//Left of dead zone
								if (e.jaxis.value < -JOYSTICK_DEAD_ZONE) {
									xDir = -1;
								}
								// Right of dead zone
								else if (e.jaxis.value > JOYSTICK_DEAD_ZONE) {
									xDir = 1;
								}
								else {
									xDir = 0;
								}
							}
							else if (e.jaxis.axis == 1) {

								// Below  dead zone
								if (e.jaxis.value < -JOYSTICK_DEAD_ZONE) {
									yDir = -1;
								}
								// Above dead zone
								else if (e.jaxis.value > JOYSTICK_DEAD_ZONE) {
									yDir = 1;
								}
								else {
									yDir = 0;
								}
							}
						}

						// Clear screen
						SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
						SDL_RenderClear(gRenderer);

						// Calculate angle
						double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / M_PI);

						//Correct angle
						if (xDir == 0 && yDir == 0) {
							joystickAngle = 0;
						}
						// Render joystick 8 way angle
						gArrowTexture.render((SCREEN_WIDTH - gArrowTexture.getWidth()) / 2, SCREEN_HEIGHT - gArrowTexture.getHeight() / 2, NULL, joystickAngle);


						// Update screen
						SDL_RenderPresent(gRenderer);

					}
				}
			}
		}
	}

	// Free resources and close SDL
	close();

	return 0;
}