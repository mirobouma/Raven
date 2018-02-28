
add_library(sdl2 INTERFACE)
target_include_directories(sdl2 INTERFACE ${SDL2_INCLUDE_DIRS})
target_link_libraries(sdl2 INTERFACE ${SDL2_LIBS})

if(WIN32)
    foreach(DLL ${SDL2_DLLS})
        add_custom_command(TARGET ${MODULE_NAME} POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy_if_different ${DLL} $<TARGET_FILE_DIR:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}>)
    endforeach()
endif()

