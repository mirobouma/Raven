#pragma once
#include "rendering/render_graph/render_graph.h"

namespace Raven
{
	/*!\class IRenderGraphModule
	\brief Interface for a render graph module.
	*/
	class IRenderGraphModule
	{
	public:
		/*! Initialization function for any global state (shaders, persistent targets) */
		virtual void Init(IRenderInterface* pRenderer) = 0;

		/*! Destroy function for any global state. */
		virtual void Destroy(IRenderInterface* pRenderer) = 0;

		/*! Load resources that need to change when input is different. */
		virtual void Load(IRenderInterface* pRenderer, ISwapChain* pSwapChain) = 0;

		/*! Unload resources. */
		virtual void Unload(IRenderInterface* pRenderer) = 0;
	};
}