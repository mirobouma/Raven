cmake_minimum_required(VERSION 3.8.0)
cmake_policy(SET CMP0074 NEW)

set(RVN_PROJECT_NAME "Raven")
set(RVN_PROJECT_VERSION 0.1.1)
set(RVN_OUTPUT_BIN_DIR "${CMAKE_SOURCE_DIR}/build/bin")
set(RVN_OUTPUT_LIB_DIR "${CMAKE_SOURCE_DIR}/build/lib")
set(RVN_ICON_RESOURCE_PATH "${CMAKE_SOURCE_DIR}/resources/icon.rc")
set(RVN_ROOT_SOURCE "${CMAKE_SOURCE_DIR}")

# Cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(ProjectGlobalMacros)
include(GlslToSpirV)
include(ProjectTestFunctions)
include(CopyFiles)

project(${RVN_PROJECT_NAME} VERSION ${RVN_PROJECT_VERSION} LANGUAGES C CXX)
enable_testing()
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Check for C++11/14/17 support
project_check_cpp_version()

if (NOT CPP11_SUPPORT)
  message(FATAL_ERROR "C++11 support is required.")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Options
option(BUILD_DOCUMENTATION "Build documentation" OFF)
option(FIX_OUTPUT_PATH "Fix output directories for non-vs-project builds." OFF)

# Check if we are building for a 64 or 32 bit architecture.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
   set(RVN_ARCHITECTURE "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
   set(RVN_ARCHITECTURE "x86")
endif()

if (WIN32)
	add_definitions(-DPLATFORM_WINDOWS) # We are compiling for windows.
elseif(UNIX AND NOT APPLE)
	add_definitions(-DPLATFORM_LINUX) # We are compiling for linux.
endif()

#find packages
#On Linux, search for SDL2
if(UNIX AND NOT APPLE)
	find_package(SDL2 REQUIRED COMPONENTS main)
endif()

find_package(Vulkan)
include_directories(${MANUAL_INCLUDE_DIRS})

# Set output directories
if(FIX_OUTPUT_PATH)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${RVN_OUTPUT_LIB_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${RVN_OUTPUT_LIB_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${RVN_OUTPUT_BIN_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
	
else()
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${RVN_OUTPUT_LIB_DIR}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${RVN_OUTPUT_LIB_DIR}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${RVN_OUTPUT_BIN_DIR}")
endif()

set(GLM_INSTALL_ENABLE OFF CACHE BOOL "GLM install" FORCE)

#add third-party modules.
project_add_3rdparty(json)
project_add_3rdparty(boost_context)
project_add_3rdparty(glm)
project_add_3rdparty(stb)
project_add_3rdparty(imgui)
project_add_3rdparty(sdl2)
project_add_3rdparty(vma)
project_add_3rdparty(assimp)

#add tools
project_add_tool(rrg)

##add modules
project_add_module(core)
project_add_module(hal)
project_add_module(platform)
project_add_module(renderer_interface)
project_add_module(renderer_vulkan)
project_add_module(rendering)
project_add_module(ui)
project_add_module(engine)

if (WIN32)
	project_add_module(platform_windows)
endif()
if(UNIX AND NOT APPLE)
	project_add_module(platform_linux)
endif()

#applications
project_add_application(game)
