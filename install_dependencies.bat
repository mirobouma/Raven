cmd /c "./vcpkg/bootstrap-vcpkg.bat"
call "./vcpkg/vcpkg.exe" install assimp:x64-windows
call "./vcpkg/vcpkg.exe" install glm:x64-windows
call "./vcpkg/vcpkg.exe" install sdl2:x64-windows
call "./vcpkg/vcpkg.exe" install llvm:x64-windows