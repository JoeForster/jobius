#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "RenderTypes.h"
#include "Vector.h"

// Forward declarations of SDL types
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;
struct SDL_Rect;

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

struct Rect2D;

// TODO_RESOURCE_MANAGEMENT separate concept of resources from SDL-specific
class SDLRenderManager
{
public:
	static std::shared_ptr<SDLRenderManager> Create(SDL_Window* window, SDL_Renderer* renderer);

	// TODO want to make this protected/private but it's awkward - could do with derived type
	// but need a nice way to pass in needed type-specific resources like the window and renderer.
	// See: https://stackoverflow.com/questions/8147027/how-do-i-call-stdmake-shared-on-a-class-with-only-protected-or-private-const
	SDLRenderManager(SDL_Window* window, SDL_Renderer* renderer);
	~SDLRenderManager(void);

	// N.B. In a real project you might have a "locking" mechanism that
	// prevents people from calling these load functions after the designated load period
	// They should only be called during the init/loading flow (i.e. a loading screen)

	// Load an image file given a path.
	// Return an ID to be stored by the caller and used in Draw.
	// returns ResourceID_Invalid if it failed to load.
	ResourceID LoadTexture(const char* imagePath);
	// Load a font file given a path.
	// Return an ID to be stored by the caller and used in Draw.
	// returns ResourceID_Invalid if it failed to load.
	ResourceID LoadFont(const char* fontPath);
	// Create or update a text resource given a string and a resource.
	// Optionally take an existing text ID to update rather than create.
	bool PrepareText(const char* text, ResourceID& textResID, ResourceID fontID = ResourceID_Invalid);
	// HACK - we only have one pre-loaded font for debug for now
	ResourceID GetDefaultFont() const { return 0; }

	// Render clear, generally at frame start
	void RenderClear();

	// Draw image based on its ID returned by LoadImage
	void Draw(ResourceID imageID, Vector2i screenCoords = Vector2i::ZERO);
	void DrawText(ResourceID textID, Vector2i screenCoords = Vector2i::ZERO);
	// Helper to both prepare and draw text in one
	bool DrawText(const char* text, ResourceID& textResID, Vector2i screenCoords = Vector2i::ZERO, ResourceID fontID = ResourceID_Invalid);

	// Primitive drawing helpers
	void DrawLine(int x0, int y0, int x1, int y1);
	void DrawRect(const Rect2D& r); 

private:
	bool Init();

	SDL_Window* m_Window;
	SDL_Renderer* m_Renderer;

	// Cached graphical/font resources
	// TODO refactor - could be structs that manage their own creation/freeing

	// Keyed by image ID returned by LoadImage, passed to Draw
	std::vector<SDL_Texture*> m_ImageTextures;
	std::vector<SDL_Rect*> m_ImageSizeRects;
	// Keyed by font ID returned by LoadFont, passed to DrawText
	std::vector<TTF_Font*> m_Fonts;
	// Keyed by text ID returned by PrepareText, passed to DrawText
	std::vector<SDL_Texture*> m_TextTextures;
	std::vector<SDL_Rect*> m_TextSizeRects;
	std::vector<std::string> m_TextValues;

};
