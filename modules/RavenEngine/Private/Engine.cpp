#include "Engine/Engine.h"
#include "Engine/Globals.h"

#include "HAL/Platform.h"
#include "WindowSystem/WindowSystem.h"
#include "Core/Debug/LogManager.h"
#include "RenderingVulkan/VulkanRenderer.h"

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

		// Initialize minimal functionality.
		GEngine->InitializeMinimal();

		// Initialize platform specific systems.
		RPlatform::Initialize();

		// Creation succeeded.
		return GEngine;
	}

	bool REngine::InitializeMinimal()
	{
		// Initialize logger.
		RLogManager::Initialize(LS_Debug);

		// Initialize critical systems.
		RWindowSystem::Initialize();

		// Initialization succeeded.
		return true;
	}

	bool REngine::InitializeSystems()
	{
		// Create rendering system.
		_Renderer = std::unique_ptr<IRenderer>(new RVulkanRenderer());

		FRendererInitializationDesc initDesc;
		initDesc.applicationName = "Application";
		initDesc.enableDebugging = true;
		initDesc.versionMajor = 1;
		initDesc.versionMinor = 0;
		initDesc.versionPatch = 0;

		_Renderer->initialize(&initDesc);

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
		GEngine->_Renderer->destroy();

		// Destroy windowing system interface.
		RWindowSystem::Destroy();

		// Initialize platform specific systems.
		RPlatform::Destroy();

		// Destroy logging manager.
		RLogManager::Destroy();

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
		//_Renderer->Render();
		
	}
}