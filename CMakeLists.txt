cmake_minimum_required(VERSION 2.8)
project(chipster)

# Find SDL
find_package(SDL REQUIRED)

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-O0 -Wall -Werror -Wextra -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-O2 -Wall -Werror -Wextra -DNDEBUG")
aux_source_directory(./src CHIPSTER_SOURCES)

add_executable(chipster ${CHIPSTER_SOURCES})
target_link_libraries(chipster ${SDL_LIBRARY})
install(TARGETS chipster DESTINATION bin)
