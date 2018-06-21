cmake_minimum_required(VERSION 3.8.0)

SET(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)

SET(CMAKE_C_RESPONSE_FILE_LINK_FLAG "@")
SET(CMAKE_CXX_RESPONSE_FILE_LINK_FLAG "@")
SET(CMAKE_NINJA_FORCE_RESPONSE_FILE 1 CACHE INTERNAL "")


set(RVN_PROJECT_NAME "Raven")
set(RVN_PROJECT_VERSION 0.0.1)
set(RVN_OUTPUT_DIR "bin")
set(RVN_ICON_RESOURCE_PATH "${CMAKE_SOURCE_DIR}/resources/icon.rc")
set(RVN_ROOT_SOURCE "${CMAKE_SOURCE_DIR}")

# Cmake modules
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(ProjectGlobalMacros)
include(GlslToSpirV)
include(ProjectTestFunctions)
include(CopyFiles)

project(${RVN_PROJECT_NAME} VERSION ${RVN_PROJECT_VERSION} LANGUAGES C CXX)
enable_testing()
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Check for C++11/14/17 support
project_check_cpp_version()

## Snippets to enforce a C++ version
#if (not CPP14_SUPPORT)
#  message(FATAL_ERROR "C++14 support is required.")
#endif()

if (NOT CPP11_SUPPORT)
  message(FATAL_ERROR "C++11 support is required.")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Tooling - some must be declared before adding other subdirectories
#project_enable_sanitizer_build()
#project_enable_documentation()

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
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
	
else()
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}")
endif()

set(GLM_INSTALL_ENABLE OFF CACHE BOOL "GLM install" FORCE)

#add third-party modules.
project_add_3rdparty(json)
project_add_3rdparty(boost_context)
project_add_3rdparty(glm)
project_add_3rdparty(stb)
project_add_3rdparty(imgui)
project_add_3rdparty(sdl2)
project_add_3rdparty(gainput)
project_add_3rdparty(VulkanMemoryAllocator)
project_add_3rdparty(assimp-3.3.1)
project_add_module(RavenMath)
project_add_module(RavenSerialization)
project_add_module(RavenCore)
project_add_module(RavenIO)
project_add_module(RavenPlugin)
project_add_module(RavenHAL)
project_add_module(RavenPlatform)
project_add_module(RavenRendering)
project_add_module(RavenRenderingVulkan)
project_add_module(RavenUI)
project_add_module(RavenEngine)
project_add_module(RavenWindowSystem)

if (WIN32)
	project_add_module(RavenPlatformWindows)
	project_add_module(RavenRenderingDX12)
endif()
if(UNIX AND NOT APPLE)
	project_add_module(RavenPlatformLinux)
endif()



#applications
project_add_application(RavenEditor)
project_add_application(RavenGame)
