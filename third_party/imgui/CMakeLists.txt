#module definition
set(MODULE_NAME "imgui")

set (SourceGroup_PublicIncludes
	"imgui/imgui.h"
	"imgui/imconfig.h"
	"imgui/imgui_internal.h"
	"imgui/imstb_rectpack.h"
	"imgui/imstb_textedit.h"
	"imgui/imstb_truetype.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"imgui/imgui.cpp"
	"imgui/imgui_demo.cpp"
	"imgui/imgui_widgets.cpp"
	"imgui/imgui_draw.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")



