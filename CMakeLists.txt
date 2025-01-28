cmake_minimum_required(VERSION 3.12)
project(iodd_parser)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug mode option
option(ENABLE_DEBUG "Enable debug mode" OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Configure debug definitions
if(ENABLE_DEBUG)
    add_definitions(-DDEBUG_MODE)
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Debug mode enabled")
else()
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Debug mode disabled")
endif()

# Collect source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.c"
)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/inc)

# Create executable
add_executable(${PROJECT_NAME} 
    ${SOURCES}
    main.cpp
)

# Compiler warnings
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wno-sign-compare 
)

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Copy IODD files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/ioddfiles DESTINATION ${CMAKE_BINARY_DIR})
