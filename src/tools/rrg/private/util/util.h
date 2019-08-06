#pragma once
#include "clang-c/Index.h"
#include <string>
#include "namespace.h"
#include "cursor_type.h"


namespace Util
{
	void ToString(const CXString &str, std::string &output);
	std::string GetQualifiedName(const RCursorType &type);

}