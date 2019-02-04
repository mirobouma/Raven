#pragma once
#include "clang-c/Index.h"
#include <string>
#include "Namespace.h"
#include "CursorType.h"


namespace Util
{
	void ToString(const CXString &str, std::string &output);
	std::string GetQualifiedName(const RCursorType &type);

}