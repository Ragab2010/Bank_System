# Specify the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(bankSystem)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable compiler-specific extensions (optional, but recommended)

# Find the SQLite3 library
find_package(SQLite3 REQUIRED)

# Include header files from the "include" directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files from the "src" directory
file(GLOB SOURCES
    src/*.cpp
    bank_system.cpp
)

# Add the executable
add_executable(bank ${SOURCES})

# Link the SQLite3 library
target_link_libraries(bank PRIVATE SQLite::SQLite3)
