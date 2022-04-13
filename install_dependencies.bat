cmd /c "./vcpkg/bootstrap-vcpkg.bat"
call "./vcpkg/vcpkg.exe" install assimp
call "./vcpkg/vcpkg.exe" install glm
call "./vcpkg/vcpkg.exe" install sdl2
call "./vcpkg/vcpkg.exe" install llvm