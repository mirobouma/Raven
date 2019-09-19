#include "core/os/os.h"

OS* OS::_singleton = nullptr;

OS::OS()
{
    set_singleton(this);
}