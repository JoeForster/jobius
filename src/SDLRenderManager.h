#pragma once

#include <vector>
#include <map>
#include <string>

//#include "Types.h"
typedef size_t ResourceID;

// Forward declarations of SDL types
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;
struct SDL_Rect;

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;


class SDLRenderManager
{
public:
	static const ResourceID ResourceID_Invalid = ~0u;

	static SDLRenderManager* Create(SDL_Window* aWindow, SDL_Renderer* aRenderer);
	~SDLRenderManager(void);

	// N.B. In a real project you might have a "locking" mechanism that
	// prevents people from calling these load functions after the designated load period
	// They should only be called during the init/loading flow (i.e. a loading screen)

	// Load an image file given a path.
	// Return an ID to be stored by the caller and used in Draw.
	// returns ResourceID_Invalid if it failed to load.
	ResourceID LoadImage(const char* szImagePath);
	// Load a font file given a path.
	// Return an ID to be stored by the caller and used in Draw.
	// returns ResourceID_Invalid if it failed to load.
	ResourceID LoadFont(const char* szFontPath);
	// Create or update a text resource given a string and a resource.
	// Optionally take an existing text ID to update rather than create.
	ResourceID PrepareText(const char* aText, ResourceID aFontID, ResourceID aUpdateTextID = ResourceID_Invalid);

	// Render clear, generally at frame start
	void RenderClear();

	// Draw image based on its ID returned by LoadImage
	void Draw(ResourceID anImageID, int aCellX = 0, int aCellY = 0);
	void DrawText(ResourceID aTextID, int aX, int aY);

private:
	SDLRenderManager(SDL_Window* aWindow, SDL_Renderer* aRenderer);
	bool Init();

	SDL_Window* myWindow;
	SDL_Renderer* myRenderer;
	SDL_Surface* world;

	// Cached graphical/font resources
	// TODO refactor - could be structs that manage their own creation/freeing

	// Keyed by image ID returned by LoadImage, passed to Draw
	std::vector<SDL_Texture*> myImageTextures;
	std::vector<SDL_Rect*> myImageSizeRects;
	// Keyed by font ID returned by LoadFont, passed to DrawText
	std::vector<TTF_Font*> myFonts;
	// Keyed by text ID returned by PrepareText, passed to DrawText
	std::vector<SDL_Texture*> myTextTextures;
	std::vector<SDL_Rect*> myTextSizeRects;
	std::vector<std::string> myTextValues;

};
