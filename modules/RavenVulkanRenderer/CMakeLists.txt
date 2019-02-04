#module definition
set(MODULE_NAME "RavenVulkanRenderer")

set (SourceGroup_PublicIncludes
	"Public/VulkanRenderer/VulkanRenderer.h"
	"Public/VulkanRenderer/Vulkan.h"
	"Public/VulkanRenderer/VulkanFence.h"
	"Public/VulkanRenderer/VulkanSemaphore.h"
	"Public/VulkanRenderer/VulkanBuffer.h"
	"Public/VulkanRenderer/VulkanImage.h"
	"Public/VulkanRenderer/VulkanRenderer.h"
	"Public/VulkanRenderer/VulkanRenderPass.h"
	"Public/VulkanRenderer/VulkanRootSignature.h"
	"Public/VulkanRenderer/VulkanPipeline.h"
	"Public/VulkanRenderer/VulkanDescriptorPool.h"
	"Public/VulkanRenderer/VulkanCaches.h"
	"Public/VulkanRenderer/VulkanDynamicUniformAllocator.h"
	"Public/VulkanRenderer/VulkanCommandPool.h"
	"Public/VulkanRenderer/VulkanFrameBuffer.h"
	"Public/VulkanRenderer/VulkanCommandList.h"
	"Public/VulkanRenderer/VulkanSharedDefines.h"
	"Public/VulkanRenderer/VulkanShader.h"
	"Public/VulkanRenderer/VulkanSwapChain.h"
	"Public/VulkanRenderer/VulkanQueue.h"
	"Public/VulkanRenderer/VulkanDevice.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/VulkanRenderer.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})

target_link_libraries(${MODULE_NAME} PUBLIC RavenHAL RavenCore RavenRenderInterface Vulkan::Vulkan stb_image assimp vma)
if(UNIX AND NOT APPLE)
	find_package(X11_XCB REQUIRED)
	target_link_libraries(${MODULE_NAME} PUBLIC sdl2 X11::XCB)

endif()
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

#enable glsl to spirv compilation.
enable_glsl_to_spirv_compiling(${MODULE_NAME})
