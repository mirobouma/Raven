#include "Engine/Engine.h"
#include "Engine/Globals.h"
#include "HAL/Platform.h"

namespace Raven {

	REngine* REngine::Create()
	{
		if (GEngine != nullptr)
		{
			// The engine is already initialized, return false.
			return GEngine;
		}

		// Create engine instance.
		GEngine = new REngine();

		// Initialize platform specific systems.
		Raven::RPlatform::Initialize();

		// Creation succeeded.
		return GEngine;
	}

	bool REngine::InitializeMinimal()
	{
		// Initialize critical systems.


		// Initialization succeeded.
		return true;
	}

	bool REngine::InitializeSystems()
	{
	
		// Initialization succeeded.
		return true;
	}

	bool REngine::Destroy()
	{
		if (GEngine == nullptr)
		{
			// We are trying to destroy an engine that does not exist, return false.
			return false;
		}

		// Initialize platform specific systems.
		Raven::RPlatform::Destroy();


		delete GEngine;
		GEngine = nullptr;

		// Destroyal succeeded.
		return true;
	}
}