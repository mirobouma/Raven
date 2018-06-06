#pragma once
#include "Rendering/RenderGraph/RenderGraph.h"

namespace Raven
{
	/*!\class IRenderGraphModule
	\brief Interface for a render graph module.
	*/
	class IRenderGraphModule
	{
	public:
		/*! Initialization function for any global state (shaders, persistent targets) */
		virtual void Init(IRenderer* pRenderer) = 0;

		/*! Destroy function for any global state. */
		virtual void Destroy(IRenderer* pRenderer) = 0;

		/*! Load resources that need to change when input is different. */
		virtual void Load(IRenderer* pRenderer, ISwapChain* pSwapChain) = 0;

		/*! Unload resources. */
		virtual void Unload(IRenderer* pRenderer) = 0;
	};
}