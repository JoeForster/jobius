cmake_minimum_required (VERSION 3.8)

project(Jobius)

#set(SOURCES "src/ComponentArray.cpp" "src/EntityManager.cpp" "src/RenderSystem.cpp")  
#set(HEADERS "src/ComponentArray.h" "src/EntityManager.h" "src/RenderSystem.h" "src/SpriteRenderer.h" "System.h")  

#add_subdirectory

#add_executable(Jobius "src/WinMain.cpp")


# TODO: Add tests and install targets if needed.

# include_directories("C:\\Dev\\jobius\\Jobius\\lib\\SDL2-2.0.14\\include")

find_path(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
find_library(SDL2_LIBRARY NAME SDL2)
find_library(SDL2MAIN_LIBRARY NAME SDL2main)
find_path(SDL2_IMAGE_INCLUDE_DIR NAME SDL2_image.h HINTS SDL2_image)
find_library(SDL2_IMAGE_LIBRARY NAME SDL2_image)
find_path(SDL2_TTF_INCLUDE_DIR NAME SDL_ttf.h HINTS SDL2_ttf)
find_library(SDL2_TTF_LIBRARY NAME SDL2_ttf)

# TODO need to copy DLLs.

add_subdirectory(src)


#target_sources(Jobius PRIVATE "ComponentArray.cpp" "EntityManager.cpp" "RenderSystem.cpp")
#target_include_directories(Jobius PUBLIC ${SDL2_INCLUDE_DIR})
#target_link_libraries(Jobius ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY})    