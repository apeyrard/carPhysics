cmake_minimum_required (VERSION 2.8)

# Useful macros
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/macros.cmake)

# Requires an out of source build
MACRO_ENSURE_OUT_OF_SOURCE_BUILD("Requires an out of source build.")

project(CarPhysics)


set (CMAKE_BUILD_TYPE debug)

set (CAR_PHYSICS_VERSION_MAJOR 0)
set (CAR_PHYSICS_VERSION_MINOR 1)
set (CAR_PHYSICS_VERSION_PATCH 0)
set (CAR_PHYSICS_VERSION ${CAR_PHYSICS_VERSION_MAJOR}.${CAR_PHYSICS_VERSION_MINOR}.${CAR_PHYSICS_VERSION_PATCH})

set(CAR_PHYSICS_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

### Compilation flags ###

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-g -O2 -Wall -Wno-sign-compare")
    add_definitions(-std=gnu++0x)
endif()

### Definitions ###

# Enable/disable graphic mode
if(NOT DEFINED CAR_PHYSICS_GRAPHIC_MODE)
    set(CAR_PHYSICS_GRAPHIC_MODE ON CACHE BOOL "Enable/Disable graphic mode")
endif()

if(CAR_PHYSICS_GRAPHIC_MODE)
    message(STATUS "Graphic mode enabled")
    add_definitions(-DCAR_PHYSICS_GRAPHIC_MODE_SFML=1)
else()
    message(STATUS "Graphic mode disabled")
    add_definitions(-DCAR_PHYSICS_GRAPHIC_MODE_SFML=0)
endif()


### Sources ###

# Include directories
set(CAR_PHYSICS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CAR_PHYSICS_INCLUDE_DIR})

# Sources
set (CAR_PHYSICS_SOURCE_DIR ./src)
set (CAR_PHYSICS_SOURCES
    ${CAR_PHYSICS_SOURCE_DIR}/renderer.cpp
    ${CAR_PHYSICS_SOURCE_DIR}/world.cpp
    ${CAR_PHYSICS_SOURCE_DIR}/drawable.cpp
    ${CAR_PHYSICS_SOURCE_DIR}/staticbox.cpp
    ${CAR_PHYSICS_SOURCE_DIR}/car.cpp
    ${CAR_PHYSICS_SOURCE_DIR}/tire.cpp
    ${CAR_PHYSICS_SOURCE_DIR}/raycastcallback.cpp
)


### Extern libraries ###

# Add custom CMake module path
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

# Setup directories
set(EXTERN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/headers)
set(EXTERN_STATIC_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/static)
set(CAR_PHYSICS_EXTERN_INCLUDE_DIR "")
set(CAR_PHYSICS_EXTERN_LIBRARIES "")

# Box2D
set(EXTERN_INCLUDE_DIR_BOX2D ${EXTERN_INCLUDE_DIR}/)
set(EXTERN_LIBRARIES_BOX2D ${EXTERN_STATIC_LIB_DIR}/Box2D/libBox2D.a)
set(CAR_PHYSICS_EXTERN_LIBRARIES ${CAR_PHYSICS_EXTERN_LIBRARIES} ${EXTERN_LIBRARIES_BOX2D})
set(CAR_PHYSICS_EXTERN_INCLUDE_DIR ${CAR_PHYSICS_EXTERN_INCLUDE_DIR} ${EXTERN_INCLUDE_DIR_BOX2D})
include_directories(SYSTEM ${EXTERN_INCLUDE_DIR_BOX2D})

# SFML
if(CAR_PHYSICS_GRAPHIC_MODE)
    find_package(SFML 2 REQUIRED system window graphics)
    if(SFML_FOUND)
        set(CAR_PHYSICS_EXTERN_LIBRARIES ${CAR_PHYSICS_EXTERN_LIBRARIES} ${SFML_LIBRARIES})
        set(CAR_PHYSICS_EXTERN_INCLUDE_DIR ${CAR_PHYSICS_EXTERN_INCLUDE_DIR} ${SFML_INCLUDE_DIR})
        include_directories(SYSTEM ${SFML_INCLUDE_DIR})
    endif()
endif()

### Static library ###
set(LIBRARY_OUTPUT_PATH ${CAR_PHYSICS_BINARY_DIR})
set(CAR_PHYSICS_STATIC_LIBRARY CarPhysicsLib)
add_library(${CAR_PHYSICS_STATIC_LIBRARY} STATIC ${CAR_PHYSICS_SOURCES})
target_link_libraries(${CAR_PHYSICS_STATIC_LIBRARY} ${CAR_PHYSICS_EXTERN_LIBRARIES})


### Executable ###
set(EXECUTABLE_NAME CarPhysics)
set(EXECUTABLE_OUTPUT_PATH ${CAR_PHYSICS_BINARY_DIR})
add_executable(${EXECUTABLE_NAME} ${CAR_PHYSICS_SOURCE_DIR}/main.cpp)
target_link_libraries(${EXECUTABLE_NAME} ${CAR_PHYSICS_STATIC_LIBRARY})

# Global variables
set(CAR_PHYSICS_INCLUDE_DIR ${CAR_PHYSICS_INCLUDE_DIR}
    CACHE STRING "CarPhysics include directory"
)
set(CAR_PHYSICS_SOURCE_DIR  ${CAR_PHYSICS_SOURCE_DIR}
    CACHE STRING "CarPhysics source directory"
)
set(CAR_PHYSICS_EXTERN_INCLUDE_DIR ${CAR_PHYSICS_EXTERN_INCLUDE_DIR}
    CACHE STRING "CarPhysics extern include directory"
)
set(CAR_PHYSICS_STATIC_LIBRARY ${CAR_PHYSICS_STATIC_LIBRARY}
    CACHE STRING "CarPhysics static library"
)
