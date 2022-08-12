add_library(ChaosSpace
	"${CMAKE_SOURCE_DIR}/src/Games/ChaosSpace/ChaosSpaceWorldBuilder.cpp"
	"${CMAKE_SOURCE_DIR}/src/Games/ChaosSpace/ChaosSpaceWorldBuilder.h"
)

target_include_directories(ChaosSpace PUBLIC "${CMAKE_SOURCE_DIR}/src/Engine")
target_include_directories(ChaosSpace PUBLIC "${CMAKE_SOURCE_DIR}/src/Games/ChaosSpace")