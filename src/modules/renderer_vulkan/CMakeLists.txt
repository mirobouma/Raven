#module definition
set(MODULE_NAME "renderer_vulkan")

set (SourceGroup_PublicIncludes
	"public/renderer_vulkan/vulkan_renderer.h"
	"public/renderer_vulkan/vulkan.h"
	"public/renderer_vulkan/vulkan_fence.h"
	"public/renderer_vulkan/vulkan_semaphore.h"
	"public/renderer_vulkan/vulkan_buffer.h"
	"public/renderer_vulkan/vulkan_image.h"
	"public/renderer_vulkan/vulkan_render_pass.h"
	"public/renderer_vulkan/vulkan_root_signature.h"
	"public/renderer_vulkan/vulkan_pipeline.h"
	"public/renderer_vulkan/vulkan_descriptor_pool.h"
	"public/renderer_vulkan/vulkan_caches.h"
	"public/renderer_vulkan/vulkan_dynamic_uniform_allocator.h"
	"public/renderer_vulkan/vulkan_command_pool.h"
	"public/renderer_vulkan/vulkan_frame_buffer.h"
	"public/renderer_vulkan/vulkan_command_list.h"
	"public/renderer_vulkan/vulkan_shared_defines.h"
	"public/renderer_vulkan/vulkan_shader.h"
	"public/renderer_vulkan/vulkan_swapchain.h"
	"public/renderer_vulkan/vulkan_queue.h"
	"public/renderer_vulkan/vulkan_device.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/renderer_vulkan.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})

target_link_libraries(${MODULE_NAME} PUBLIC hal core renderer_interface Vulkan::Vulkan stb_image assimp vma)
if(UNIX AND NOT APPLE)
	find_package(X11_XCB REQUIRED)
	target_link_libraries(${MODULE_NAME} PUBLIC sdl2 X11::XCB)

endif()
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

#enable glsl to spirv compilation.
enable_glsl_to_spirv_compiling(${MODULE_NAME})
