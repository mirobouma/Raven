#pragma once
#include "core/os/dynamic_library.h"
#include "core/main/main_loop.h"

#include <cinttypes>

#define RAVEN_APPLICATION(T) \
MainLoop* raven_create_main_loop() { return new T(); }

