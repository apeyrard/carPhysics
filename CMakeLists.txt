cmake_minimum_required (VERSION 3.0)

# Useful macros
include(${CMAKE_SOURCE_DIR}/cmake/macros.cmake)

# Requires an out of source build
MACRO_ENSURE_OUT_OF_SOURCE_BUILD("Requires an out of source build.")

project(neuroCar)

set (CMAKE_BUILD_TYPE debug)

set (NEUROCAR_VERSION_MAJOR 0)
set (NEUROCAR_VERSION_MINOR 1)
set (NEUROCAR_VERSION_PATCH 0)
set (NEUROCAR_VERSION ${NEUROCAR_VERSION_MAJOR}.${NEUROCAR_VERSION_MINOR}.${NEUROCAR_VERSION_PATCH})

set(NEUROCAR_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})



# Flags
if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-g -O2 -Wall -Wno-sign-compare")
    add_definitions(-std=gnu++0x)
endif()

# Include directories
set(NEUROCAR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
include_directories(${NEUROCAR_INCLUDE_DIR})

# Sources
set (NEUROCAR_SOURCE_DIR ./src)
set (NEUROCAR_SOURCES
    ${NEUROCAR_SOURCE_DIR}/main.cpp
    ${NEUROCAR_SOURCE_DIR}/renderer.cpp
    ${NEUROCAR_SOURCE_DIR}/world.cpp
    ${NEUROCAR_SOURCE_DIR}/drawable.cpp
    ${NEUROCAR_SOURCE_DIR}/staticbox.cpp
    ${NEUROCAR_SOURCE_DIR}/car.cpp
    ${NEUROCAR_SOURCE_DIR}/tire.cpp
    ${NEUROCAR_SOURCE_DIR}/raycastcallback.cpp
)

# Executable
set(EXECUTABLE_OUTPUT_PATH ${NEUROCAR_BINARY_DIR})

add_executable(neuroCar ${NEUROCAR_SOURCES})

# libraries
find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(neuroCar ${SFML_LIBRARIES})
endif()

target_link_libraries(neuroCar /usr/local/lib/libBox2D.a)
