#find_library(ASSIMP_LIBRARIES NAMES assimp libassimp.dll.a PATHS ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(ASSIMP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
find_package(ASSIMP REQUIRED)

add_library(assimp INTERFACE)
target_include_directories(assimp INTERFACE ${assimp_INCLUDE_DIRS})
target_link_libraries(assimp INTERFACE ${assimp_LIBRARIES})

if(NOT FIX_OUTPUT_PATH)
ASSIMP_COPY_BINARIES(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/)
else()
ASSIMP_COPY_BINARIES(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
endif()
add_dependencies(assimp AssimpCopyBinaries)

