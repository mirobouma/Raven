
add_subdirectory(Core)
add_subdirectory(IO)
add_subdirectory(Renderer)
add_subdirectory(UI)
add_subdirectory(RendererVulkan)

# Compile DX12 module if on windows.
if (WIN32)
    add_subdirectory(RendererDX12)
endif (WIN32)

