if not exist "solution" mkdir solution
cd solution
cmake ..\ -G "Visual Studio 15 2017 Win64"
start Raven.sln