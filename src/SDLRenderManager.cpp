#include "SDLRenderManager.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include <assert.h>

SDLRenderManager* SDLRenderManager::Create(SDL_Window* aWindow, SDL_Renderer* aRenderer)
{
	SDLRenderManager* drawer = new SDLRenderManager(aWindow, aRenderer);

	if (!drawer->Init())
	{
		delete drawer;
		drawer = NULL;
	}

	return drawer;
}

SDLRenderManager::SDLRenderManager(SDL_Window* aWindow, SDL_Renderer* aRenderer)
	: myWindow(aWindow)
	, myRenderer(aRenderer)
{
}

SDLRenderManager::~SDLRenderManager(void)
{
	// Free all of our managed memory.
	// If you can guarantee this class lasts the lifetime of the program
	// this may not be strictly necessary, but it's still good practice.
	for (SDL_Texture*& tex : myImageTextures)
		SDL_DestroyTexture(tex);
	for (SDL_Rect*& rect : myImageSizeRects)
		delete rect;
	for (TTF_Font*& font : myFonts)
		TTF_CloseFont(font);
	for (SDL_Texture*& tex : myTextTextures)
		SDL_DestroyTexture(tex);
	for (SDL_Rect*& rect : myTextSizeRects)
		delete rect;
}

bool SDLRenderManager::Init()
{
	if (!myWindow)
		return false;

	return true;
}


ResourceID SDLRenderManager::LoadImage(const char* imageFile)
{
	SDL_Surface* surface = IMG_Load(imageFile);
	if (surface == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadImage FAILED due to IMG_Load failure: %s\n", IMG_GetError());
		return -1;
	}

	SDL_Texture* optimizedSurface = SDL_CreateTextureFromSurface(myRenderer, surface);
	if (optimizedSurface == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadImage FAILED due to SDL_CreateTextureFromSurface failure: %s\n", SDL_GetError());
		SDL_FreeSurface(surface); // IMG_Load allocated this above, so free it
		return ResourceID_Invalid;
	}

	// Each image object vector should be the same size, added to only here.
	ResourceID nextID = myImageTextures.size();
	assert(nextID == myImageSizeRects.size());

	// Store everything created here to be used by Draw for this ID.
	myImageTextures.push_back(optimizedSurface);

	// NOTE this assumes that SDL_CreateTextureFromSurface will give us an
	// optimizedSurface of the same size as surface.
	SDL_Rect* sizeRect = new SDL_Rect;
	sizeRect->x = 0;
	sizeRect->y = 0;
	sizeRect->w = surface->w;
	sizeRect->h = surface->h;
	myImageSizeRects.push_back(sizeRect);

	// Free temporary resources allocated by this function.
	// Note that SDL_CreateTextureFromSurface copies the original surface,
	// so that original surface can be thrown away.
	SDL_FreeSurface(surface);

	return nextID;
}

ResourceID SDLRenderManager::LoadFont(const char* fontFile)
{
	// Simply load the font from the file, and if successful store and return the ID.
	// -1 indicates a valid font could not be loaded.
	TTF_Font* font = TTF_OpenFont(fontFile, 24);
	if (font == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadFont FAILED due to TTF_OpenFont failure: %s\n", TTF_GetError());
		return -1;
	}

	ResourceID nextID = myFonts.size();
	myFonts.push_back(font);
	return nextID;
}

void SDLRenderManager::RenderClear()
{
	SDL_SetRenderDrawColor(myRenderer, 0, 0, 0, 255);
	SDL_RenderClear(myRenderer);
}

void SDLRenderManager::Draw(ResourceID imageID, int aCellX, int aCellY)
{
	// Basic checks (but assume vectors are same size, which is verified on load)
	assert(imageID != ResourceID_Invalid);
	if (imageID == ResourceID_Invalid)
		return; // Invalid ID passed
	assert(imageID < myImageTextures.size());
	if (imageID >= myImageTextures.size())
		return; // Invalid ID passed

	SDL_Texture* imageTex = myImageTextures[imageID];
	SDL_Rect* sizeRect = myImageSizeRects[imageID];
	SDL_Rect posRect;
	posRect.x = aCellX;
	posRect.y = aCellY;
	posRect.w = sizeRect->w;
	posRect.h = sizeRect->h;

	SDL_RenderCopy(myRenderer, imageTex, sizeRect, &posRect);
}

