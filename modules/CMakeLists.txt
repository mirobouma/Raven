add_subdirectory(Platform)

#Platform specific implementations.
if (WIN32)
	add_subdirectory(PlatformWindows)
endif (WIN32)

add_subdirectory(Core)
add_subdirectory(IO)
add_subdirectory(Rendering)
add_subdirectory(RenderingVulkan)
add_subdirectory(UI)

# Compile DX12 module if on windows.
if (WIN32)
    add_subdirectory(RenderingDX12)
endif (WIN32)

