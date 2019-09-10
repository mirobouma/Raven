macro(project_add_third_party name)
  add_subdirectory(src/third_party/${name} ${ARGN})
endmacro(project_add_third_party)

macro(project_add_application name)
  add_subdirectory(src/applications/${name} ${ARGN})
endmacro(project_add_application)

macro(project_add_engine_module name)
  add_subdirectory(src/engine/${name} ${ARGN})
endmacro(project_add_engine_module)

macro(project_add_plugin name)
  add_subdirectory(src/plugins/${name} ${ARGN})
endmacro(project_add_plugin)

macro(project_add_os name)
  add_subdirectory(src/os/${name} ${ARGN})
endmacro(project_add_os)

macro(project_add_tool name)
  add_subdirectory(src/tools/${name} ${ARGN})
endmacro(project_add_tool)

# Fix output directories for VS "Open folder" builds.
macro(set_output_paths)
    if(FIX_OUTPUT_PATH)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${RVN_OUTPUT_LIB_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${RVN_OUTPUT_LIB_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${RVN_OUTPUT_BIN_DIR}/${CMAKE_BUILD_TYPE}_${RVN_ARCHITECTURE}")
        
    else()
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${RVN_OUTPUT_LIB_DIR}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${RVN_OUTPUT_LIB_DIR}")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${RVN_OUTPUT_BIN_DIR}")
    endif()
endmacro(set_output_paths)

