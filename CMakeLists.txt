cmake_minimum_required(VERSION 2.8)
project(chipster)

# Find SDL
find_package(SDL REQUIRED)

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-O0 {$CMAKE_C_FLAGS} -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-O2 {$CMAKE_C_FLAGS} -DNDEBUG")
aux_source_directory(./src CHIPSTER_SOURCES)

add_executable(chipster ${CHIPSTER_SOURCES})
target_link_libraries(chipster ${SDL_LIBRARY})
