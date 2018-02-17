#include "Core\Utility\Assert.h"
#include <cstdio>
#include <string>

namespace Raven {
	void RAssert::HandleAssert(const char* file, int line, const char* expression)
	{
		printf_s("Assertion failed:\nFile: %s\nLine Number:%i\nExpression:%s", file, line, expression);
	}
}