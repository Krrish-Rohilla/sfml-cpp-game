cmake_minimum_required(VERSION 3.28) # Updated minimum for full modern C++ toolchain support
project(SkateboardPhysicsEngine VERSION 1.0 LANGUAGES CXX)

# Enforce C++26 standard
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Ensures clean, standard code without compiler-specific hacks

# Find native SFML 3 packages
find_package(SFML 3 REQUIRED COMPONENTS graphics window system)

# Gather source modules
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# Establish the engine target
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link engine modules together with SFML 3
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::graphics SFML::window SFML::system)