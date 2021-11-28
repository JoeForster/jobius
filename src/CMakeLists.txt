add_executable(Jobius "WinMain.cpp")

set(SOURCES
	"WinMain.cpp"
	"ComponentArray.cpp"
	"EntityManager.cpp"
	"Vector.cpp"
	"World.cpp"
	"PhysicsSystem.cpp"
	"BoxCollisionSystem.cpp"
	"PlaneCollisionSystem.cpp"
	"SpriteRenderSystem.cpp"
	"SDLInputSystem.cpp"
	"PlayerControlSystem.cpp"
	"SDLRenderManager.cpp"
)

set(HEADERS
	"ComponentArray.h"
	"ComponentManager.h"
	"Vector.h"
	"World.h"
	"EntityManager.h"
	"PhysicsSystem.h"
	"BoxCollisionSystem.h"
	"PlaneCollisionSystem.h"
	"SpriteRenderSystem.h"
	"PhysicsSystem.h"
	"SDLInputSystem.h"
	"PlayerControlSystem.h"
	"System.h"
	"SDLRenderManager.h"
	"SystemManager.h"
	"TransformComponent.h"
	"SpriteComponent.h"
	"RigidBodyComponent.h"
	"AABBComponent.h"
	"PlaneComponent.h"
	"KBInputComponent.h"
	"PadInputComponent.h"
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_IMAGE_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_TTF_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2_image)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2_ttf)

set (ASSETS_SRC_PATH "${CMAKE_SOURCE_DIR}/assets")
#set (ASSETS_DST_PATH "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")
set (ASSETS_DST_PATH "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SRC_PATH} ${ASSETS_DST_PATH}
	DEPENDS ${ASSETS_DST_PATH}
	COMMENT "Copy assets ${ASSETS_SRC_PATH} => ${ASSETS_DST_PATH}"
)
