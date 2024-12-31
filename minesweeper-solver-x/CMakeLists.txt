# Project
cmake_minimum_required(VERSION 3.12)
project(msx)

# Define the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sources
set(SOURCES
    board.cpp
    game.cpp
    msx.cpp
    screen.cpp
    solver.cpp
    virtual.cpp
    bench.cpp
)

# Headers
set(HEADERS
    board.h
    game.h
    screen.h
    solver.h
    virtual.h
    bench.h
)

# Manifest
set(MANIFEST app.manifest)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${MANIFEST})