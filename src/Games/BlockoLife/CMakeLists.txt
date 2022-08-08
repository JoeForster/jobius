add_library(BlockoLife
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/BlockoLifeWorldBuilder.cpp"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/BlockoLifeWorldBuilder.h"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Components/SpeciesComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Systems/GameOfLifeSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Systems/GameOfLifeSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Systems/CreatureSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Systems/CreatureSystem.h"
)

target_include_directories(BlockoLife PUBLIC "${CMAKE_SOURCE_DIR}/src/Engine")
target_include_directories(BlockoLife PUBLIC "${CMAKE_SOURCE_DIR}/src/Games/BlockoLife")