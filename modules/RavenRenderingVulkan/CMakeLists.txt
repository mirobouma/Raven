#module definition
set(MODULE_NAME "RavenRenderingVulkan")

set (SourceGroup_PublicIncludes
	"Public/RenderingVulkan/TestVulkanRenderer.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/TestVulkanRenderer.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenRendering Vulkan::Vulkan)
if(UNIX AND NOT APPLE)
target_link_libraries(${MODULE_NAME} PUBLIC sdl2)
endif()
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

