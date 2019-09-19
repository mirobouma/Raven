set(PROJECT_NAME raven-editor)

set(SourceGroup_PublicIncludes
    "public/editor/editor.h"
)

source_group("Public" FILES ${SourceGroup_PublicIncludes})

set(SourceGroup_Source
	"private/editor.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})
set(SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

add_executable(${PROJECT_NAME} ${SOURCES})   

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(${PROJECT_NAME} PUBLIC ${PLATFORM_MODULE} raven-core)