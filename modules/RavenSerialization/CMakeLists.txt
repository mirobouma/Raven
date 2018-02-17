#module definition
set(MODULE_NAME "RavenSerialization")

set (SourceGroup_PublicIncludes

)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source

)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

#add_library(${MODULE_NAME} STATIC ${SOURCE})
#target_link_libraries(${MODULE_NAME} RavenCore)
#target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

