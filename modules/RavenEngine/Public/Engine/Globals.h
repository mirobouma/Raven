#pragma once
#include "Engine.h"
#include "Application.h"

namespace Raven {
	extern REngine* GEngine; /*!< Global reference to engine instance. */
	extern IApplication* GApplication; /*!< Global reference to application instance. */
	extern IPlatform* GPlatform /*!< Global reference to platform instance. */
}