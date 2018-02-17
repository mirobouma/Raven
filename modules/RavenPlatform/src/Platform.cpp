#include "Platform/Platform.h"
#include "Core/Utility/Assert.h"

namespace Raven {
	bool IPlatform::Initialize() {

		// Initialize interface with new due to protected constructor (only IPlatform has access)
		RWindowSystemInterface* winSysInterface = new RWindowSystemInterface();
		_WindowSystemInterface = std::unique_ptr<RWindowSystemInterface>(winSysInterface);

		// Return successful initialization if the window system interface initialized correctly.
		return _WindowSystemInterface->Initialize();
	}
}

