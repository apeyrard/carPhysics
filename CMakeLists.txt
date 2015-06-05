cmake_minimum_required (VERSION 2.8)

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


### Sources ###

# Include directories
set(NEUROCAR_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
include_directories(${NEUROCAR_INCLUDE_DIR})

# Sources
set (NEUROCAR_SOURCE_DIR ./src)
set (NEUROCAR_SOURCES
    ${NEUROCAR_SOURCE_DIR}/renderer.cpp
    ${NEUROCAR_SOURCE_DIR}/world.cpp
    ${NEUROCAR_SOURCE_DIR}/drawable.cpp
    ${NEUROCAR_SOURCE_DIR}/staticbox.cpp
    ${NEUROCAR_SOURCE_DIR}/car.cpp
    ${NEUROCAR_SOURCE_DIR}/tire.cpp
    ${NEUROCAR_SOURCE_DIR}/raycastcallback.cpp
)


### Extern libraries ###

# Add custom CMake module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

# Setup directories
set(EXTERN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/headers)
set(EXTERN_STATIC_LIB_DIR ${CMAKE_SOURCE_DIR}/libs/static)
set(NEUROCAR_EXTERN_LIBRARIES "")

# Box2D
set(EXTERN_INCLUDE_DIR_BOX2D ${EXTERN_INCLUDE_DIR}/)
set(EXTERN_LIBRARIES_BOX2D ${EXTERN_STATIC_LIB_DIR}/Box2D/libBox2D.a)
set(NEUROCAR_EXTERN_LIBRARIES ${NEUROCAR_EXTERN_LIBRARIES} ${EXTERN_LIBRARIES_BOX2D})
include_directories(SYSTEM ${EXTERN_INCLUDE_DIR_BOX2D})

# SFML
find_package(SFML 2 REQUIRED system window graphics)
if(SFML_FOUND)
    set(NEUROCAR_EXTERN_LIBRARIES ${NEUROCAR_EXTERN_LIBRARIES} ${SFML_LIBRARIES})
    include_directories(SYSTEM ${SFML_INCLUDE_DIR})
endif()

### Static library ###
set(LIBRARY_OUTPUT_PATH ${NEUROCAR_BINARY_DIR})
set(NEUROCAR_STATIC_LIBRARY NeuroCarPhysics)
add_library(${NEUROCAR_STATIC_LIBRARY} STATIC ${NEUROCAR_SOURCES})
target_link_libraries(${NEUROCAR_STATIC_LIBRARY} ${NEUROCAR_EXTERN_LIBRARIES})


### Executable ###
set(EXECUTABLE_NAME neuroCar)
set(EXECUTABLE_OUTPUT_PATH ${NEUROCAR_BINARY_DIR})
add_executable(${EXECUTABLE_NAME} ${NEUROCAR_SOURCE_DIR}/main.cpp)
target_link_libraries(${EXECUTABLE_NAME} ${NEUROCAR_STATIC_LIBRARY})
