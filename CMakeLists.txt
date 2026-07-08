cmake_minimum_required(VERSION 3.24)
project(sfml-cpp-game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Point to your laptop's MSYS2 SFML directory
set(SFML_DIR "C:/msys64/ucrt64/lib/cmake/SFML")
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

# GLOB_RECURSE
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)