#pragma once
#include "Rendering/RenderGraph/IRenderGraphModule.h"
#include "Rendering/Game/Mesh.h"

namespace Raven
{
	/*!\struct FRGMCommonTargets
	\brief Resources returned by the setup function of the render graph module.
	*/
	struct FRGMForwardTargets
	{
		FVirtualResourceRef* pColor;
		FVirtualResourceRef* pDepth;
	};

	/*!\struct FForwardMeshUniforms
	\brief structure of uniforms we need to draw a mesh. 
	*/
	struct FForwardMeshUniforms {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec4 camPos;
	};

	/*!\class RGMMesh
	\brief Render graph module for drawing meshes.
	*/
	class RGMMesh : public IRenderGraphModule
	{
	public:
		/*! Initialization function for any global state (shaders, persistent targets) */
		virtual void Init(IRenderer* pRenderer);

		/*! Destroy function for any global state. */
		virtual void Destroy(IRenderer* pRenderer);

		/*! Load resources that need to change when input is different. */
		virtual void Load(IRenderer* pRenderer, ISwapChain* pSwapChain);

		/*! Unload resources. */
		virtual void Unload(IRenderer* pRenderer);

		/*! Acquire next swapchain image and import the target into the render graph system. */
		FRGMForwardTargets RGDrawMeshesForward(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* color, FVirtualResourceRef* depth, uint32_t numMeshes, RMesh** ppMeshes);

	protected:
		FForwardMeshUniforms mUniforms;
		ISampler*			 mSampler;
		IShader*			 mShader;
		IRootSignature*		 mRootSignature;
		IPipeline*			 mPipeline;
	};
}