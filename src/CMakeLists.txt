# Project
cmake_minimum_required(VERSION 3.12)
project(msx)

# Define the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create library targets for each component
add_library(utils STATIC
    utils/screen.cpp
    utils/util.cpp
    utils/terminal.cpp
)

add_library(core STATIC
    core/board.cpp
    core/solver.cpp
    core/game.cpp
)

add_library(games STATIC
    games/google.cpp
    games/virtual.cpp
)

add_library(benchmarks STATIC
    benchmarks/bench.cpp
)

# Set include directories for each library
target_include_directories(utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(games PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(benchmarks PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Create the main executable
add_executable(msx 
    app/app.manifest
    app/msx.cpp
)

# Link the main executable with all required libraries
target_link_libraries(msx PRIVATE
    benchmarks
    games
    core
    utils
)

# Copy the manifest file to the output directory
if(MSVC)
    add_custom_command(
        TARGET msx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/app/app.manifest
            $<TARGET_FILE_DIR:msx>/app.manifest
    )
endif()
