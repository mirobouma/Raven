#include "Core/Service/ServiceProvider.h"

namespace Raven {
	RServiceProvider* GServiceProvider = nullptr; /*!< Global service provider. Is assigned to nullptr until it has been properly initialized by the engine. */
}