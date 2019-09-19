set(PROJECT_NAME raven-math)

set (SourceGroup_PublicIncludes
    "public/math/math.h"
)
source_group("Public" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/math.cpp"
)

source_group("Source" FILES ${SourceGroup_Source})
set (SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

if(BUILD_MONOLITHIC)
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
else()
    add_library(${PROJECT_NAME} ${SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

find_package(blaze)
if(blaze_FOUND)
   add_library(blaze_target INTERFACE)
   target_link_libraries(${PROJECT_NAME} INTERFACE blaze::blaze)
endif()
