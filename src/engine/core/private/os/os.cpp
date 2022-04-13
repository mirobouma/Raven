#include "core/os/os.h"

namespace Raven
{
    OS* OS::_singleton = nullptr;

    OS::OS()
    {
        set_singleton(this);
    }
}