cmake_policy(SET CMP0074 NEW)
find_package(SDL2 REQUIRED COMPONENTS main)
add_library(sdl2 INTERFACE)
target_include_directories(sdl2 INTERFACE ${SDL2_INCLUDE_DIRS})
target_link_libraries(sdl2 INTERFACE ${SDL2_LIBS})

#if(WIN32)
#    foreach(DLL ${SDL2_DLLS})
#        add_custom_command(TARGET Raven POST_BUILD COMMAND
#            ${CMAKE_COMMAND} -E copy_if_different ${DLL} $<TARGET_FILE_DIR:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}>)
#    endforeach()
#endif()