ResourceID SDLRenderManager::PrepareText(const char* aText, ResourceID aFontID, ResourceID aUpdateTextID)
{
	// Basic checks
	assert(aFontID != ResourceID_Invalid);
	if (aFontID == ResourceID_Invalid)
		return ResourceID_Invalid; // Invalid ID passed
	assert(aFontID < myFonts.size());
	if (aFontID >= myFonts.size())
		return ResourceID_Invalid; // Invalid ID passed

	TTF_Font* font = myFonts[aFontID];

	// Colour is hard-coded for now.
	SDL_Color fg = { 255,0,0,255 };

	ResourceID textID = aUpdateTextID;

	// EXISTING TEXT - only re-create it if the text has changed
	// TODO consider a way of doing this without creating strings on
	// the heap - though any use of a hash would have to avoid issues
	// with collisions
	if (textID != ResourceID_Invalid)
	{
		assert(textID < myTextTextures.size());
		assert(myTextTextures.size() == myTextSizeRects.size());
		assert(myTextValues.size() == myTextValues.size());

		if (myTextValues[textID] != aText)
		{
			if (myTextTextures[textID] != nullptr)
			{
				SDL_DestroyTexture(myTextTextures[textID]);
				myTextTextures[textID] = nullptr;
			}
			// There might be a more efficient way to update this without
			// recreating everything, but this should be fine for our use.
			SDL_Surface* surface = TTF_RenderText_Solid(font, aText, fg);
			myTextTextures[textID] = SDL_CreateTextureFromSurface(myRenderer, surface);

			SDL_Rect* sizeRect = myTextSizeRects[textID];
			sizeRect->x = 0;
			sizeRect->y = 0;
			sizeRect->w = surface->w;
			sizeRect->h = surface->h;

			myTextValues[textID] = aText;

			SDL_FreeSurface(surface);
		}
	}
	// NEW TEXT - create a new texture for it
	else
	{
		SDL_Surface* surface = TTF_RenderText_Solid(font, aText, fg);
		SDL_Texture* optimizedSurface = SDL_CreateTextureFromSurface(myRenderer, surface);

		textID = myTextTextures.size();
		assert(textID == myTextValues.size());
		assert(textID == myTextSizeRects.size());

		SDL_Rect* sizeRect = new SDL_Rect;
		sizeRect->x = 0;
		sizeRect->y = 0;
		sizeRect->w = surface->w;
		sizeRect->h = surface->h;

		// Store everything created here to be used by Draw for this ID.
		myTextTextures.push_back(optimizedSurface);
		myTextValues.push_back(aText);
		myTextSizeRects.push_back(sizeRect);

		SDL_FreeSurface(surface);
	}

	return textID;
}

void SDLRenderManager::DrawText(ResourceID aTextID, int aX, int aY)
{
	// Basic checks - TODO might be something worth removing once everything is verified,
	// since the caller should be responsible for passing valid IDs where required.
	assert(aTextID != ResourceID_Invalid);
	if (aTextID == ResourceID_Invalid)
		return; // Invalid ID passed
	assert(aTextID < myTextTextures.size());
	if (aTextID >= myTextTextures.size())
		return; // Invalid ID passed

	// Colour is hard-coded for now.
	SDL_Color fg = { 255,0,0,255 };
	SDL_Texture* optimizedSurface = myTextTextures[aTextID];
	SDL_Rect* sizeRect = myTextSizeRects[aTextID];

	SDL_Rect posRect;
	posRect.x = aX;
	posRect.y = aY;
	posRect.w = sizeRect->w;
	posRect.h = sizeRect->h;

	SDL_RenderCopy(myRenderer, optimizedSurface, sizeRect, &posRect);
}
