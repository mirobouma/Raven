set(PROJECT_NAME raven-core)

set (SourceGroup_PublicIncludes
    "public/core/engine.h"
    "public/core/main/application.h"
    "public/core/main/application_loop.h"
    "public/core/main/game_loop.h"
)
source_group("Public" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
    "private/main.cpp"
	"private/core.cpp"
    "private/engine.cpp"
    "private/main/application.cpp"
    "private/main/game_loop.cpp"
)

source_group("Source" FILES ${SourceGroup_Source})
set (SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

if(BUILD_MONOLITHIC)
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
else()
    add_library(${PROJECT_NAME} ${SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")
target_link_libraries(${PROJECT_NAME} PUBLIC raven-os raven-renderer-api raven-math raven-util)
