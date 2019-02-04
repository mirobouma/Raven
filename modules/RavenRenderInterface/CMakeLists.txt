#module definition
set(MODULE_NAME "RavenRenderInterface")

set (SourceGroup_PublicIncludes
	"Public/RenderInterface/IRenderInterface.h"
	"Public/RenderInterface/IBuffer.h"
	"Public/RenderInterface/ISwapChain.h"
	"Public/RenderInterface/IShader.h"
	"Public/RenderInterface/ICommandList.h"
	"Public/RenderInterface/ICommandPool.h"
	"Public/RenderInterface/IPipeline.h"
	"Public/RenderInterface/IQueue.h"
	"Public/RenderInterface/IRenderTarget.h"
	"Public/RenderInterface/IRootSignature.h"
	"Public/RenderInterface/ISampler.h"
	"Public/RenderInterface/IImage.h"
	"Public/RenderInterface/IFence.h"
	"Public/RenderInterface/ISemaphore.h"
	"Public/RenderInterface/SharedDefines.h"
)

source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/IRenderInterface.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenCore RavenPlatform assimp stb_image)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

