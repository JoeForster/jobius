cmake_minimum_required (VERSION 3.8)

add_executable(Jobius "WinMain.cpp")

set(SOURCES
	"WinMain.cpp"
	"ComponentArray.cpp"
	"EntityManager.cpp"
	"PhysicsSystem.cpp"
	"RenderSystem.cpp"
	"SDLRenderManager.cpp"
	"SpriteRenderer.cpp"
)

set(HEADERS
	"ComponentArray.h"
	"ComponentManager.h"
	"World.h"
	"EntityManager.h"
	"PhysicsSystem.h"
	"RenderSystem.h"
	"PhysicsSystem.h"
	"System.h"
	"SpriteRenderer.h"
	"SystemManager.h"
	"Transform.h"
)

target_sources(Jobius PRIVATE ${SOURCES} ${HEADERS})

target_include_directories(Jobius PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(Jobius PUBLIC ${SDL2_IMAGE_INCLUDE_DIR})
target_include_directories(Jobius PUBLIC ${SDL2_TTF_INCLUDE_DIR})
target_link_libraries(Jobius ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY})
