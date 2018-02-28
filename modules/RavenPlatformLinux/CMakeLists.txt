#module definition
set(MODULE_NAME "RavenPlatformLinux")

set (SourceGroup_PublicIncludes
	"Public/PlatformLinux/PlatformLinux.h"
	"Public/PlatformLinux/PlatformLinuxInfo.h"
	"Public/PlatformLinux/PlatformLinuxWindow.h"
	"Public/PlatformLinux/PlatformLinuxApplication.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/PlatformLinux.cpp"
	"Private/PlatformLinuxInfo.cpp"
	"Private/PlatformLinuxWindow.cpp"
	"Private/PlatformLinuxApplication.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

if (NOT(TARGET sdl2))
	message("SDL2 doesnt exist")
endif()

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenPlatform sdl2)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")



