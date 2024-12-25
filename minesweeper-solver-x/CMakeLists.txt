# CMake project for minesweeper-solver-x, include source and define project-specific logic here.

# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(msx)

# Define the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect source files
set(SOURCES
    board.cpp
    game.cpp
    msx.cpp
    screen.cpp
    solver.cpp
)

# Collect header files (optional but good for IDE organization)
set(HEADERS
    board.h
    game.h
    screen.h
    solver.h
)

# Resources (e.g. manifest)
set(MANIFEST app.manifest)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${MANIFEST})