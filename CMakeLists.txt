cmake_minimum_required(VERSION 3.8.0)

set(RVN_PROJECT_NAME "Raven")
set(RVN_PROJECT_VERSION 0.0.1)
set(RVN_OUTPUT_DIR "bin")
set(RVN_ICON_RESOURCE_PATH "${CMAKE_SOURCE_DIR}/resources/icon.rc")

# Cmake modules
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(ProjectGlobalMacros)
include(ProjectTestFunctions)

project(${RVN_PROJECT_NAME} VERSION ${RVN_PROJECT_VERSION} LANGUAGES CXX)
enable_testing()
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Check for C++11/14 support
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
project_enable_sanitizer_build()
project_enable_documentation()

#options
option(BUILD_DOCUMENTATION "Build documentation" OFF)
option(FIX_OUTPUT_PATH "Fix output directories for non-vs-project builds." OFF)

#check if we are building for a 64 or 32 bit architecture.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
   set(RVN_ARCHITECTURE "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
   set(RVN_ARCHITECTURE "x86")
endif()

include_directories(${MANUAL_INCLUDE_DIRS})

#set output directories
if(FIX_OUTPUT_PATH)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
else()
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RVN_OUTPUT_DIR}")
endif()

#third party modules
project_add_3rdparty(json)
project_add_3rdparty(glm)

#first-party modules
project_add_module(RavenMath)
project_add_module(RavenSerialization)
project_add_module(RavenCore)
project_add_module(RavenIO)
project_add_module(RavenPlugin)
project_add_module(RavenPlatform)
project_add_module(RavenPlatformWin32)
project_add_module(RavenRendering)
project_add_module(RavenRenderingVulkan)
project_add_module(RavenUI)
project_add_module(RavenRenderingDX12)
project_add_module(RavenEngine)

#applications
project_add_application(RavenEditor)
project_add_application(RavenGame)