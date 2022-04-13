#pragma once
#include "core/string.h"

namespace Raven
{
	String utf8_encode(const WString& wstr);
	WString utf8_decode(const String& str);
}
