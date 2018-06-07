#find_library(ASSIMP_LIBRARIES NAMES assimp libassimp.dll.a PATHS ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(ASSIMP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
find_package(ASSIMP REQUIRED)

add_library(assimp INTERFACE)
target_include_directories(assimp INTERFACE ${assimp_INCLUDE_DIRS})
target_link_libraries(assimp INTERFACE ${assimp_LIBRARIES})

ASSIMP_COPY_BINARIES(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_dependencies(assimp AssimpCopyBinaries)

