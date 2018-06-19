#include "Rendering/RenderGraph/Modules/RGMVolumetric.h"
#include <chrono>
#include "stb_image.h"
#include "Core/Debug/LogManager.h"
#include <random>

namespace Raven
{
	void RGMVolumetricFog::Init(IRenderer* pRenderer)
	{
		FSamplerCreateDesc samplerDesc;
		samplerDesc.addressModeU = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerDesc.addressModeV = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerDesc.addressModeW = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerDesc.anisotropyEnable = false;
		samplerDesc.comparisonFunc = ESamplerComparisonFunc::SAMPLER_COMPARISON_LESS;
		samplerDesc.borderColor = EBorderColor::BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		samplerDesc.maxAnisotropy = 1;
		samplerDesc.magFilter = ESamplerFilter::SAMPLER_FILTER_LINEAR;
		samplerDesc.minFilter = ESamplerFilter::SAMPLER_FILTER_LINEAR;
		samplerDesc.maxLod = 0.0f;
		samplerDesc.minLod = 0.0f;
		samplerDesc.mipLodBias = 0.0f;
		samplerDesc.mipmapMode = EMipMode::MIP_MODE_NEAREST;

		pRenderer->createSampler(&mShadowSampler, &samplerDesc);

		samplerDesc.comparisonFunc = ESamplerComparisonFunc::SAMPLER_COMPARISON_NONE;
		pRenderer->createSampler(&mDepthSampler, &samplerDesc);

		// Create FS quad.
		glm::vec2 FSQuadVertices[] = {
			glm::vec2(-1.0f, -1.0f), glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f),
			glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
		};

		FBufferCreateDesc vbDesc;
		vbDesc.usage = BUFFER_USAGE_VERTEX_BUFFER | BUFFER_USAGE_TRANSFER_DST;
		vbDesc.memoryType = MEMORY_TYPE_GPU;
		vbDesc.pData = &FSQuadVertices;
		vbDesc.dataSize = sizeof(glm::vec2) * 8;
		vbDesc.size = vbDesc.dataSize;

		pRenderer->createBuffer(&mFSVertexBuffer, &vbDesc);

		uint32_t FSQuadIndices[] = {
			0, 1, 2, 0, 2, 3
		};

		FBufferCreateDesc ibDesc;
		ibDesc.usage = BUFFER_USAGE_INDEX_BUFFER | BUFFER_USAGE_TRANSFER_DST;
		ibDesc.memoryType = MEMORY_TYPE_GPU;
		ibDesc.pData = &FSQuadIndices;
		ibDesc.dataSize = sizeof(glm::uint32_t) * 6;
		ibDesc.size = ibDesc.dataSize;

		pRenderer->createBuffer(&mFSIndexBuffer, &ibDesc);

		FShaderCreateDesc shaderDesc = {};
		shaderDesc.shaders[0].path = "shaders/volumetric_fog_directional.vert.spv";
		shaderDesc.shaders[0].stage = EShaderStage::SHADER_STAGE_VERTEX;
		shaderDesc.shaders[1].path = "shaders/volumetric_fog_directional.frag.spv";
		shaderDesc.shaders[1].stage = EShaderStage::SHADER_STAGE_FRAGMENT;

		pRenderer->createShader(&mVolumetricPassShader, &shaderDesc);

		// Create input assembler layout
		FInputAttributeDescription iaAttributes[2];
		iaAttributes[0].binding = 0;
		iaAttributes[0].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[0].location = 0;
		iaAttributes[0].offset = 0;

		iaAttributes[1].binding = 0;
		iaAttributes[1].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[1].location = 1;
		iaAttributes[1].offset = sizeof(glm::vec2);

		// Create input assembler binding
		FInputBindingDescription iaBinding;
		iaBinding.binding = 0;
		iaBinding.inputRate = EVertexInputRate::VERTEX_INPUT_RATE_VERTEX;
		iaBinding.stride = sizeof(glm::vec2) * 2;

		// Create input assembler layout
		FInputAssemblerLayout iaLayout;
		iaLayout.numInputBindingDescriptions = 1;
		iaLayout.pInputBindingDescriptions = &iaBinding;
		iaLayout.numInputAttributeDescriptions = 2;
		iaLayout.pInputAttributeDescriptions = &iaAttributes[0];

		// Create descriptor bindings.
		FDescriptorLayoutBinding bindings[6];
		bindings[0].set = 0;
		bindings[0].binding = 0;
		bindings[0].descriptorCount = 1;
		bindings[0].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		bindings[0].stageFlags = SHADER_STAGE_FLAG_VERTEX | SHADER_STAGE_FLAG_FRAGMENT;

