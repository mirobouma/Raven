find_library(ASSIMP_LIBRARIES NAMES assimp libassimp.dll.a PATHS ${CMAKE_CURRENT_SOURCE_DIR}/libs)
find_package(ASSIMP REQUIRED)

add_library(assimp INTERFACE)
target_include_directories(assimp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(assimp INTERFACE ${ASSIMP_LIBRARIES})

