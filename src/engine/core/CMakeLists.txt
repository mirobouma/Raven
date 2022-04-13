set(PROJECT_NAME raven-core)

set (SourceGroup_PublicIncludes
    "public/core/engine.h"
    "public/core/errors.h"
    "public/core/string.h"
    "public/core/vector.h"
    "public/core/memory.h"
    "public/core/logging_macros.h"
    "public/core/io/path.h"
    "public/core/io/logger.h"
    "public/core/modules/module.h"
    "public/core/modules/module_manager.h"
    "public/core/os/os.h"
    "public/core/os/file_system.h"
    "public/core/os/dynamic_library.h"
    "public/core/os/memory.h"
    "public/core/os/monitor.h"
    "public/core/os/thread.h"
    "public/core/os/window.h"
    "public/core/os/window_system.h"
    "public/core/os/window_event_listener.h"
    "public/core/main/application.h"
    "public/core/main/application_loop.h"
    "public/core/main/game_loop.h"
    "public/core/main/application.h"
    "public/core/api/rendering/buffer.h"
    "public/core/api/rendering/command_list.h"
    "public/core/api/rendering/command_pool.h"
    "public/core/api/rendering/defines.h"
    "public/core/api/rendering/fence.h"
    "public/core/api/rendering/image.h"
    "public/core/api/rendering/pipeline.h"
    "public/core/api/rendering/queue.h"
    "public/core/api/rendering/render_target.h"
    "public/core/api/rendering/rendering_api.h"
    "public/core/api/rendering/root_signature.h"
    "public/core/api/rendering/sampler.h"
    "public/core/api/rendering/semaphore.h"
    "public/core/api/rendering/shader.h"
    "public/core/api/rendering/swap_chain.h"
)
source_group("Public" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/core.cpp"
    "private/engine.cpp"
    "private/main/application.cpp"
    "private/main/game_loop.cpp"
    "private/os/os.cpp" 
    "private/os/thread.cpp" 
    "private/os/window.cpp" 
    "private/os/window_system.cpp" 
    "private/io/path.cpp"
    "private/io/logger.cpp"
)

source_group("Source" FILES ${SourceGroup_Source})
set (SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

if(BUILD_MONOLITHIC)
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
else()
    add_library(${PROJECT_NAME} ${SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")
target_link_libraries(${PROJECT_NAME} PUBLIC raven-math raven-util)
