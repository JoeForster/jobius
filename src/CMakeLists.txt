cmake_minimum_required (VERSION 3.8)

add_executable(Jobius "WinMain.cpp")

set(SOURCES
	"ComponentArray.cpp"
	"EntityManager.cpp"
	"RenderSystem.cpp"
)

set(HEADERS
	"ComponentArray.h"
	"ComponentManager.h"
	"Coordinator.h"
	"EntityManager.h"
	"RenderSystem.h"
	"SpriteRenderer.h"
	"System.h"
	"SystemManager.h"
)

target_sources(Jobius PRIVATE ${SOURCES} ${HEADERS})

target_include_directories(Jobius PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(Jobius ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY})
