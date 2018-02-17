#START-FILE-LIST
set (SourceGroup_Main
	"src/Main.cpp"
)
source_group("Main" FILES ${SourceGroup_Main})

set (SOURCE
	${SourceGroup_Main}
)

add_executable(RavenGame WIN32 ${SOURCE} ${RAVEN_ICON_RESOURCE_PATH})
target_include_directories(RavenGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(RavenGame RavenPlatformWin32)
