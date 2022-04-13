#pragma once
#include "core/string.h"

namespace Raven
{
    class Path {
    public:
        Path(const String& string);
        operator const String& () { return str(); }
        operator const char* () { return c_str(); }

        const char* c_str();
        const String& str();


    private:
        String path;
    };
}