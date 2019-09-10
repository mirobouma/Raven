cmake_minimum_required(VERSION 3.8.0)

# Helper variables.
set(RVN_ENGINE_NAME "Raven")
set(RVN_PROJECT_VERSION 0.1.1)
set(RVN_OUTPUT_BIN_DIR "${CMAKE_SOURCE_DIR}/build/bin")
set(RVN_OUTPUT_LIB_DIR "${CMAKE_SOURCE_DIR}/build/lib")
set(RVN_ICON_RESOURCE_PATH "${CMAKE_SOURCE_DIR}/resources/icon.rc")
set(RVN_ROOT_SOURCE "${CMAKE_SOURCE_DIR}")

# CMake scripts.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckCppVersion)
include(CopyFiles)
include(RavenProject)
include(RavenPlatform)

# Options
option(FIX_OUTPUT_PATH "Fix output directories for non-vs-project builds." OFF)
option(BUILD_EDITOR "Build the editor." ON)
option(BUILD_MONOLITHIC "Builds the project in monolithic mode" OFF)

project(${RVN_ENGINE_NAME} VERSION ${RVN_PROJECT_VERSION} LANGUAGES C CXX)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
   set(RVN_ARCHITECTURE "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
   set(RVN_ARCHITECTURE "x86")
endif()

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Check for C++11/14/17 support
project_check_cpp_version()

if(NOT CPP11_SUPPORT)
  message(FATAL_ERROR "C++11 support is required.")
endif()

# Checks for which platform we are compiling.
check_platform()

# Set output paths for binaries.
set_output_paths()

# Add correct module for this platform.
add_platform_module()

# Add third-party modules.
#project_add_third_party(blaze)
project_add_third_party(sdl2)
project_add_third_party(vma)
project_add_third_party(tinystl)

# Add modules
project_add_engine_module(core)
project_add_engine_module(math)
project_add_engine_module(plugin)
project_add_engine_module(rendering)
project_add_engine_module(util)

# Add applications
project_add_application(game)