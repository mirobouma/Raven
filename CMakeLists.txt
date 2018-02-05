cmake_minimum_required(VERSION 3.8.0)

set(PROJECT_NAME "Centurion")
set(PROJECT_VERSION 0.0.1)
set(CENTURION_OUTPUT_DIR "bin")

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

#options
option(BUILD_DOCUMENTATION "Build documentation" OFF)

#check if we are building for a 64 or 32 bit architecture.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
   set(CENTURION_ARCHITECTURE "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
   set(CENTURION_ARCHITECTURE "x86")
endif()

#set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${CENTURION_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}_${CENTURION_ARCHITECTURE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${CENTURION_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}_${CENTURION_ARCHITECTURE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${CENTURION_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}_${CENTURION_ARCHITECTURE}")

# check if Doxygen is installed
if(BUILD_DOCUMENTATION)
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
		# set input and output files
		set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
		set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

		# request to configure the file
		configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
		message("Doxygen build started")

		# note the option ALL which allows to build the docs together with the application
		add_custom_target( doc_doxygen ALL
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating API documentation with Doxygen"
			VERBATIM )

	else (DOXYGEN_FOUND)
	  message("Doxygen need to be installed to generate the doxygen documentation")
	endif (DOXYGEN_FOUND)
endif (BUILD_DOCUMENTATION)

#add submodules
add_subdirectory(tools)
add_subdirectory(cmake)
add_subdirectory(docs)
add_subdirectory(modules)
add_subdirectory(applications)
