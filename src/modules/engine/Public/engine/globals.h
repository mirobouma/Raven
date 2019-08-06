#pragma once
#include "engine.h"
#include "hal/platform.h"

namespace Raven {
	extern REngine* GEngine; /*!< Global reference to engine instance. */
	extern RPlatform* GPlatform; /*!< Global reference to application instance. */
}