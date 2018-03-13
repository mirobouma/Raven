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

target_link_libraries(${MODULE_NAME} PUBLIC RavenMath RavenRendering Vulkan::Vulkan)
if(UNIX AND NOT APPLE)
	find_package(X11_XCB REQUIRED)
	target_link_libraries(${MODULE_NAME} PUBLIC sdl2 X11::XCB)

endif()
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

#enable glsl to spirv compilation.
enable_glsl_to_spirv_compiling(${MODULE_NAME})
