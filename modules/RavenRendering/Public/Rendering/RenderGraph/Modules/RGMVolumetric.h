#pragma once
#include "Rendering/RenderGraph/IRenderGraphModule.h"
#include "Rendering/Game/Mesh.h"
#include "Math/Camera.h"

namespace Raven
{
	/*!\struct FRGMSSAOTargets
	\brief Resources returned by the setup function of the render graph module.
	*/
	struct FRGMVolumetricFogTargets
	{
		FVirtualResourceRef* pOutput;
		FVirtualResourceRef* pSceneDepth;
		FVirtualResourceRef* pShadowDepth;
		FVirtualResourceRef* pScenePosition;
	};

	struct FVolumetricFogConstants
	{
		glm::mat4		cView;
		glm::mat4		cInvView;
		glm::mat4		cShadowMat;
		glm::vec4		cLightDir;
		glm::vec4		cLightCol;
		glm::vec4		cLightMieG;
		glm::vec2		cScreenSize;
		float			cNearZ;
		float			cFarZ;
		float			cAmbientFog;
		float			cLightIntensity;
		float			cLightScatteringCoef;
		float			cLightExtinctionCoef;
		float			cSkyboxExtinctionCoef;
		uint32_t		cNumSteps;
		float			cMaxRayLength;
		uint32_t		cEnableHeightFog;
		float			cHeightFog;
		float			cHeightFogScale;
		float			cNoiseSpeed;
		float			cFogDensity;
		bool			cEnableNoise;

	};

	struct FVolumetricFogVars
	{
		glm::vec4		LightCol;
		float			LightMieG;
		float			AmbientFog;
		float			LightIntensity;
		float			LightScatteringCoef;
		float			LightExtinctionCoef;
		float			SkyboxExtinctionCoef;
		uint32_t		NumSteps;
		float			MaxRayLength;
		uint32_t		EnableHeightFog;
		float			HeightFog;
		float			HeightFogScale;
		float			NoiseSpeed;
		float			FogDensity;
		bool			EnableNoise;
	};

	/*!\class RGMVolumetricFog
	\brief Render graph module for computing volumetric fog.
	*/
	class RGMVolumetricFog : public IRenderGraphModule
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

		/*! Draw volumetric fog based on shadow map. */
		FRGMVolumetricFogTargets RGDrawVolumetricFog(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* scenePosition, FVirtualResourceRef* sceneDepth, FVirtualResourceRef* shadowDepth, RCamera* camera, RCamera* shadowCamera, FVolumetricFogVars& vars);

	protected:
		FVolumetricFogConstants		 mConstants;
		ISampler*					 mShadowSampler;
		ISampler*					 mDepthSampler;
		IShader*					 mVolumetricPassShader;
		IRootSignature*				 mVolumetricRootSignature;
		IPipeline*					 mVolumetricPipeline;
									 
		IBuffer*					 mFSVertexBuffer;
		IBuffer*					 mFSIndexBuffer;
	};
}