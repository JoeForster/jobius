add_executable(JobiusUnitTests "UnitTestsMain.cpp")

# FIXME separate engine library so we don't have to rebuild same sources here and in Engine project
set(TESTS_SOURCES
	"${CMAKE_SOURCE_DIR}/src/UnitTests/UnitTestsMain.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/BehaviourTree.cpp"
	"${CMAKE_SOURCE_DIR}/src/UnitTests/BehaviourTreeTests.cpp"
)

set(TESTS_HEADERS
	"${CMAKE_SOURCE_DIR}/src/UnitTests/catch.hpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/BehaviourTree.h"
	"${CMAKE_SOURCE_DIR}/src/Engine/BehaviourTreeBuilder.h"
)

target_sources(${PROJECT_NAME}UnitTests PRIVATE ${TESTS_SOURCES} ${TESTS_HEADERS})