cmake_minimum_required(VERSION 2.8)
project(chipster)

# Find SDL
find_package(SDL REQUIRED)

# src globbing
FILE(GLOB Sources src/*.c)
FILE(GLOB Headers src/*.h)

add_executable(chipster ${Sources} ${Headers})
target_link_libraries(chipster ${SDL_LIBRARY})
