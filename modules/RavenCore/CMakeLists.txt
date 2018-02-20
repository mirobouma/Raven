#module definition
set(MODULE_NAME "RavenCore")

set (SourceGroup_PublicIncludes
	"Public/Core/Service/Service.h"
	"Public/Core/Service/ServiceProvider.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/Core.cpp"
	"Private/Utility/Assert.cpp"
	"Private/Service/ServiceProvider.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenMath RavenHAL)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

