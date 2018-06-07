#Project Definition
set(PROJECT_NAME "RavenGame")


set (SourceGroup_Source
	"Private/Main.cpp"
	"Private/GameWindow.h"
	"Private/GameWindow.cpp"
	"Private/ConsoleLogger.h"
	"Private/ConsoleLogger.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
)

if (WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCE} ${RAVEN_ICON_RESOURCE_PATH})
elseif(UNIX AND NOT APPLE)
	add_executable(${PROJECT_NAME} ${SOURCE} ${RAVEN_ICON_RESOURCE_PATH})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE RavenEngine RavenUI assimp stb_image)
#target_compile_definitions(${PROJECT_NAME} PUBLIC -DMALLOC_CHECK_)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

if(NOT FIX_OUTPUT_PATH)
	INSTALL(DIRECTORY ${RVN_ROOT_SOURCE}/models/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/models)
	INSTALL(DIRECTORY ${RVN_ROOT_SOURCE}/textures/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/textures)
	INSTALL(DIRECTORY ${RVN_ROOT_SOURCE}/fonts/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/fonts)
 else()
	INSTALL(DIRECTORY ${RVN_ROOT_SOURCE}/models/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/models)
	INSTALL(DIRECTORY ${RVN_ROOT_SOURCE}/textures/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/textures)
	INSTALL(DIRECTORY ${RVN_ROOT_SOURCE}/fonts/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fonts)
 endif()
 

