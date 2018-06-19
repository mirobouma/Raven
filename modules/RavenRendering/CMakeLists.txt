#module definition
set(MODULE_NAME "RavenRendering")

set (SourceGroup_PublicIncludes
	"Public/Rendering/Interfaces/IRenderer.h"
	"Public/Rendering/Interfaces/IBuffer.h"
	"Public/Rendering/Interfaces/ISwapChain.h"
	"Public/Rendering/Interfaces/IShader.h"
	"Public/Rendering/Interfaces/ICommandList.h"
	"Public/Rendering/Interfaces/ICommandPool.h"
	"Public/Rendering/Interfaces/IPipeline.h"
	"Public/Rendering/Interfaces/IQueue.h"
	"Public/Rendering/Interfaces/IRenderTarget.h"
	"Public/Rendering/Interfaces/IRootSignature.h"
	"Public/Rendering/Interfaces/ISampler.h"
	"Public/Rendering/Interfaces/ITexture.h"
	"Public/Rendering/Interfaces/IFence.h"
	"Public/Rendering/Interfaces/ISemaphore.h"
	"Public/Rendering/Interfaces/SharedDefines.h"
	"Public/Rendering/Game/RenderTarget.h"
	"Public/Rendering/Game/Texture.h"
	"Public/Rendering/Game/Buffer.h"
	"Public/Rendering/Game/Mesh.h"
	"Public/Rendering/Game/Material.h"
	"Public/Rendering/RenderGraph/RenderGraph.h"
	"Public/Rendering/RenderGraph/IRenderGraphModule.h"
	"Public/Rendering/RenderGraph/Modules/RGMMesh.h"
	"Public/Rendering/RenderGraph/Modules/RGMCommon.h"
	"Public/Rendering/RenderGraph/Modules/RGMSSAO.h"
	"Public/Rendering/RenderGraph/Modules/RGMVolumetric.h"
)

source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/Interfaces/IRenderer.cpp"
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
target_link_libraries(${MODULE_NAME} PUBLIC RavenCore RavenPlatform assimp stb_image RavenMath)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

