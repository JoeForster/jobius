add_executable(Jobius "WinMain.cpp")

# TODO tidy this path hack prefix repetition
set(ENGINE_SOURCES
	"${CMAKE_SOURCE_DIR}/src/Engine/WinMain.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/ComponentArray.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/EntityManager.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/Vector.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/World.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/PhysicsSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/BoxCollisionSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlaneCollisionSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/SpriteRenderSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/SDLInputSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlayerControlSystem.cpp"
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
	"${CMAKE_SOURCE_DIR}/src/Engine/Coordinates.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/World.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/EntityManager.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PhysicsSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/BoxCollisionSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlaneCollisionSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/SpriteRenderSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PhysicsSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/SDLInputSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlayerControlSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/NPCSensorSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/NPCControlSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/System.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/SDLRenderManager.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/SystemManager.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/TransformComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/SpriteComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/RigidBodyComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/AABBComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlaneComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/KBInputComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PadInputComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/NPCBlackboardComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/PlayerComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/Behaviours.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/BehaviourTree.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/BehaviourTreeBuilder.h"
)

target_sources(${PROJECT_NAME} PRIVATE ${ENGINE_SOURCES} ${ENGINE_HEADERS})

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
