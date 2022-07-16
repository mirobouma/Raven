set(PROJECT_NAME raven-renderer-api)

set (SourceGroup_PublicIncludes
    "public/rendering/buffer.h"
    "public/rendering/command_list.h"
    "public/rendering/command_pool.h"
    "public/rendering/defines.h"
    "public/rendering/fence.h"
    "public/rendering/image.h"
    "public/rendering/pipeline.h"
    "public/rendering/queue.h"
    "public/rendering/render_target.h"
    "public/rendering/rendering_api.h"
    "public/rendering/root_signature.h"
    "public/rendering/sampler.h"
    "public/rendering/semaphore.h"
    "public/rendering/shader.h"
    "public/rendering/swap_chain.h"
)
source_group("Public" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/rendering_api.cpp"
)

source_group("Source" FILES ${SourceGroup_Source})
set (SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

if(BUILD_MONOLITHIC)
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
else()
    add_library(${PROJECT_NAME} ${SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")
target_link_libraries(${PROJECT_NAME} PUBLIC raven-math raven-util raven-os-api)
