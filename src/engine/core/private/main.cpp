#include "core/main/application.h"
#include "os/os.h"

using namespace Raven;

int main(int argc, char** argv)
{
	OS os;
	Application application;

	Result result = application.setup(argc, argv);
	if (result != Result::SUCCESS)
		return os.get_exit_code(result);

	result = application.run();
	if (result != Result::SUCCESS)
		return os.get_exit_code(result);

	application.cleanup();
	return os.get_exit_code(result);
}