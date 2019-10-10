set(PROJECT_NAME raven-renderer-vulkan)

set (SourceGroup_PublicIncludes
    "public/renderer-vulkan/vulkan.h"
    "public/renderer-vulkan/vulkan_buffer.h"
    "public/renderer-vulkan/vulkan_caches.h"
    "public/renderer-vulkan/vulkan_command_list.h"
    "public/renderer-vulkan/vulkan_command_pool.h"
    "public/renderer-vulkan/vulkan_defines.h"
    "public/renderer-vulkan/vulkan_descriptor_pool.h"
    "public/renderer-vulkan/vulkan_dynamic_uniform_allocator.h"
    "public/renderer-vulkan/vulkan_fence.h"
    "public/renderer-vulkan/vulkan_frame_buffer.h"
    "public/renderer-vulkan/vulkan_image.h"
    "public/renderer-vulkan/vulkan_pipeline.h"
    "public/renderer-vulkan/vulkan_queue.h"
    "public/renderer-vulkan/vulkan_render_pass.h"
    "public/renderer-vulkan/vulkan_render_target.h"
    "public/renderer-vulkan/vulkan_renderer.h"
    "public/renderer-vulkan/vulkan_root_signature.h"
    "public/renderer-vulkan/vulkan_sampler.h"
    "public/renderer-vulkan/vulkan_semaphore.h"
    "public/renderer-vulkan/vulkan_shader.h"
    "public/renderer-vulkan/vulkan_swap_chain.h"
)

source_group("Public" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/vulkan_renderer.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})
set (SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
    )

target_link_libraries(${PROJECT_NAME} PUBLIC raven-core raven-util raven-math  Vulkan::Vulkan vma)
