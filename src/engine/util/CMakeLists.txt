set(PROJECT_NAME raven-util)

set (SourceGroup_PublicIncludes
    "public/util/enum_as_flags.h"
    "public/util/result.h"
    "public/util/logger.h"
    "public/util/logging_macros.h"
    "public/util/memory.h"
    "public/util/path.h"
    "public/util/hash.h"
)
source_group("Public" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/util.cpp"
    "private/logger.cpp"
    "private/path.cpp"
)

source_group("Source" FILES ${SourceGroup_Source})
set (SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

if(BUILD_MONOLITHIC)
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
else()
    add_library(${PROJECT_NAME} ${SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")
