cmake_minimum_required(VERSION 3.24)
project(sfml-cpp-game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Point to your laptop's MSYS2 SFML directory
set(SFML_DIR "C:/msys64/ucrt64/lib/cmake/SFML")
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

# GLOB_RECURSE to automatically detect source files inside the src/ folder
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# Define target executable assembly
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)

# ─────────────────────────────────────────────────────────────────────────────
# PROFESSIONAL ASSET PIPELINE AUTOMATION
# ─────────────────────────────────────────────────────────────────────────────
# This rule triggers every time you hit compile. It uses CMake's OS-agnostic 
# file system tools to clone your 'assets/' folder directly into your compiler 
# output workspace directory (e.g. build/). This prevents runtime file-not-found errors.
add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_BINARY_DIR}/assets"
    COMMENT "Syncing game asset catalog directly to runtime build execution target folder..."
)

# Force the main game target executable to wait for assets to sync before finishing compilation
add_dependencies(${PROJECT_NAME} CopyAssets)