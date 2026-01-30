cmake_minimum_required(VERSION 3.10)

project(example_d2gl_basics)

# ----------------
# Project Settings
# ----------------
# by default the build type is debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# --------------------
# Collect Source Files
# --------------------
# (The commlib files are compiled along with the example)
file(GLOB sources ./*.c ../../src/*.c ../../src/*.h ../../include/*.h)

# ----------------
# Build Executable
# ----------------
add_executable(d2gl_basics ${sources})
target_include_directories(d2gl_basics PRIVATE ../../include)
if(WIN32 OR MINGW OR MSYS)
    target_link_libraries(d2gl_basics PRIVATE setupapi ws2_32)
endif()
