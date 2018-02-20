#module definition.
set(MODULE_NAME "RavenHAL")

set (SourceGroup_PublicIncludes
	"Public/HAL/PlatformInfo.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/HAL.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

#compile as static library even though it is an interface to make dependencies easier to handle.
add_library(${MODULE_NAME} STATIC ${SOURCE})

#platform specific modules.
if (WIN32)
	target_link_libraries(${MODULE_NAME} PUBLIC RavenPlatformWindows)
endif()

target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")



