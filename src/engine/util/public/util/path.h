#pragma once
#include <string>

namespace Raven
{
    class Path {
    public:
        Path(const std::string& string);
        operator const std::string& () { return str(); }
        operator const char* () { return c_str(); }

        const char* c_str();
        const std::string& str();


    private:
        std::string path;
    };
}