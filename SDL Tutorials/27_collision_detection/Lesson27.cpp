#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


// Starts up SDL and creates window
bool init();

// Loads Media
bool loadMedia();

// Frees media and shuts down SDL
void close();

// Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

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

// The application time based timer
class LTimer {
public:
	// Initializes variables
	LTimer();

	// The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	// Gets the timer's time
	Uint32 getTicks();

	// Checks status of the timer
	bool isStarted();
	bool isPaused();

private:
	// The clock time when the timer started
	Uint32 mStartTicks;

	// The ticks stored when the timer was paused
	Uint32 mPausedTicks;

	// The timer status
	bool mPaused;
	bool mStarted;
};

class Dot {
public:
	// The dimensions of the dot 
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	// Maximum axis velocity of the dot
	static const int DOT_VEL = 10;

	// Initializes the variables
	Dot();

	// Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event& e);

	// Moves the dot
	void move(SDL_Rect & wall);

	// Show the dot on the screen
	void render();

private:
	// The x and Y offsets of the dot
	int mPosX, mPosY;

	// The velocity of the dot
	int mVelX, mVelY;

	// Dot's collision box
	SDL_Rect mCollider;
};
// The window we'll be rendering to
SDL_Window* gWindow = NULL;

// The surface contained by the window
SDL_Renderer* gRenderer = NULL;

// Scene textures
LTexture gDotTexture;
LTexture gTimeTextTexture;

#if defined(SDL_TTF_MAJOR_VERSION)
// Globally used font
TTF_Font* gFont = NULL;
#endif

Dot::Dot() {
	// Initialize the offsets
	mPosX = 0;
	mPosY = 0;

	// Set collision box dimension
	mCollider.w = DOT_WIDTH;
	mCollider.h = DOT_HEIGHT;
	// initialize the velocity
	mVelX = 0;
	mVelY = 0;
}

void Dot::handleEvent(SDL_Event& e) {
	// If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		// Adjust the velocity
		switch (e.key.keysym.sym) {
		case SDLK_UP: mVelY -= DOT_VEL; break;
		case SDLK_DOWN: mVelY += DOT_VEL; break;
		case SDLK_LEFT: mVelX -= DOT_VEL; break;
		case SDLK_RIGHT: mVelX += DOT_VEL; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
		// Adjust the velocity
		switch (e.key.keysym.sym) {
		case SDLK_UP: mVelY += DOT_VEL; break;
		case SDLK_DOWN: mVelY -= DOT_VEL; break;
		case SDLK_LEFT: mVelX += DOT_VEL; break;
		case SDLK_RIGHT: mVelX -= DOT_VEL; break;
		}
	}
}

void Dot::move(SDL_Rect &wall) {
	// Move teh dot left or right
	mPosX += mVelX;
	mCollider.x = mPosX;

	// If the dot went too far to the left or right or colided
	if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH) || checkCollision(mCollider, wall)) {
		// Move back
		mPosX -= mVelX;
		mCollider.x = mPosX;
	}

	// Move the dot up or down
	mPosY += mVelY;
	mCollider.y = mPosY;

	// If the dot went too far up or down
	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT) || checkCollision(mCollider, wall)) {
		// Move back
		mPosY -= mVelY;
		mCollider.y = mPosY;
	}
}

void Dot::render() {
	// Show the dot
	gDotTexture.render(mPosX, mPosY);
}
LTimer::LTimer() {
	// Initialize the variables
	mStartTicks = 0;
	mPausedTicks = 0;

	mPaused = false;
	mStarted = false;
}

void LTimer::start() {
	// Start the timer
	mStarted = true;

	// Unpause the timer
	mPaused = false;

	// Get the current clock time
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop() {
	// Stop the timer
	mStarted = false;

	// Unpause the timer
	mPaused = false;

	// Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause() {
	// If the timer is running and isn't already paused
	if (mStarted && !mPaused) {
		// Pause the timer
		mPaused = true;

		// Calculate the paused ticks
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void LTimer::unpause() {
	// If the timer is running and is paused
	if (mStarted && mPaused) {
		// Unpause the timer
		mPaused = false;

		// Calculate the unpaused ticks
		mStartTicks = SDL_GetTicks() - mPausedTicks;
		mPausedTicks = 0;
	}
}

Uint32 LTimer::getTicks() {
	// If the timer is running and paused
	if (mStarted && mPaused) {
		return mPausedTicks;
	}
	// If the timer is running and not paused
	else if (mStarted) {
		return SDL_GetTicks() - mStartTicks;
	}
	// If the timer is not running
	else {
		return 0;
	}
}

bool LTimer::isStarted() {
	// Timer is running and paused or unpaused
	return mStarted;
}

bool LTimer::isPaused() {
	// Timer is running and paused
	return mPaused && mStarted;
}

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

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if (!gDotTexture.loadFromFile("27_collision_detection/dot.bmp"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	return success;
}

void close() {
	// Free loaded images
	gTimeTextTexture.free();

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

bool checkCollision(SDL_Rect a, SDL_Rect b) {
	// The side of the rectangles
	int leftA, leftB, rightA, rightB, topA, topB, bottomA, bottomB;
	
	// Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	// Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	// If any of the sides from A are outside of B
	if (bottomA <= topB) {
		return false;
	}
	if (topA >= bottomB) {
		return false;
	}
	if (rightA <= leftB) {
		return false;
	}
	if (leftA >= rightB) {
		return false;
	}
	// If none of the sides from A are outside B
	return true;
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

			// Set the wall
			SDL_Rect wall;
			wall.x = 300;
			wall.y = 40;
			wall.w = 40;
			wall.h = 400;

			// The dot that will be moving around on the screen
			Dot dot;

			// In memory text stream
			std::stringstream timeText;

			// While application is running
			while (!quit) {

				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					dot.handleEvent(e);
				}
				// Move the dot
				dot.move(wall);

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				// Render objects
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderDrawRect(gRenderer, &wall);
				dot.render();

				//Update screen
				SDL_RenderPresent(gRenderer);

			}
		}
	}

	// Free resources and close SDL
	close();

	return 0;
}