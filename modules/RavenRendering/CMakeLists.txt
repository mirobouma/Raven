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
	"Public/Rendering/Interfaces/SharedDefines.h"
)

source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/Interfaces/IRenderer.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenCore RavenPlatform)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

