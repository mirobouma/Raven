#include "windows/os_windows.h"
#include "core/main/application.h"
#include "math/math.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

using namespace Raven;

int main(int argc, char** argv)
{
	OSWindows os;
	Application* application = new Application();

	Error result = application->setup(argc, argv);
	if (result != Error::SUCCESS)
		return os.get_exit_code(result);

	result = application->run();
	if (result != Error::SUCCESS)
		return os.get_exit_code(result);

	application->cleanup();

	delete application;
	return os.get_exit_code(result);
}