		bindings[1].set = 1;
		bindings[1].binding = 0;
		bindings[1].descriptorCount = 1;
		bindings[1].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings[1].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[2].set = 1;
		bindings[2].binding = 1;
		bindings[2].descriptorCount = 1;
		bindings[2].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings[2].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[3].set = 1;
		bindings[3].binding = 2;
		bindings[3].descriptorCount = 1;
		bindings[3].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings[3].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[4].set = 1;
		bindings[4].binding = 3;
		bindings[4].descriptorCount = 1;
		bindings[4].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings[4].ppImmutableSamplers = &mShadowSampler;
		bindings[4].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[5].set = 1;
		bindings[5].binding = 4;
		bindings[5].descriptorCount = 1;
		bindings[5].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings[5].ppImmutableSamplers = &mDepthSampler;
		bindings[5].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		FRootSignatureCreateDesc rsDesc;
		rsDesc.pShaderProgram = mVolumetricPassShader;
		rsDesc.pInputAssemblerLayout = &iaLayout;
		rsDesc.numDescriptorBindings = 6;
		rsDesc.pDescriptorBindings = &bindings[0];

		pRenderer->createRootSignature(&mVolumetricRootSignature, &rsDesc);

	}

	void RGMVolumetricFog::Destroy(IRenderer* pRenderer)
	{
		pRenderer->destroyShader(mVolumetricPassShader);
		pRenderer->destroySampler(mShadowSampler);
		pRenderer->destroySampler(mDepthSampler);
		pRenderer->destroyRootSignature(mVolumetricRootSignature);
		pRenderer->destroyBuffer(mFSVertexBuffer);
		pRenderer->destroyBuffer(mFSIndexBuffer);
	}

	void RGMVolumetricFog::Load(IRenderer* pRenderer, ISwapChain* pSwapChain)
	{
		// Create graphics pipeline.
		FGraphicsPipelineCreateDesc pipelineDesc = {};
		pipelineDesc.pRootSignature = mVolumetricRootSignature;
		pipelineDesc.pShader = mVolumetricPassShader;
		pipelineDesc.pBlendState = nullptr;
		pipelineDesc.pDepthStencilState = nullptr;
		pipelineDesc.pRasterizerState = nullptr;
		pipelineDesc.depthStencilFormat = TEXTURE_FORMAT_UNKNOWN;
		pipelineDesc.numRenderTargets = 1;
		pipelineDesc.renderTargetFormats[0] = TEXTURE_FORMAT_RGBA8;
		pipelineDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE;
		pipelineDesc.sampleCount =  1;
		pipelineDesc.sampleQuality = 1;

		pRenderer->createPipeline(&mVolumetricPipeline, &pipelineDesc);
	}

	void RGMVolumetricFog::Unload(IRenderer* pRenderer)
	{
		pRenderer->destroyPipeline(mVolumetricPipeline);
	}

	FRGMVolumetricFogTargets RGMVolumetricFog::RGDrawVolumetricFog(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* scenePosition, FVirtualResourceRef* sceneDepth, FVirtualResourceRef* shadowDepth, RCamera* camera, RCamera* shadowCamera, FVolumetricFogVars& vars)
	{
		FRGMVolumetricFogTargets output;

		// Add the pass and setup its inputs and outputs.
		auto pass1 = pBuilder->add_pass("create_volumetric_fog_target", [&](RRenderPassBuilder& builder)
		{
			FVirtualTextureDesc backbufferInfo = sceneDepth->mResource->mDesc.GetTextureDesc();

			// create resource.
			FVirtualResourceDesc desc;
			desc.mType = EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_TEXTURE;
			desc.mTextureDesc.width = backbufferInfo.width;
			desc.mTextureDesc.height = backbufferInfo.height;
			desc.mTextureDesc.depth = 1;
			desc.mTextureDesc.format = ETextureFormat::TEXTURE_FORMAT_RGBA8;
			desc.mTextureDesc.sampleCount = 1;
			desc.mTextureDesc.sampleQuality = 1;

			// import resource.
			output.pOutput = builder.create_resource("volumetric_fog_output", desc);
		});

		// Add the pass and setup its inputs and outputs.
		auto pass2 = pBuilder->add_pass("draw_volumetric_fog", [&](RRenderPassBuilder& builder)
		{
			if (output.pOutput != nullptr)
			{
				output.pOutput = builder.write_resource("output", output.pOutput, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT);
			}
		
			if (scenePosition != nullptr)
			{
				output.pScenePosition = builder.read_resource("position", scenePosition, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			}
		
			if (sceneDepth != nullptr)
			{
				output.pSceneDepth = builder.read_resource("sceneDepth", sceneDepth, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			}
		
			if (shadowDepth != nullptr)
			{
				output.pShadowDepth = builder.read_resource("shadowDepth", shadowDepth, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			}
		});
		
		// Set execute function.
		pass2->setExecuteFunc([=](IRenderer* renderer, RRenderGraph& graph, RCompiledPass& pass, ICommandList* cmd)
		{
			FLoadStoreOps loadStoreOps = {};
			loadStoreOps.colorLoadOps[0] = LOAD_OP_CLEAR;
			loadStoreOps.colorStoreOps[0] = STORE_OP_STORE;
			loadStoreOps.colorClearValues[0][0] = 0.0f;
			loadStoreOps.colorClearValues[0][1] = 0.0f;
			loadStoreOps.colorClearValues[0][2] = 0.0f;
			loadStoreOps.colorClearValues[0][3] = 0.0f;
		
			loadStoreOps.depthLoadOp = LOAD_OP_DONTCARE;
			loadStoreOps.depthStoreOp = STORE_OP_DONTCARE;
			loadStoreOps.depthClearValue = 1.0f;
		
			IRenderTarget* output = pass.getRenderTargetResource("output");
			IRenderTarget* scenePosition = pass.getRenderTargetResource("position");
			IRenderTarget* sceneDepth = pass.getRenderTargetResource("sceneDepth");
			IRenderTarget* shadowDepth = pass.getRenderTargetResource("shadowDepth");
		
			// Begin rendering pass.
			renderer->cmdBeginRenderPass(cmd, 1, &output, nullptr, &loadStoreOps);
		
			// Bind graphics pipeline.
			renderer->cmdBindPipeline(cmd, mVolumetricPipeline);
			renderer->cmdSetViewport(cmd, FRect({ output->GetDesc()->width,  output->GetDesc()->height }, { 0, 0 }));
			renderer->cmdSetScissorRects(cmd, FRect({ output->GetDesc()->width,  output->GetDesc()->height }, { 0, 0 }));
		
			// Bind index/vertex buffer
			renderer->cmdBindIndexBuffer(cmd, mFSIndexBuffer);
			renderer->cmdBindVertexBuffer(cmd, 1, &mFSVertexBuffer);
		
			ITexture* sceneDepthTexture = sceneDepth->GetTexture();
			ITexture* positionTexture = scenePosition->GetTexture();
			ITexture* shadowDepthTexture = shadowDepth->GetTexture();
		
			FDescriptorBinding bindings[4];
		
			bindings[0].set = 1;
			bindings[0].binding = 0;
			bindings[0].numDescriptors = 1;
			bindings[0].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[0].ppTextures = &shadowDepthTexture;
			bindings[0].range = 0; // range 0 means full range.
			bindings[0].offset = 0;
		
			bindings[1].set = 1;
			bindings[1].binding = 1;
			bindings[1].numDescriptors = 1;
			bindings[1].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[1].ppTextures = &sceneDepthTexture;
			bindings[1].range = 0; // range 0 means full range.
			bindings[1].offset = 0;
		
			bindings[2].set = 1;
			bindings[2].binding = 2;
			bindings[2].numDescriptors = 1;
			bindings[2].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[2].ppTextures = &positionTexture;
			bindings[2].range = 0; // range 0 means full range.
			bindings[2].offset = 0;
		
			FDynamicUniformAllocation uniformAlloc;
			
			mConstants.cScreenSize = glm::vec2(output->GetDesc()->width, output->GetDesc()->height);
			mConstants.cView = camera->getTransform()->getMatrix();
			mConstants.cInvView = glm::inverse(mConstants.cView);
			mConstants.cShadowMat = shadowCamera->getProjectionMatrix() * shadowCamera->getTransform()->getMatrix();
			mConstants.cLightDir = glm::vec4(shadowCamera->getTransform()->GetForward(), 0.0f);
			mConstants.cLightCol = vars.LightCol;
			mConstants.cLightMieG = glm::vec4(1.0f - (vars.LightMieG * vars.LightMieG), 1.0f + (vars.LightMieG * vars.LightMieG), 2.0f * vars.LightMieG, 0.25 * glm::pi<float>());
			mConstants.cNearZ = camera->getNearZ();
			mConstants.cFarZ = camera->getFarZ();
			mConstants.cLightIntensity = vars.LightIntensity;
			mConstants.cLightScatteringCoef = vars.LightScatteringCoef;
			mConstants.cLightExtinctionCoef = vars.LightExtinctionCoef;
			mConstants.cSkyboxExtinctionCoef = vars.SkyboxExtinctionCoef;
			mConstants.cNumSteps = vars.NumSteps;
			mConstants.cMaxRayLength = vars.MaxRayLength;
			mConstants.cEnableHeightFog = vars.EnableHeightFog;
			mConstants.cHeightFog = vars.HeightFog;
			mConstants.cHeightFogScale = vars.HeightFogScale;
			mConstants.cNoiseSpeed = vars.NoiseSpeed;
			mConstants.cFogDensity = vars.FogDensity;
			mConstants.cAmbientFog = vars.AmbientFog;
			mConstants.cEnableNoise = vars.EnableNoise;

			renderer->cmdAllocateDynamicUniformBuffer(cmd, &uniformAlloc, sizeof(FVolumetricFogConstants), &mConstants);
		
			bindings[3].set = 0;
			bindings[3].binding = 0;
			bindings[3].numDescriptors = 1;
			bindings[3].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			bindings[3].ppBuffers = &uniformAlloc.buffer;
			bindings[3].range = uniformAlloc.range;
			bindings[3].offset = uniformAlloc.offset;
		
			// Bind resources.
			renderer->cmdBindDescriptors(cmd, 4, &bindings[0]);
		
			// Draw.
			renderer->cmdDrawIndexed(cmd, 6, 0, 0);
		
			// Finish rendering pass.
			renderer->cmdEndRenderPass(cmd);
		});

		return output;
	}

}
