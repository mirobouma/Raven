if not exist "solution" mkdir solution
cd solution
cmake ..\ -G "Visual Studio 16 2019" -A x64
start Raven.sln