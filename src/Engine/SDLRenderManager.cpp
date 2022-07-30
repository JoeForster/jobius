#include "SDLRenderManager.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "Vector.h"

#include <assert.h>

static SDL_Color ToSDLColour(const Colour4i& c)
{

};

std::shared_ptr<SDLRenderManager> SDLRenderManager::Create(SDL_Window* window, SDL_Renderer* renderer)
{
	auto drawer = std::make_shared<SDLRenderManager>(window, renderer);

	drawer->Init();

	return drawer;
}

SDLRenderManager::SDLRenderManager(SDL_Window* window, SDL_Renderer* renderer)
	: m_Window(window)
	, m_Renderer(renderer)
{
}

SDLRenderManager::~SDLRenderManager(void)
{
	// Free all of our managed memory.
	// If you can guarantee this class lasts the lifetime of the program
	// this may not be strictly necessary, but it's still good practice.
	for (SDL_Texture*& tex : m_ImageTextures)
		SDL_DestroyTexture(tex);
	for (SDL_Rect*& rect : m_ImageSizeRects)
		delete rect;
	for (TTF_Font*& font : m_Fonts)
		TTF_CloseFont(font);
	for (SDL_Texture*& tex : m_TextTextures)
		SDL_DestroyTexture(tex);
	for (SDL_Rect*& rect : m_TextSizeRects)
		delete rect;
}

bool SDLRenderManager::Init()
{
	if (!m_Window)
		return false;

	return true;
}


ResourceID SDLRenderManager::LoadTexture(const char* imageFile)
{
	SDL_Surface* surface = IMG_Load(imageFile);
	if (surface == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadTexture FAILED due to IMG_Load failure: %s\n", IMG_GetError());
		return -1;
	}

	SDL_Texture* optimizedSurface = SDL_CreateTextureFromSurface(m_Renderer, surface);
	if (optimizedSurface == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadTexture FAILED due to SDL_CreateTextureFromSurface failure: %s\n", SDL_GetError());
		SDL_FreeSurface(surface); // IMG_Load allocated this above, so free it
		return ResourceID_Invalid;
	}

	// Each image object vector should be the same size, added to only here.
	ResourceID nextID = m_ImageTextures.size();
	assert(nextID == m_ImageSizeRects.size());

	// Store everything created here to be used by Draw for this ID.
	m_ImageTextures.push_back(optimizedSurface);

	// NOTE this assumes that SDL_CreateTextureFromSurface will give us an
	// optimizedSurface of the same size as surface.
	SDL_Rect* sizeRect = new SDL_Rect;
	sizeRect->x = 0;
	sizeRect->y = 0;
	sizeRect->w = surface->w;
	sizeRect->h = surface->h;
	m_ImageSizeRects.push_back(sizeRect);

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

	ResourceID nextID = m_Fonts.size();
	m_Fonts.push_back(font);
	return nextID;
}

void SDLRenderManager::RenderClear()
{
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_Renderer);
}

void SDLRenderManager::Draw(ResourceID imageID, Vector2i screenCoords)
{
	// Basic checks (but assume vectors are same size, which is verified on load)
	assert(imageID != ResourceID_Invalid);
	if (imageID == ResourceID_Invalid)
		return; // Invalid ID passed
	assert(imageID < m_ImageTextures.size());
	if (imageID >= m_ImageTextures.size())
		return; // Invalid ID passed

	SDL_Texture* imageTex = m_ImageTextures[imageID];
	SDL_Rect* sizeRect = m_ImageSizeRects[imageID];
	SDL_Rect posRect;
	posRect.x = screenCoords.x;
	posRect.y = screenCoords.y;
	posRect.w = sizeRect->w;
	posRect.h = sizeRect->h;

	SDL_RenderCopy(m_Renderer, imageTex, sizeRect, &posRect);
}

