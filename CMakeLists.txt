cmake_minimum_required(VERSION 3.14)
project(GraphicsLabFrameworkSolution C CXX)

# C/C++ Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(DEMO demo)


# Platform-specific flags
if(APPLE)
    add_compile_options(-Wno-deprecated-declarations -O3 -Wall -Wextra)
elseif(UNIX AND NOT APPLE)
    add_compile_options(-O3 -Wall -Wextra -pthread)
endif()

# SDL3 - Try system, else fetch from source
find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
    message(STATUS "SDL2 not found, downloading...")
    include(FetchContent)
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.7
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SDL2)
    add_compile_definitions(SDL2_BUILT_FROM_SOURCE)
else()
    message(STATUS "Found system SDL2")
endif()

# Spinach
# Collect C++ source files for spinach
file(GLOB_RECURSE GLF_C CONFIGURE_DEPENDS
    grf/*.c
)



# Collect headers 
file(GLOB_RECURSE GLF_H CONFIGURE_DEPENDS
    grf/*.h
)


# demo
# Collect C++ source files
file(GLOB_RECURSE DEMO_C CONFIGURE_DEPENDS
    demo/*.c
)

# Collect headers 
file(GLOB_RECURSE DEMO_H CONFIGURE_DEPENDS
	demo/*.h
)

# Create the executable
add_executable(	${DEMO}
	${DEMO_C}
	${DEMO_H}
    ${GLF_C}
    ${GLF_H} 
)

target_include_directories(${DEMO} PRIVATE
    grf
    demo
)

if(TARGET SDL2::SDL2main)
    target_link_libraries(${DEMO} PRIVATE SDL2::SDL2 SDL2::SDL2main)
else()
    target_link_libraries(${DEMO} PRIVATE SDL2::SDL2)
endif()

