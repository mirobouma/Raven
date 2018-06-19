#pragma once
#include "Rendering/RenderGraph/IRenderGraphModule.h"
#include "Rendering/Game/Mesh.h"
#include "Math/Camera.h"

namespace Raven
{
	/*!\struct FRGMSSAOTargets
	\brief Resources returned by the setup function of the render graph module.
	*/
	struct FRGMSSAOTargets
	{
		FVirtualResourceRef* pSSAO;
		FVirtualResourceRef* pDepth;
	};
	
	struct FRGMDeferredSSAOTargets
	{
		FVirtualResourceRef* pSSAO;
		FVirtualResourceRef* pDepth;
		FVirtualResourceRef* pNormal;
		FVirtualResourceRef* pPosition;
	};

	struct FSSAOConstants
	{
		glm::mat4	cProjection;
		glm::mat4	cInvProjection;
		glm::vec4	cFrustumCorners[4];
		glm::vec4	cKernels[32];
		glm::vec2	cScreenSize;
		float		cNearZ;
		float		cFarZ;
		float		cRandomSize;
		float		cSampleRadius;
		float		cIntensity;
		float		cScale;
		float		cBias;
		glm::vec3	cPad0;
		
	};

	struct FSSAOVars
	{
		float		cRandomSize;
		float		cSampleRadius;
		float		cIntensity;
		float		cScale;
		float		cBias;
	};

	/*!\class RGMSSAO
	\brief Render graph module for computing SSAO
	*/
	class RGMSSAO : public IRenderGraphModule
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

		/*! Draw SSAO based on depth. */
		FRGMSSAOTargets RGDrawSSAO_Depth(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* depth, RCamera* camera, FSSAOVars& vars);

		/*! Draw SSAO based on normal buffer. */
		FRGMDeferredSSAOTargets RGDrawSSAO_Deferred(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* depth, FVirtualResourceRef* normal, FVirtualResourceRef* position, RCamera* camera, FSSAOVars& vars);

	protected:
		FSSAOConstants		 mConstants;
		ISampler*			 mSampler;
		IShader*			 mShader;
		IRootSignature*		 mRootSignature;
		IPipeline*			 mPipeline;

		IShader*			 mDeferredShader;
		IRootSignature*		 mDeferredRootSignature;
		IPipeline*			 mDeferredPipeline;

		IBuffer*			 mFSVertexBuffer;
		IBuffer*			 mFSIndexBuffer;
		ITexture*			 mRandomTexture;
		FRasterizerState	 mRaserizerState;
	};
}