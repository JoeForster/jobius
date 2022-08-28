# TODO separate out the "shell" part from the "engine" part
set (SHELL_PROJECT_NAME ${PROJECT_NAME}Shell)
set (ENGINE_PROJECT_NAME ${PROJECT_NAME}Engine)



add_compile_definitions(JOBIUS_SHELL_GAME)

option(JOBIUS_SHELL_GAME_BLOCKO_LIFE "Blocko Life" ON)
option(JOBIUS_SHELL_GAME_CHAOS_SPACE "Chaos Space" OFF)

if(JOBIUS_SHELL_GAME_BLOCKO_LIFE)
  add_compile_definitions(JOBIUS_SHELL_GAME BlockoLife)
endif()
if(JOBIUS_SHELL_GAME_CHAOS_SPACE)
  add_compile_definitions(JOBIUS_SHELL_GAME ChaosSpace)
endif()


add_executable(${SHELL_PROJECT_NAME} "WinMain.cpp")

# TODO tidy this path hack prefix repetition
set(ENGINE_SOURCES
	"${CMAKE_SOURCE_DIR}/src/Engine/ComponentArray.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/EntityManager.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/Vector.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/PhysicsSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/BoxCollisionSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlaneCollisionSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/SpriteRenderSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/GridSpriteRenderSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/SDLInputSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlayerControlSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/Camera2DSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/NPCSensorSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/NPCControlSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/SDLRenderManager.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/BehaviourTree.cpp"
)

set(ENGINE_HEADERS
	"${CMAKE_SOURCE_DIR}/src/Engine/RenderTypes.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/ComponentArray.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/ComponentManager.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/Vector.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/World.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/WorldBuilder.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/EntityManager.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PhysicsSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/BoxCollisionSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlaneCollisionSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/SpriteRenderSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/GridSpriteRenderSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PhysicsSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/SDLInputSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlayerControlSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/Camera2DSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/NPCSensorSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/NPCControlSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/System.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/SDLRenderManager.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/SystemManager.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/TransformComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/GridTransformComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/SpriteComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/RigidBodyComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/AABBComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlaneComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/KBInputComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PadInputComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/NPCBlackboardComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlayerComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/GridWorldComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/Camera2DComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/Behaviours.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/BehaviourTree.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/BehaviourTreeBuilder.h"
)

add_library(${ENGINE_PROJECT_NAME} ${ENGINE_SOURCES} ${ENGINE_HEADERS})


# TODO only one part (Engine or SDL specific lib?) should have this dep, remove it from the shell to avoid doubling up here

target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIR})

target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE SDL2::SDL2)


target_include_directories(${SHELL_PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIR})

target_include_directories(${SHELL_PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(${SHELL_PROJECT_NAME} PUBLIC ${SDL2_IMAGE_INCLUDE_DIR})
target_include_directories(${SHELL_PROJECT_NAME} PUBLIC ${SDL2_TTF_INCLUDE_DIR})

target_link_libraries(${SHELL_PROJECT_NAME} PRIVATE SDL2::SDL2)
target_link_libraries(${SHELL_PROJECT_NAME} PRIVATE SDL2::SDL2main)
target_link_libraries(${SHELL_PROJECT_NAME} PRIVATE SDL2::SDL2_image)
target_link_libraries(${SHELL_PROJECT_NAME} PRIVATE SDL2::SDL2_ttf)


# TODO: Tidy this so we build a game project using the engine as a library?

target_link_libraries(${SHELL_PROJECT_NAME} PUBLIC BlockoLife)
target_link_libraries(${SHELL_PROJECT_NAME} PUBLIC ChaosSpace)
target_link_libraries(${SHELL_PROJECT_NAME} PUBLIC ${ENGINE_PROJECT_NAME})

target_include_directories(
	${SHELL_PROJECT_NAME} PUBLIC
	"${PROJECT_SOURCE_DIR}/src/Games/BlockoLife"
	"${PROJECT_SOURCE_DIR}/src/Games/ChaosSpace"
)

set (ASSETS_SRC_PATH "${CMAKE_SOURCE_DIR}/assets")
#set (ASSETS_DST_PATH "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")
set (ASSETS_DST_PATH "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(
	TARGET ${SHELL_PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SRC_PATH} ${ASSETS_DST_PATH}
	DEPENDS ${ASSETS_DST_PATH}
	COMMENT "Copy assets ${ASSETS_SRC_PATH} => ${ASSETS_DST_PATH}"
)
