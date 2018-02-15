# Feature detection
find_program(GCOV_EXE gcov)
find_program(LCOV_EXE lcov)
find_program(GENHTML_EXE genhtml)
find_package(PythonInterp)
find_program(MAKE_EXE make)
find_program(DOXYGEN_EXE doxygen)

# Check C++ version
macro(project_check_cpp_version)
  if (NOT MSVC)
    # Tests for Clang and GCC
    check_cxx_compiler_flag(-std=c++1y CPP14_SUPPORT)
    if (CPP14_SUPPORT)
      set(CPP11_SUPPORT TRUE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
      message("-- C++14 support found.")
    else()
      check_cxx_compiler_flag(-std=c++11 CPP11_SUPPORT)
      if (CPP11_SUPPORT)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        message("-- C++11 support found.")
      endif()
    endif()
  else()
    # Tests for MSVC
    # Unfortunately, due to various unsupported things in msvc versions,
    # this is poor informatiion about actual support
    check_cxx_source_compiles("#include <utility>\nusing std::integer_sequence;\n int main(){return 0;}" CPP14_SUPPORT)
    if (CPP14_SUPPORT)
      set(CPP11_SUPPORT TRUE)
      message("-- C++14 support found.")
    else()
      check_cxx_source_compiles("static constexpr int TEST=0;\n int main(){return 0;}" CPP11_SUPPORT)
      if (CPP11_SUPPORT)
        message("-- C++11 support found.")
      endif()
    endif ()
  endif()
endmacro(project_check_cpp_version)


# Add 3rdparty for main file
macro(project_add_3rdparty name)
  add_subdirectory(third_party/${name} ${ARGN})
endmacro(project_add_3rdparty)

# Add module for main file
macro(project_add_application name)
  add_subdirectory(applications/${name} ${ARGN})
endmacro(project_add_application)

# Add module for main file
macro(project_add_module name)
  add_subdirectory(modules/${name} ${ARGN})
endmacro(project_add_module)


# Enable sanitizers builds
macro(project_enable_sanitizer_build)
  configure_file(${PROJECT_SOURCE_DIR}/cmake/SanitizerBlacklist.txt.in ${PROJECT_BINARY_DIR}/SanitizerBlacklist.txt)
  if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(SAN ${PROJECT_BUILD_SANITIZER_TYPE})
    if (NOT ${SAN} STREQUAL "")
      if (${SAN} STREQUAL "address"
          OR ${SAN} STREQUAL "memory"
          OR ${SAN} STREQUAL "thread"
          OR ${SAN} STREQUAL "undefined")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=${SAN} -fsanitize-blacklist=${PROJECT_BINARY_DIR}/SanitizerBlacklist.txt")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fsanitize=${SAN} -fsanitize-blacklist=${PROJECT_BINARY_DIR}/SanitizerBlacklist.txt")
      else()
        message(FATAL_ERROR "Clang sanitizer ${SAN} is unknown.")
      endif()
    endif()
  endif()
  if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(SAN ${PROJECT_BUILD_SANITIZER_TYPE})
    if (NOT ${SAN} STREQUAL "")
      if (${SAN} STREQUAL "address"
          OR ${SAN} STREQUAL "thread"
          OR ${SAN} STREQUAL "undefined")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=${SAN}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fsanitize=${SAN}")
      else()
        message(FATAL_ERROR "GCC sanitizer ${SAN} is unknown.")
      endif()
    endif()
  endif()
endmacro(project_enable_sanitizer_build)

# Format the whole source code with clang-format
macro(project_enable_documentation)
  add_custom_target(docs)
  if(NOT ${DOXYGEN_EXE} STREQUAL "DOXYGEN_EXE-NOTFOUND")
    message("-- Doxygen found, targets 'docs-doxygen-html' and 'docs-doxygen-xml' enabled.")
      add_custom_target(docs-doxygen-xml
      COMMAND ${DOXYGEN_EXE} docs/doxygen/source/doxyxml.conf
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
    add_custom_target(docs-doxygen-html
      COMMAND ${DOXYGEN_EXE} docs/doxygen/source/doxyhtml.conf
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
    # Dependency to XML will be added by sphinx, not the default
    add_dependencies(docs docs-doxygen-html)
    if (${PYTHONINTERP_FOUND} AND NOT ${MAKE_EXE} STREQUAL "MAKE_EXE-NOTFOUND")
      message("-- Python, doxygen and make found, target 'docs-sphinx' enabled.")
	  add_custom_target(docs-sphinx-apigen
        DEPENDS docs-doxygen-xml
        COMMAND ${PythonInterp} breathe-apidoc -o "${PROJECT_SOURCE_DIR}/docs/sphinx/source/api" "${PROJECT_SOURCE_DIR}/docs/doxygen/build/xml" 
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/docs/sphinx
      )
      add_custom_target(docs-sphinx
        DEPENDS docs-doxygen-xml docs-sphinx-apigen
        COMMAND ${MAKE_EXE} html
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/docs/sphinx
      )
      add_dependencies(docs docs-sphinx)
    endif()
  endif()
endmacro(project_enable_documentation)