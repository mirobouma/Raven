#include "core/io/path.h"
#include <algorithm>

Path::Path(const String& path)
{
    this->path = path;
    std::replace(this->path.begin(), this->path.end(), '\\', '/');
}

const char* Path::c_str()
{
    return path.c_str();
}

const String& Path::str()
{
    return path;
}
