#include "util/path.h"
#include <algorithm>

namespace Raven
{
    Path::Path(const std::string& path)
    {
        this->path = path;
        std::replace(this->path.begin(), this->path.end(), '\\', '/');
    }

    const char* Path::c_str()
    {
        return path.c_str();
    }

    const std::string& Path::str()
    {
        return path;
    }
}
