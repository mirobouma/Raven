#include "Engine/Engine.h"
#include "Engine/Globals.h"

#include "HAL/Platform.h"
#include "WindowSystem/WindowSystem.h"

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
		RWindowSystem::Initialize();

		// Initialization succeeded.
		return true;
	}

	bool REngine::InitializeSystems()
	{
		// Create rendering system.
		_Renderer = std::unique_ptr<RVulkanTestRenderer>(new RVulkanTestRenderer());
		_Renderer->Initialize();

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

		// Destroy renderer.
		GEngine->_Renderer->Destroy();

		// Destroy windowing system interface.
		RWindowSystem::Destroy();

		// Initialize platform specific systems.
		Raven::RPlatform::Destroy();

		delete GEngine;
		GEngine = nullptr;

		// Destroyal succeeded.
		return true;
	}

	void REngine::Render()
	{
		if (GEngine == nullptr)
		{
			// The engine isn't initialized so we can't render.
			return;
		}

		// Render the game.
		_Renderer->Render();
		
	}
}