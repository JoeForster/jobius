add_library(BlockoLife
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Species.h"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/BlockoLifeWorldBuilder.cpp"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/BlockoLifeWorldBuilder.h"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/EntityBuilder.cpp"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/EntityBuilder.h"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Components/SpeciesComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Components/HealthComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Components/BlockDropperComponent.h"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Systems/GameOfLifeSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Systems/GameOfLifeSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Systems/CreatureSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Systems/CreatureSystem.h"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Systems/BlockDropperSystem.cpp"
	"${CMAKE_SOURCE_DIR}/src/Games/BlockoLife/Systems/BlockDropperSystem.h"
)

target_include_directories(BlockoLife PUBLIC "${CMAKE_SOURCE_DIR}/src/Engine")
target_include_directories(BlockoLife PUBLIC "${CMAKE_SOURCE_DIR}/src/Games/BlockoLife")