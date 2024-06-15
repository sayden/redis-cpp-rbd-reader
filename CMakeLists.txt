cmake_minimum_required(VERSION 3.20)

project(Rediscpp LANGUAGES CXX)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(rediscpp ${SOURCE_FILES})

# Testing framework
enable_testing()
find_package(Catch2 CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS)
file(GLOB_RECURSE TEST_FILES test/*.cpp)
add_executable(tests ${SOURCE_FILES} ${TEST_FILES} )
target_link_libraries(rediscpp PRIVATE Catch2::Catch2 Catch2::Catch2WithMain Boost::boost)
add_test(NAME tests COMMAND tests)
