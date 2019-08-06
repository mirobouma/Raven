#module definition
set(MODULE_NAME "rendering")

set (SourceGroup_PublicIncludes
	"public/rendering/game/render_target.h"
	"public/rendering/game/texture.h"
	"public/rendering/game/buffer.h"
	"public/rendering/game/mesh.h"
	"public/rendering/game/material.h"
	"public/rendering/render_graph/render_graph.h"
	"public/rendering/render_graph/render_graph_module.h"
	"public/rendering/render_graph/modules/rgm_mesh.h"
	"public/rendering/render_graph/modules/rgm_common.h"
	"public/rendering/render_graph/modules/rgm_ssao.h"
	"public/rendering/render_graph/modules/rgm_volumetric.h"
)

source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/game/mesh.cpp"
	"private/render_graph/render_graph.cpp"
	"private/render_graph/modules/rgm_common.cpp"
	"private/render_graph/modules/rgm_mesh.cpp"
	"private/render_graph/modules/rgm_ssao.cpp"
	"private/render_graph/modules/rgm_volumetric.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC core renderer_interface)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")