set(PROJECT_NAME raven-game)

set(SourceGroup_PublicIncludes
    "public/game/game.h"
)

source_group("Public" FILES ${SourceGroup_PublicIncludes})

set(SourceGroup_Source
	"private/game.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})
set(SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

if(BUILD_MONOLITHIC)
    add_executable(${PROJECT_NAME} ${SOURCES}) 
    else()
    add_library(${PROJECT_NAME} ${SOURCES})
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(${PROJECT_NAME} PUBLIC raven-core)