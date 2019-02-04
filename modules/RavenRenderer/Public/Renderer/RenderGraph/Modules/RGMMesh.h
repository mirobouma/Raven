#pragma once
#include "Renderer/RenderGraph/IRenderGraphModule.h"
#include "Renderer/Game/Mesh.h"
#include "Core/Math/Camera.h"

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

	struct FRGMDeferredTargets
	{
		FVirtualResourceRef* pAlbedo;
		FVirtualResourceRef* pPosition;
		FVirtualResourceRef* pNormal;
		FVirtualResourceRef* pDepth;
	};

	struct FRGMDeferredComposeTargets
	{
		FVirtualResourceRef* pOutput;
		FVirtualResourceRef* pAlbedo;
		FVirtualResourceRef* pPosition;
		FVirtualResourceRef* pSSAO;
		FVirtualResourceRef* pNormal;
		FVirtualResourceRef* pDepth;
		FVirtualResourceRef* pShadowMap;
		FVirtualResourceRef* pVolumetricFog;
	};
	
	/*!\struct FForwardMeshUniforms
	\brief structure of uniforms we need to draw a mesh. 
	*/
	struct FForwardMeshUniforms {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec4 camPos;
		glm::vec2 screenSize;
	};

	struct FDeferredUniforms 
	{
		glm::mat4 cModel;
		glm::mat4 cView;
		glm::mat4 cProj;

		float cFarZ;
		float cNearZ;
	};

	struct FDeferredComposeUniforms
	{
		glm::mat4 cView;
		glm::mat4 cProj;
		glm::mat4 cInvView;
		glm::mat4 cShadowMatrix;
		glm::vec4 cLightDir;

		float cFarZ;
		float cNearZ;
		glm::vec2 cScreenSize;
	};

	/*!\class RGMMesh
	\brief Render graph module for drawing meshes.
	*/
	class RGMMesh : public IRenderGraphModule
	{
	public:
		/*! Initialization function for any global state (shaders, persistent targets) */
		virtual void Init(IRenderInterface* pRenderer);

		/*! Destroy function for any global state. */
		virtual void Destroy(IRenderInterface* pRenderer);

		/*! Load resources that need to change when input is different. */
		virtual void Load(IRenderInterface* pRenderer, ISwapChain* pSwapChain);

		/*! Unload resources. */
		virtual void Unload(IRenderInterface* pRenderer);

		/*! Depth pre-pass. */
		FRGMForwardTargets RGDrawDepth(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* depth, RCamera* pCamera, uint32_t numMeshes, RMesh** ppMeshes);

		/*!  Draw meshes (forward) using pre-pass depth. */
		FRGMForwardTargets RGDrawMeshesForwardDepthPrepass(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* pColor, FVirtualResourceRef* pDepth, FVirtualResourceRef* pSsao, RCamera* pCamera, uint32_t numMeshes, RMesh** ppMeshes);

		/*! Generate G-Buffer. */
		FRGMDeferredTargets RGDrawMeshesDeferredGBuffer(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* pDepth, RCamera* pCamera, uint32_t numMeshes, RMesh** ppMeshes);

		/*! Acquire next swapchain image and import the target into the render graph system. */
		FRGMDeferredComposeTargets RGDrawMeshesDeferredCompose(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* pOutput, FVirtualResourceRef* pAlbedo, FVirtualResourceRef* pDepth, FVirtualResourceRef* pPosition, FVirtualResourceRef* pNormals, FVirtualResourceRef* pSsao, FVirtualResourceRef* pShadowMap, FVirtualResourceRef* pVolumetricFog, RCamera* pCamera, RCamera* pShadowCamera);


	protected:

		void CreateDepthResources(IRenderInterface* pRenderer);
		void CreateForwardResources(IRenderInterface* pRenderer);
		void CreateDeferredResources(IRenderInterface* pRenderer);

		FForwardMeshUniforms mUniforms;
		ISampler*			 mSampler;
		ISampler*			 mShadowSampler;
		IShader*			 mForwardShader;
		IRootSignature*		 mForwardRootSignature;
		IShader*			 mPrepassShader;
		IRootSignature*		 mPrepassRootSignature;
		IPipeline*			 mPipelineDepthPrepass;
		IPipeline*			 mPipelineForwardNoDepthWrite;
		FDepthStencilState	 mNoDepthWriteState;

		FRasterizerState     mRasterizerState;
		FDeferredUniforms	 mDeferredUniforms;
		IShader*			 mDeferredShader;
		IRootSignature*		 mDeferredRootSignature;
		IPipeline*			 mDeferredPipeline;

		IShader*			 mDeferredComposeShader;
		IRootSignature*		 mDeferredComposeRootSignature;
		IPipeline*			 mDeferredComposePipeline;

		IBuffer*			 mFSVertexBuffer;
		IBuffer*			 mFSIndexBuffer;
		IImage*			 mBlackTexture;
	};
}