#module definition
set(MODULE_NAME "RavenRenderer")

set (SourceGroup_PublicIncludes
	"Public/Renderer/Game/RenderTarget.h"
	"Public/Renderer/Game/Texture.h"
	"Public/Renderer/Game/Buffer.h"
	"Public/Renderer/Game/Mesh.h"
	"Public/Renderer/Game/Material.h"
	"Public/Renderer/RenderGraph/RenderGraph.h"
	"Public/Renderer/RenderGraph/IRenderGraphModule.h"
	"Public/Renderer/RenderGraph/Modules/RGMMesh.h"
	"Public/Renderer/RenderGraph/Modules/RGMCommon.h"
	"Public/Renderer/RenderGraph/Modules/RGMSSAO.h"
	"Public/Renderer/RenderGraph/Modules/RGMVolumetric.h"
)

source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/Game/Mesh.cpp"
	"Private/RenderGraph/RenderGraph.cpp"
	"Private/RenderGraph/Modules/RGMCommon.cpp"
	"Private/RenderGraph/Modules/RGMMesh.cpp"
	"Private/RenderGraph/Modules/RGMSSAO.cpp"
	"Private/RenderGraph/Modules/RGMVolumetric.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenCore RavenRenderInterface)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")