bool SDLRenderManager::PrepareText(const char* text, ResourceID& textResID, ResourceID fontID)
{
	// Basic checks	
	if (fontID == ResourceID_Invalid)
		fontID = GetDefaultFont();
	assert(fontID != ResourceID_Invalid);
	assert(fontID < m_Fonts.size());
	if (fontID >= m_Fonts.size())
		return false; // Invalid ID passed

	TTF_Font* font = m_Fonts[fontID];

	// Colour is hard-coded for now.
	SDL_Color fg = { 255, 0, 0, 255 };

	// EXISTING TEXT - only re-create it if the text has changed
	// TODO_RESOURCE_MANAGEMENT consider a way of doing this without creating strings on
	// the heap - though any use of a hash would have to avoid issues
	// with collisions
	if (textResID != ResourceID_Invalid)
	{
		assert(textResID < m_TextTextures.size());
		assert(m_TextTextures.size() == m_TextSizeRects.size());
		assert(m_TextValues.size() == m_TextValues.size());

		if (m_TextValues[textResID] != text)
		{
			if (m_TextTextures[textResID] != nullptr)
			{
				SDL_DestroyTexture(m_TextTextures[textResID]);
				m_TextTextures[textResID] = nullptr;
			}
			// There might be a more efficient way to update this without
			// recreating everything, but this should be fine for our use.
			SDL_Surface* surface = TTF_RenderText_Solid(font, text, fg);
			m_TextTextures[textResID] = SDL_CreateTextureFromSurface(m_Renderer, surface);

			SDL_Rect* sizeRect = m_TextSizeRects[textResID];
			sizeRect->x = 0;
			sizeRect->y = 0;
			sizeRect->w = surface->w;
			sizeRect->h = surface->h;

			m_TextValues[textResID] = text;

			SDL_FreeSurface(surface);
		}
	}
	// NEW TEXT - create a new texture for it
	else
	{
		SDL_Surface* surface = TTF_RenderText_Solid(font, text, fg);
		SDL_Texture* optimizedSurface = SDL_CreateTextureFromSurface(m_Renderer, surface);

		textResID = m_TextTextures.size();
		assert(textResID == m_TextValues.size());
		assert(textResID == m_TextSizeRects.size());

		SDL_Rect* sizeRect = new SDL_Rect;
		sizeRect->x = 0;
		sizeRect->y = 0;
		sizeRect->w = surface->w;
		sizeRect->h = surface->h;

		// Store everything created here to be used by Draw for this ID.
		m_TextTextures.push_back(optimizedSurface);
		m_TextValues.push_back(text);
		m_TextSizeRects.push_back(sizeRect);

		SDL_FreeSurface(surface);
	}

	return true;
}

void SDLRenderManager::DrawText(ResourceID textID, Vector2i screenCoords)
{
	// Basic checks - TODO might be something worth removing once everything is verified,
	// since the caller should be responsible for passing valid IDs where required.
	assert(textID != ResourceID_Invalid);
	if (textID == ResourceID_Invalid)
		return; // Invalid ID passed
	assert(textID < m_TextTextures.size());
	if (textID >= m_TextTextures.size())
		return; // Invalid ID passed

	// Colour is hard-coded for now.
	SDL_Color fg = { 255,0,0,255 };
	SDL_Texture* optimizedSurface = m_TextTextures[textID];
	SDL_Rect* sizeRect = m_TextSizeRects[textID];

	SDL_Rect posRect;
	posRect.x = screenCoords.x;
	posRect.y = screenCoords.y;
	posRect.w = sizeRect->w;
	posRect.h = sizeRect->h;

	SDL_RenderCopy(m_Renderer, optimizedSurface, sizeRect, &posRect);
}

bool SDLRenderManager::DrawText(const char* text, ResourceID& textResID, Vector2i screenCoords, ResourceID fontID)
{
	bool result = PrepareText(text, textResID, fontID);
	if (result)
	{
		DrawText(textResID, screenCoords);
	}
	return result;
}

void SDLRenderManager::DrawLine(int x0, int y0, int x1, int y1, Colour4i colour)
{
	SDL_SetRenderDrawColor(m_Renderer, colour.r, colour.g, colour.b, colour.a);
	SDL_RenderDrawLine(m_Renderer, x0, y0, x1, y1);
}

void SDLRenderManager::DrawLine(Vector2i from, Vector2i to, Colour4i colour)
{
	DrawLine(from.x, from.y, to.x, to.y, colour);
}

void SDLRenderManager::DrawRect(const Rect2D& r, Colour4i colour)
{
	SDL_SetRenderDrawColor(m_Renderer, colour.r, colour.g, colour.b, colour.a);
	SDL_RenderDrawLine(m_Renderer, (int)r.min.x, (int)r.min.y, (int)r.max.x, (int)r.min.y);
	SDL_RenderDrawLine(m_Renderer, (int)r.max.x, (int)r.min.y, (int)r.max.x, (int)r.max.y);
	SDL_RenderDrawLine(m_Renderer, (int)r.max.x, (int)r.max.y, (int)r.min.x, (int)r.max.y);
	SDL_RenderDrawLine(m_Renderer, (int)r.min.x, (int)r.max.y, (int)r.min.x, (int)r.min.y);
}
