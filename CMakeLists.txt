cmake_minimum_required(VERSION 3.21)

# --- 1. THE CACHE KILLERS (MUST BE BEFORE EVERYTHING ELSE) ---
# This stops the re-downloading of bindings and dependencies
# --- 1. THE CACHE KILLERS (ONLY FOR YOUR PC) ---
if (EXISTS "D:/geode-cache")
    set(GEODE_BINDINGS_REPO_PATH "D:/geode-bindings" CACHE PATH "Local bindings" FORCE)
    set(CPM_SOURCE_CACHE "D:/geode-cache" CACHE PATH "Global CPM cache" FORCE)
    message(STATUS "Using local D: drive cache for speed!")
else()
    message(STATUS "No D: drive found, using default GitHub Action paths.")
endif()


# 2. Set C++23 globally
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(dima.test_mod VERSION 1.0.0)

# --- 3. CLANG-CL & MSVC OPTIMIZATIONS ---
if (MSVC)
    add_compile_options(/permissive-)
    add_compile_options(/Zc:preprocessor)
    add_compile_options(/utf-8 /FS)
    add_compile_options(/Zc:__cplusplus)
    
    # SPEED HACK: Use the LLD linker if you're using clang-cl
    # This makes the final "Linking" bar almost instant.
    add_link_options("-fuse-ld=lld")
endif()

# Locate Geode SDK
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK!")
endif()

# --- 4. THE FIX FOR GEODE 5.0 ---
# Geode 5.0 PCH can sometimes cause "Internal Compiler Errors" (ICE) with Clang
set(GEODE_DISABLE_PCH ON CACHE BOOL "" FORCE)

# Add Geode Subdirectory
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Collect source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Create the mod library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Configure the mod
setup_geode_mod(${PROJECT_NAME})

