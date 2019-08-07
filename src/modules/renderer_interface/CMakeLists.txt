#module definition
set(MODULE_NAME "renderer_interface")

set (SourceGroup_PublicIncludes
	"public/renderer_interface/renderer_interface.h"
	"public/renderer_interface/buffer.h"
	"public/renderer_interface/swapchain.h"
	"public/renderer_interface/shader.h"
	"public/renderer_interface/command_list.h"
	"public/renderer_interface/command_pool.h"
	"public/renderer_interface/pipeline.h"
	"public/renderer_interface/queue.h"
	"public/renderer_interface/render_target.h"
	"public/renderer_interface/root_signature.h"
	"public/renderer_interface/sampler.h"
	"public/renderer_interface/image.h"
	"public/renderer_interface/fence.h"
	"public/renderer_interface/semaphore.h"
	"public/renderer_interface/shared_defines.h"
)

source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/renderer_interface.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC core platform assimp stb_image)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

