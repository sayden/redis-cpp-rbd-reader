cmake_minimum_required(VERSION 3.20)

project(Rediscpp LANGUAGES CXX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=pointer-compare -fsanitize=pointer-subtract")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=pointer-compare -fsanitize=pointer-subtract")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=pointer-compare -fsanitize=pointer-subtract")

add_compile_options(-Wall -Wextra -Wpedantic -Werror -g -O0 -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=pointer-compare -fsanitize=pointer-subtract)
# add_compile_options(-Wall -Wextra -Wpedantic -Werror -g)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(rediscpp ${SOURCE_FILES})

# Testing framework
enable_testing()
# this is heuristically generated, and may not be correct
find_package(Catch2 CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS)
file(GLOB_RECURSE TEST_FILES test/*.cpp)
add_executable(tests ${SOURCE_FILES} ${TEST_FILES} )
target_link_libraries(rediscpp PRIVATE Catch2::Catch2 Catch2::Catch2WithMain Boost::boost)
add_test(NAME tests COMMAND tests)
