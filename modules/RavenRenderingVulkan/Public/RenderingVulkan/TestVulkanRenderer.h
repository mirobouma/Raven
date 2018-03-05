#pragma once
#include "vulkan/vulkan.h"

namespace Raven {
	// Test class for vulkan rendering until proper interface is set up
	class RVulkanTestRenderer {
	public:
		RVulkanTestRenderer() = default;
		~RVulkanTestRenderer() = default;

		void Initialize();
		void Render();
		void Destroy();
	};
}