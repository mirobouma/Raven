#include "Rendering/RenderGraph/Modules/RGMMesh.h"
#include <chrono>

namespace Raven
{
	void RGMMesh::Init(IRenderer* pRenderer)
	{
		FSamplerCreateDesc samplerDesc;
		samplerDesc.addressModeU = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_REPEAT;
		samplerDesc.addressModeV = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_REPEAT;
		samplerDesc.addressModeW = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_REPEAT;
		samplerDesc.anisotropyEnable = true;
		samplerDesc.comparisonFunc = ESamplerComparisonFunc::SAMPLER_COMPARISON_NONE;
		samplerDesc.anisotropyEnable = true;
		samplerDesc.borderColor = EBorderColor::BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		samplerDesc.maxAnisotropy = 16;
		samplerDesc.magFilter = ESamplerFilter::SAMPLER_FILTER_LINEAR;
		samplerDesc.minFilter = ESamplerFilter::SAMPLER_FILTER_LINEAR;
		samplerDesc.maxLod = 1000.0f;
		samplerDesc.minLod = 0.0f;
		samplerDesc.mipLodBias = 0.0f;
		samplerDesc.mipmapMode = EMipMode::MIP_MODE_LINEAR;

		pRenderer->createSampler(&mSampler, &samplerDesc);

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

		mNoDepthWriteState.depthTestEnable = true;
		mNoDepthWriteState.depthWriteEnable = false;
		mNoDepthWriteState.depthCompareOp = COMPARE_OP_EQUAL;
		mNoDepthWriteState.depthBoundsTestEnable = false;
		mNoDepthWriteState.minDepthBounds = 0.0f; // Optional
		mNoDepthWriteState.maxDepthBounds = 1.0f; // Optional
		mNoDepthWriteState.stencilTestEnable = false;
		mNoDepthWriteState.front = {}; // Optional
		mNoDepthWriteState.back = {}; // Optional

		mRasterizerState.cullMode = ECullMode::CULL_MODE_BACK;
		mRasterizerState.depthClampEnable = false;
		mRasterizerState.rasterizerDiscardEnable = false;
		mRasterizerState.polygonMode = POLYGON_MODE_FILL;
		mRasterizerState.lineWidth = 1.0f;
		mRasterizerState.frontFace = EFrontFaceWinding::FRONT_FACE_WINDING_CLOCKWISE;
		mRasterizerState.depthBiasEnable = true;
		mRasterizerState.depthBiasConstantFactor = 4.0f; // Optional
		mRasterizerState.depthBiasClamp = 0.0f; // Optional
		mRasterizerState.depthBiasSlopeFactor = 1.5; // Optional

		CreateDepthResources(pRenderer);
		CreateForwardResources(pRenderer);
		CreateDeferredResources(pRenderer);

	

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

		glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

		FTextureCreateDesc texDesc = {};
		texDesc.type = TEXTURE_TYPE_2D;
		texDesc.memoryType = MEMORY_TYPE_GPU;
		texDesc.arrayLayers = 1;
		texDesc.baseArrayLayer = 0;
		texDesc.baseMipLevel = 0;
		texDesc.depth = 1;
		texDesc.format = TEXTURE_FORMAT_RGBA32_FLOAT;
		texDesc.width = 1;
		texDesc.height = 1;
		texDesc.mipLevels = 0;
		texDesc.pData = &color;
		texDesc.dataSize = sizeof(glm::vec4);
		texDesc.sampleCount = 1;
		texDesc.sampleQuality = 0;
		texDesc.tiling = IMAGE_TILING_OPTIMAL;
		texDesc.usage = TEXTURE_USAGE_SAMPLED | TEXTURE_USAGE_TRANSFER_DST;

		pRenderer->createTexture(&mBlackTexture, &texDesc);

	}

	void RGMMesh::Destroy(IRenderer* pRenderer)
	{
		pRenderer->destroyShader(mForwardShader);
		pRenderer->destroyShader(mPrepassShader);
		pRenderer->destroyShader(mDeferredShader);
		pRenderer->destroyShader(mDeferredComposeShader);

		pRenderer->destroySampler(mSampler);
		pRenderer->destroySampler(mShadowSampler);

		pRenderer->destroyRootSignature(mForwardRootSignature);
		pRenderer->destroyRootSignature(mPrepassRootSignature);
		pRenderer->destroyRootSignature(mDeferredRootSignature);
		pRenderer->destroyRootSignature(mDeferredComposeRootSignature);

		pRenderer->destroyBuffer(mFSVertexBuffer);
		pRenderer->destroyBuffer(mFSIndexBuffer);
		pRenderer->destroyTexture(mBlackTexture);
	}

	void RGMMesh::Load(IRenderer* pRenderer, ISwapChain* pSwapChain)
	{
		// Create graphics pipeline.
		FGraphicsPipelineCreateDesc pipelineDesc = {};
		pipelineDesc.pRootSignature = mPrepassRootSignature;
		pipelineDesc.pShader = mPrepassShader;
		pipelineDesc.pBlendState = nullptr;
		pipelineDesc.pDepthStencilState = nullptr;
		pipelineDesc.pRasterizerState = &mRasterizerState;
		pipelineDesc.depthStencilFormat = TEXTURE_FORMAT_D32_FLOAT;
		pipelineDesc.numRenderTargets = 0;
		pipelineDesc.renderTargetFormats[0] = TEXTURE_FORMAT_RGBA8;
;
		pipelineDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE;
		pipelineDesc.sampleCount = pSwapChain->getRenderTarget(0)->GetDesc()->sampleCount;
		pipelineDesc.sampleQuality = pSwapChain->getRenderTarget(0)->GetDesc()->sampleQuality;

		pRenderer->createPipeline(&mPipelineDepthPrepass, &pipelineDesc);

		pipelineDesc.pRootSignature = mForwardRootSignature;
		pipelineDesc.pShader = mForwardShader;
		pipelineDesc.pDepthStencilState = &mNoDepthWriteState;
		pipelineDesc.depthStencilFormat = TEXTURE_FORMAT_D32_FLOAT;
		pipelineDesc.numRenderTargets = 1;
		pipelineDesc.renderTargetFormats[0] = TEXTURE_FORMAT_RGBA8;
		pRenderer->createPipeline(&mPipelineForwardNoDepthWrite, &pipelineDesc);

		// Create graphics pipeline.
		FGraphicsPipelineCreateDesc deferredPipelineDesc = {};
		deferredPipelineDesc.pRootSignature = mDeferredRootSignature;
		deferredPipelineDesc.pShader = mDeferredShader;
		deferredPipelineDesc.pBlendState = nullptr;
		deferredPipelineDesc.pDepthStencilState = nullptr;
		deferredPipelineDesc.pRasterizerState = nullptr;
		deferredPipelineDesc.depthStencilFormat = TEXTURE_FORMAT_D32_FLOAT;
		deferredPipelineDesc.numRenderTargets = 3;
		deferredPipelineDesc.renderTargetFormats[0] = TEXTURE_FORMAT_RGBA32_FLOAT;
		deferredPipelineDesc.renderTargetFormats[1] = TEXTURE_FORMAT_RGBA8;
		deferredPipelineDesc.renderTargetFormats[2] = TEXTURE_FORMAT_RGBA8;
		deferredPipelineDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE;
		deferredPipelineDesc.sampleCount = pSwapChain->getRenderTarget(0)->GetDesc()->sampleCount;
		deferredPipelineDesc.sampleQuality = pSwapChain->getRenderTarget(0)->GetDesc()->sampleQuality;

		pRenderer->createPipeline(&mDeferredPipeline, &deferredPipelineDesc);

		// Create graphics pipeline.
		FGraphicsPipelineCreateDesc deferredComposePipelineDesc = {};
		deferredComposePipelineDesc.pRootSignature = mDeferredComposeRootSignature;
		deferredComposePipelineDesc.pShader = mDeferredComposeShader;
		deferredComposePipelineDesc.pBlendState = nullptr;
		deferredComposePipelineDesc.pDepthStencilState = &mNoDepthWriteState;
		deferredComposePipelineDesc.pRasterizerState = nullptr;
		deferredComposePipelineDesc.depthStencilFormat = TEXTURE_FORMAT_UNKNOWN;
		deferredComposePipelineDesc.numRenderTargets = 1;
		deferredComposePipelineDesc.renderTargetFormats[0] = TEXTURE_FORMAT_RGBA8;
		deferredComposePipelineDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE;
		deferredComposePipelineDesc.sampleCount = pSwapChain->getRenderTarget(0)->GetDesc()->sampleCount;
		deferredComposePipelineDesc.sampleQuality = pSwapChain->getRenderTarget(0)->GetDesc()->sampleQuality;

		pRenderer->createPipeline(&mDeferredComposePipeline, &deferredComposePipelineDesc);
	}

	void RGMMesh::CreateDeferredResources(IRenderer* pRenderer)
	{
		FShaderCreateDesc deferredShaderDesc = {};
		deferredShaderDesc.shaders[0].path = "shaders/deferred.vert.spv";
		deferredShaderDesc.shaders[0].stage = EShaderStage::SHADER_STAGE_VERTEX;
		deferredShaderDesc.shaders[1].path = "shaders/deferred.frag.spv";
		deferredShaderDesc.shaders[1].stage = EShaderStage::SHADER_STAGE_FRAGMENT;

		pRenderer->createShader(&mDeferredShader, &deferredShaderDesc);

		FShaderCreateDesc deferredComposeShaderDesc = {};
		deferredComposeShaderDesc.shaders[0].path = "shaders/deferred_compose.vert.spv";
		deferredComposeShaderDesc.shaders[0].stage = EShaderStage::SHADER_STAGE_VERTEX;
		deferredComposeShaderDesc.shaders[1].path = "shaders/deferred_compose.frag.spv";
		deferredComposeShaderDesc.shaders[1].stage = EShaderStage::SHADER_STAGE_FRAGMENT;

		pRenderer->createShader(&mDeferredComposeShader, &deferredComposeShaderDesc);

		// Create input assembler layout
		FInputAttributeDescription iaAttributes[5];
		iaAttributes[0].binding = 0;
		iaAttributes[0].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[0].location = 0;
		iaAttributes[0].offset = 0;

		iaAttributes[1].binding = 0;
		iaAttributes[1].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[1].location = 1;
		iaAttributes[1].offset = sizeof(glm::vec3);

		iaAttributes[2].binding = 0;
		iaAttributes[2].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[2].location = 2;
		iaAttributes[2].offset = sizeof(glm::vec3) * 2;

		iaAttributes[3].binding = 0;
		iaAttributes[3].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[3].location = 3;
		iaAttributes[3].offset = sizeof(glm::vec3) * 3;

		iaAttributes[4].binding = 0;
		iaAttributes[4].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[4].location = 4;
		iaAttributes[4].offset = sizeof(glm::vec3) * 4;

		// Create input assembler binding
		FInputBindingDescription iaBinding;
		iaBinding.binding = 0;
		iaBinding.inputRate = EVertexInputRate::VERTEX_INPUT_RATE_VERTEX;
		iaBinding.stride = sizeof(FMeshVertex);

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
		bindings[4].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings[4].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[5].set = 1;
		bindings[5].binding = 4;
		bindings[5].descriptorCount = 1;
		bindings[5].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings[5].ppImmutableSamplers = &mSampler;
		bindings[5].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		// Create input assembler layout
		FInputAssemblerLayout iaLayout;
		iaLayout.numInputBindingDescriptions = 1;
		iaLayout.pInputBindingDescriptions = &iaBinding;
		iaLayout.numInputAttributeDescriptions = 5;
		iaLayout.pInputAttributeDescriptions = &iaAttributes[0];

		FRootSignatureCreateDesc rs2Desc;
		rs2Desc.pShaderProgram = mDeferredShader;
		rs2Desc.pInputAssemblerLayout = &iaLayout;
		rs2Desc.numDescriptorBindings = 6;
		rs2Desc.pDescriptorBindings = &bindings[0];

		pRenderer->createRootSignature(&mDeferredRootSignature, &rs2Desc);

		// Create input assembler layout
		iaAttributes[0].binding = 0;
		iaAttributes[0].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[0].location = 0;
		iaAttributes[0].offset = 0;

		iaAttributes[1].binding = 0;
		iaAttributes[1].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[1].location = 1;
		iaAttributes[1].offset = sizeof(glm::vec2);


		// Create input assembler binding
		FInputBindingDescription iaBinding2;
		iaBinding2.binding = 0;
		iaBinding2.inputRate = EVertexInputRate::VERTEX_INPUT_RATE_VERTEX;
		iaBinding2.stride = sizeof(glm::vec2) * 2;

		// Create input assembler layout
		FInputAssemblerLayout iaLayout2;
		iaLayout2.numInputBindingDescriptions = 1;
		iaLayout2.pInputBindingDescriptions = &iaBinding2;
		iaLayout2.numInputAttributeDescriptions = 2;
		iaLayout2.pInputAttributeDescriptions = &iaAttributes[0];

		// Create descriptor bindings.
		FDescriptorLayoutBinding bindings2[9];

		bindings2[0].set = 1;
		bindings2[0].binding = 0;
		bindings2[0].descriptorCount = 1;
		bindings2[0].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings2[0].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[1].set = 1;
		bindings2[1].binding = 1;
		bindings2[1].descriptorCount = 1;
		bindings2[1].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings2[1].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[2].set = 1;
		bindings2[2].binding = 2;
		bindings2[2].descriptorCount = 1;
		bindings2[2].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings2[2].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[3].set = 1;
		bindings2[3].binding = 3;
		bindings2[3].descriptorCount = 1;
		bindings2[3].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings2[3].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[4].set = 1;
		bindings2[4].binding = 6;
		bindings2[4].descriptorCount = 1;
		bindings2[4].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings2[4].ppImmutableSamplers = &mSampler;
		bindings2[4].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[5].set = 0;
		bindings2[5].binding = 0;
		bindings2[5].descriptorCount = 1;
		bindings2[5].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		bindings2[5].stageFlags = SHADER_STAGE_FLAG_VERTEX | SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[6].set = 1;
		bindings2[6].binding = 4;
		bindings2[6].descriptorCount = 1;
		bindings2[6].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings2[6].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[7].set = 1;
		bindings2[7].binding = 5;
		bindings2[7].descriptorCount = 1;
		bindings2[7].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings2[7].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[8].set = 1;
		bindings2[8].binding = 7;
		bindings2[8].descriptorCount = 1;
		bindings2[8].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings2[8].ppImmutableSamplers = &mShadowSampler;
		bindings2[8].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;


		// Create root signature.
		FRootSignatureCreateDesc rsDesc;
		rsDesc.pShaderProgram = mDeferredComposeShader;
		rsDesc.pInputAssemblerLayout = &iaLayout2;
		rsDesc.numDescriptorBindings = 9;
		rsDesc.pDescriptorBindings = &bindings2[0];

		pRenderer->createRootSignature(&mDeferredComposeRootSignature, &rsDesc);
	}

	void RGMMesh::CreateDepthResources(IRenderer* pRenderer)
	{
		FShaderCreateDesc prepassshaderDesc = {};
		prepassshaderDesc.shaders[0].path = "shaders/forward.vert.spv";
		prepassshaderDesc.shaders[0].stage = EShaderStage::SHADER_STAGE_VERTEX;

		pRenderer->createShader(&mPrepassShader, &prepassshaderDesc);

		// Create input assembler layout
		FInputAttributeDescription iaAttributes[5];
		iaAttributes[0].binding = 0;
		iaAttributes[0].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[0].location = 0;
		iaAttributes[0].offset = 0;

		iaAttributes[1].binding = 0;
		iaAttributes[1].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[1].location = 1;
		iaAttributes[1].offset = sizeof(glm::vec3);

		iaAttributes[2].binding = 0;
		iaAttributes[2].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[2].location = 2;
		iaAttributes[2].offset = sizeof(glm::vec3) * 2;

		iaAttributes[3].binding = 0;
		iaAttributes[3].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[3].location = 3;
		iaAttributes[3].offset = sizeof(glm::vec3) * 3;

		iaAttributes[4].binding = 0;
		iaAttributes[4].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[4].location = 4;
		iaAttributes[4].offset = sizeof(glm::vec3) * 4;

		// Create input assembler binding
		FInputBindingDescription iaBinding;
		iaBinding.binding = 0;
		iaBinding.inputRate = EVertexInputRate::VERTEX_INPUT_RATE_VERTEX;
		iaBinding.stride = sizeof(FMeshVertex);

		// Create descriptor bindings.
		FDescriptorLayoutBinding bindings[1];
		bindings[0].set = 0;
		bindings[0].binding = 0;
		bindings[0].descriptorCount = 1;
		bindings[0].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		bindings[0].stageFlags = SHADER_STAGE_FLAG_VERTEX | SHADER_STAGE_FLAG_FRAGMENT;

		// Create input assembler layout
		FInputAssemblerLayout iaLayout;
		iaLayout.numInputBindingDescriptions = 1;
		iaLayout.pInputBindingDescriptions = &iaBinding;
		iaLayout.numInputAttributeDescriptions = 5;
		iaLayout.pInputAttributeDescriptions = &iaAttributes[0];

		FRootSignatureCreateDesc rs2Desc;
		rs2Desc.pShaderProgram = mPrepassShader;
		rs2Desc.pInputAssemblerLayout = &iaLayout;
		rs2Desc.numDescriptorBindings = 1;
		rs2Desc.pDescriptorBindings = &bindings[0];

		pRenderer->createRootSignature(&mPrepassRootSignature, &rs2Desc);
	}


	void RGMMesh::CreateForwardResources(IRenderer* pRenderer)
	{
		// Create shaders.
		FShaderCreateDesc shaderDesc = {};
		shaderDesc.shaders[0].path = "shaders/forward.vert.spv";
		shaderDesc.shaders[0].stage = EShaderStage::SHADER_STAGE_VERTEX;
		shaderDesc.shaders[1].path = "shaders/forward.frag.spv";
		shaderDesc.shaders[1].stage = EShaderStage::SHADER_STAGE_FRAGMENT;

		pRenderer->createShader(&mForwardShader, &shaderDesc);

		// Create input assembler layout
		FInputAttributeDescription iaAttributes[5];
		iaAttributes[0].binding = 0;
		iaAttributes[0].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[0].location = 0;
		iaAttributes[0].offset = 0;

		iaAttributes[1].binding = 0;
		iaAttributes[1].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[1].location = 1;
		iaAttributes[1].offset = sizeof(glm::vec3);

		iaAttributes[2].binding = 0;
		iaAttributes[2].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[2].location = 2;
		iaAttributes[2].offset = sizeof(glm::vec3) * 2;

		iaAttributes[3].binding = 0;
		iaAttributes[3].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[3].location = 3;
		iaAttributes[3].offset = sizeof(glm::vec3) * 3;

		iaAttributes[4].binding = 0;
		iaAttributes[4].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[4].location = 4;
		iaAttributes[4].offset = sizeof(glm::vec3) * 4;

		// Create input assembler binding
		FInputBindingDescription iaBinding;
		iaBinding.binding = 0;
		iaBinding.inputRate = EVertexInputRate::VERTEX_INPUT_RATE_VERTEX;
		iaBinding.stride = sizeof(FMeshVertex);

		// Create input assembler layout
		FInputAssemblerLayout iaLayout;
		iaLayout.numInputBindingDescriptions = 1;
		iaLayout.pInputBindingDescriptions = &iaBinding;
		iaLayout.numInputAttributeDescriptions = 5;
		iaLayout.pInputAttributeDescriptions = &iaAttributes[0];

		// Create descriptor bindings.
		FDescriptorLayoutBinding bindings[5];
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

		bindings[2].set = 0;
		bindings[2].binding = 1;
		bindings[2].descriptorCount = 1;
		bindings[2].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings[2].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[3].set = 1;
		bindings[3].binding = 2;
		bindings[3].descriptorCount = 1;
		bindings[3].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings[3].ppImmutableSamplers = &mSampler;
		bindings[3].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[4].set = 2;
		bindings[4].binding = 0;
		bindings[4].descriptorCount = 1;
		bindings[4].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		bindings[4].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		// Create root signature.
		FRootSignatureCreateDesc rsDesc;
		rsDesc.pShaderProgram = mForwardShader;
		rsDesc.pInputAssemblerLayout = &iaLayout;
		rsDesc.numDescriptorBindings = 5;
		rsDesc.pDescriptorBindings = &bindings[0];

		pRenderer->createRootSignature(&mForwardRootSignature, &rsDesc);
	}

	void RGMMesh::Unload(IRenderer* pRenderer)
	{
		pRenderer->destroyPipeline(mPipelineForwardNoDepthWrite);
		pRenderer->destroyPipeline(mPipelineDepthPrepass);
		pRenderer->destroyPipeline(mDeferredComposePipeline);
		pRenderer->destroyPipeline(mDeferredPipeline);
	}

	FRGMForwardTargets RGMMesh::RGDrawDepth(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* pDepth, RCamera* pCamera, uint32_t numMeshes, RMesh** ppMeshes)
	{
		FRGMForwardTargets output;

		// Add the pass and setup its inputs and outputs.
		auto pass = pBuilder->add_pass("draw_meshes_depth_prepass", [&](RRenderPassBuilder& builder)
		{
			if (pDepth != nullptr)
			{
				output.pDepth = builder.write_resource("depth", pDepth, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_DEPTH_WRITE);
			}
		});

		// Set execute function.
		pass->setExecuteFunc([=](IRenderer* renderer, RRenderGraph& graph, RCompiledPass& pass, ICommandList* cmd)
		{
			FLoadStoreOps loadStoreOps = {};
			loadStoreOps.depthLoadOp = LOAD_OP_CLEAR;
			loadStoreOps.depthStoreOp = STORE_OP_STORE;
			loadStoreOps.depthClearValue = 1.0f;

			IRenderTarget* depth = pass.getRenderTargetResource("depth");

			// Begin rendering pass.
			renderer->cmdBeginRenderPass(cmd, 0, nullptr, depth, &loadStoreOps);

			// Bind graphics pipeline.
			renderer->cmdBindPipeline(cmd, mPipelineDepthPrepass);
			renderer->cmdSetViewport(cmd, FRect({ depth->GetDesc()->width,  depth->GetDesc()->height }, { 0, 0 }));
			renderer->cmdSetScissorRects(cmd, FRect({ depth->GetDesc()->width,  depth->GetDesc()->height }, { 0, 0 }));
			renderer->cmdSetDynamicDepthBias(cmd, 4.5f, 0.0f, 2.5f);

			static auto startTime = std::chrono::high_resolution_clock::now();

			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() / 10.0f;

			// Create dynamic uniform allocation.
			FDynamicUniformAllocation cameraUniformAlloc;
			mUniforms.model = glm::mat4(1.0);
			mUniforms.view = pCamera->getTransform()->getMatrix();
			mUniforms.proj = pCamera->getProjectionMatrix();

			renderer->cmdAllocateDynamicUniformBuffer(cmd, &cameraUniformAlloc, sizeof(FForwardMeshUniforms), &mUniforms);

			FDescriptorBinding bindings[1];
			bindings[0].set = 0;
			bindings[0].binding = 0;
			bindings[0].numDescriptors = 1;
			bindings[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			bindings[0].ppBuffers = &cameraUniformAlloc.buffer;
			bindings[0].range = cameraUniformAlloc.range;
			bindings[0].offset = cameraUniformAlloc.offset;

			// Bind uniform buffer.
			renderer->cmdBindDescriptors(cmd, 1, &bindings[0]);

			for (uint32_t i = 0; i < numMeshes; ++i)
			{
				RMesh* mesh = ppMeshes[i];

				renderer->cmdBindIndexBuffer(cmd, mesh->mIndexBuffer);
				renderer->cmdBindVertexBuffer(cmd, 1, &mesh->mVertexBuffer);

				for (uint32_t i = 0; i < mesh->mSubMeshes.size(); ++i)
				{
					// Draw mesh.
					renderer->cmdDrawIndexed(cmd, mesh->mSubMeshes[i].mIndexCount, mesh->mSubMeshes[i].mIndexOffset, mesh->mSubMeshes[i].mVertexOffset);
				}
			}

			// Finish rendering pass.
			renderer->cmdEndRenderPass(cmd);
		});

		return output;
	}

	FRGMForwardTargets RGMMesh::RGDrawMeshesForwardDepthPrepass(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* pColor, FVirtualResourceRef* pDepth, FVirtualResourceRef* pSsao, RCamera* pCamera, uint32_t numMeshes, RMesh** ppMeshes)
	{
		FRGMForwardTargets output;

		// Add the pass and setup its inputs and outputs.
		auto pass = pBuilder->add_pass("draw_meshes_forward", [&](RRenderPassBuilder& builder)
		{
			if (pColor != nullptr)
			{
				output.pColor = builder.write_resource("color", pColor, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT);
			}

			if (pDepth != nullptr)
			{
				output.pDepth = builder.read_resource("depth", pDepth, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_DEPTH_READ);
			}

			if (pSsao != nullptr)
			{
				builder.read_resource("ssao", pSsao, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			}
		});

		// Set execute function.
		pass->setExecuteFunc([=](IRenderer* renderer, RRenderGraph& graph, RCompiledPass& pass, ICommandList* cmd)
		{
			FLoadStoreOps loadStoreOps = {};
			loadStoreOps.colorLoadOps[0] = LOAD_OP_CLEAR;
			loadStoreOps.colorStoreOps[0] = STORE_OP_STORE;
			loadStoreOps.colorClearValues[0][0] = 0.529f;
			loadStoreOps.colorClearValues[0][1] = 0.808f;
			loadStoreOps.colorClearValues[0][2] = 0.980f;
			loadStoreOps.colorClearValues[0][3] = 1.0f;
			loadStoreOps.depthLoadOp = LOAD_OP_LOAD;
			loadStoreOps.depthStoreOp = STORE_OP_DONTCARE;
			loadStoreOps.depthClearValue = 1.0f;

			IRenderTarget* color = pass.getRenderTargetResource("color");
			IRenderTarget* depth = pass.getRenderTargetResource("depth");
			IRenderTarget* ssao = pass.getRenderTargetResource("ssao");

			// Begin rendering pass.
			renderer->cmdBeginRenderPass(cmd, 1, &color, depth, &loadStoreOps);

			// Bind graphics pipeline.
			renderer->cmdBindPipeline(cmd, mPipelineForwardNoDepthWrite);
			renderer->cmdSetViewport(cmd, FRect({ color->GetDesc()->width,  color->GetDesc()->height }, { 0, 0 }));
			renderer->cmdSetScissorRects(cmd, FRect({ color->GetDesc()->width,  color->GetDesc()->height }, { 0, 0 }));

			static auto startTime = std::chrono::high_resolution_clock::now();
			
			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() / 10.0f;

			// Create dynamic uniform allocation.
			FDynamicUniformAllocation cameraUniformAlloc;
			mUniforms.model = glm::mat4(1.0);
			mUniforms.view = pCamera->getTransform()->getMatrix();
			mUniforms.proj = pCamera->getProjectionMatrix();
			mUniforms.screenSize = glm::vec2(color->GetDesc()->width, color->GetDesc()->height);

			renderer->cmdAllocateDynamicUniformBuffer(cmd, &cameraUniformAlloc, sizeof(FForwardMeshUniforms), &mUniforms);

			FDescriptorBinding bindings[2];
			bindings[0].set = 0;
			bindings[0].binding = 0;
			bindings[0].numDescriptors = 1;
			bindings[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			bindings[0].ppBuffers = &cameraUniformAlloc.buffer;
			bindings[0].range = cameraUniformAlloc.range;
			bindings[0].offset = cameraUniformAlloc.offset;

			ITexture* ssaoTex = ssao->GetTexture();

			bindings[1].set = 0;
			bindings[1].binding = 1;
			bindings[1].numDescriptors = 1;
			bindings[1].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[1].ppTextures = &ssaoTex;
			bindings[1].range = 0; // range 0 means full range.
			bindings[1].offset = 0;

			// Bind uniform buffer.
			renderer->cmdBindDescriptors(cmd, 2, &bindings[0]);

			for (uint32_t i = 0; i < numMeshes; ++i)
			{
				RMesh* mesh = ppMeshes[i];

				renderer->cmdBindIndexBuffer(cmd, mesh->mIndexBuffer);
				renderer->cmdBindVertexBuffer(cmd, 1, &mesh->mVertexBuffer);

				for (uint32_t i = 0; i < mesh->mSubMeshes.size(); ++i)
				{
					bindings[0].set = 1;
					bindings[0].binding = 0;
					bindings[0].numDescriptors = 1;
					bindings[0].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					bindings[0].ppTextures = &mesh->mSubMeshes[i].mMaterial->mTextures[TEXTURE_SLOT_ALBEDO];
					bindings[0].range = 0; // range 0 means full range.
					bindings[0].offset = 0;

					// Bind resources.
					renderer->cmdBindDescriptors(cmd, mesh->mSubMeshes[i].mMaterial->mTextures[TEXTURE_SLOT_ALBEDO] != nullptr ? 1 : 0, &bindings[0]);

					// Draw mesh.
					renderer->cmdDrawIndexed(cmd, mesh->mSubMeshes[i].mIndexCount, mesh->mSubMeshes[i].mIndexOffset, mesh->mSubMeshes[i].mVertexOffset);
				}
			}

			// Finish rendering pass.
			renderer->cmdEndRenderPass(cmd);
		});

		return output;
	}

	FRGMDeferredTargets RGMMesh::RGDrawMeshesDeferredGBuffer(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* pDepth, RCamera* pCamera, uint32_t numMeshes, RMesh** ppMeshes)
	{
		FRGMDeferredTargets output;

		// Add the pass and setup its inputs and outputs.
		auto pass1 = pBuilder->add_pass("generate_gbuffer_targets", [&](RRenderPassBuilder& builder)
		{
			FVirtualTextureDesc backbufferInfo = pDepth->mResource->mDesc.GetTextureDesc();

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
			output.pAlbedo = builder.create_resource("gbuffer_albedo", desc);

			output.pNormal = builder.create_resource("gbuffer_normals", desc);

			desc.mTextureDesc.format = ETextureFormat::TEXTURE_FORMAT_RGBA32_FLOAT;

			output.pPosition = builder.create_resource("gbuffer_position", desc);
		});

		// Add the pass and setup its inputs and outputs.
		auto pass2 = pBuilder->add_pass("fill_gbuffer", [&](RRenderPassBuilder& builder)
		{
			if (output.pAlbedo != nullptr)
			{
				output.pAlbedo = builder.write_resource("gbuffer_albedo", output.pAlbedo, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT);
			}

			if (output.pNormal != nullptr)
			{
				output.pNormal = builder.write_resource("gbuffer_normals", output.pNormal, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT);
			}

			if (output.pPosition != nullptr)
			{
				output.pPosition = builder.write_resource("gbuffer_position", output.pPosition, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT);
			}

			if (pDepth != nullptr)
			{
				output.pDepth = builder.write_resource("depth", pDepth, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_DEPTH_WRITE);
			}
			
		});

		// Set execute function.
		pass2->setExecuteFunc([=](IRenderer* renderer, RRenderGraph& graph, RCompiledPass& pass, ICommandList* cmd)
		{
			FLoadStoreOps loadStoreOps = {};
			loadStoreOps.colorLoadOps[0] = LOAD_OP_CLEAR;
			loadStoreOps.colorStoreOps[0] = STORE_OP_STORE;
			loadStoreOps.colorLoadOps[1] = LOAD_OP_CLEAR;
			loadStoreOps.colorStoreOps[1] = STORE_OP_STORE;
			loadStoreOps.colorLoadOps[2] = LOAD_OP_CLEAR;
			loadStoreOps.colorStoreOps[2] = STORE_OP_STORE;

			loadStoreOps.colorClearValues[0][0] = 0.0f;
			loadStoreOps.colorClearValues[0][1] = 0.0f;
			loadStoreOps.colorClearValues[0][2] = 0.0f;
			loadStoreOps.colorClearValues[0][3] = 0.0f;

			loadStoreOps.colorClearValues[1][0] = 0.0f;
			loadStoreOps.colorClearValues[1][1] = 0.0f;
			loadStoreOps.colorClearValues[1][2] = 0.0f;
			loadStoreOps.colorClearValues[1][3] = 0.0f;

			loadStoreOps.colorClearValues[2][0] = 0.0f;
			loadStoreOps.colorClearValues[2][1] = 0.0f;
			loadStoreOps.colorClearValues[2][2] = 0.0f;
			loadStoreOps.colorClearValues[2][3] = 0.0f;

			loadStoreOps.depthLoadOp = LOAD_OP_CLEAR;
			loadStoreOps.depthStoreOp = STORE_OP_STORE;
			loadStoreOps.depthClearValue = 1.0f;

			IRenderTarget* albedo = pass.getRenderTargetResource("gbuffer_albedo");
			IRenderTarget* position = pass.getRenderTargetResource("gbuffer_position");
			IRenderTarget* normal = pass.getRenderTargetResource("gbuffer_normals");
			IRenderTarget* depth = pass.getRenderTargetResource("depth");

			IRenderTarget* targets[] = { position, albedo, normal };

			// Begin rendering pass.
			renderer->cmdBeginRenderPass(cmd, 3, &targets[0], depth, &loadStoreOps);

			// Bind graphics pipeline.
			renderer->cmdBindPipeline(cmd, mDeferredPipeline);
			renderer->cmdSetViewport(cmd, FRect({ albedo->GetDesc()->width,  albedo->GetDesc()->height }, { 0, 0 }));
			renderer->cmdSetScissorRects(cmd, FRect({ albedo->GetDesc()->width,  albedo->GetDesc()->height }, { 0, 0 }));

			// Create dynamic uniform allocation.
			FDynamicUniformAllocation uniformAlloc;
			mDeferredUniforms.cModel = glm::mat4(1.0);
			mDeferredUniforms.cView = pCamera->getTransform()->getMatrix();
			mDeferredUniforms.cProj = pCamera->getProjectionMatrix();
			mDeferredUniforms.cFarZ = pCamera->getFarZ();
			mDeferredUniforms.cNearZ = pCamera->getNearZ();

			renderer->cmdAllocateDynamicUniformBuffer(cmd, &uniformAlloc, sizeof(FDeferredUniforms), &mDeferredUniforms);

			FDescriptorBinding bindings[4];
			bindings[0].set = 0;
			bindings[0].binding = 0;
			bindings[0].numDescriptors = 1;
			bindings[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			bindings[0].ppBuffers = &uniformAlloc.buffer;
			bindings[0].range = uniformAlloc.range;
			bindings[0].offset = uniformAlloc.offset;

			// Bind uniform buffer.
			renderer->cmdBindDescriptors(cmd, 1, &bindings[0]);

			for (uint32_t i = 0; i < numMeshes; ++i)
			{
				RMesh* mesh = ppMeshes[i];

				renderer->cmdBindIndexBuffer(cmd, mesh->mIndexBuffer);
				renderer->cmdBindVertexBuffer(cmd, 1, &mesh->mVertexBuffer);

				for (uint32_t i = 0; i < mesh->mSubMeshes.size(); ++i)
				{
					bindings[0].set = 1;
					bindings[0].binding = 0;
					bindings[0].numDescriptors = 1;
					bindings[0].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					bindings[0].ppTextures = &mesh->mSubMeshes[i].mMaterial->mTextures[TEXTURE_SLOT_ALBEDO];
					bindings[0].range = 0; 
					bindings[0].offset = 0;

					bindings[1].set = 1;
					bindings[1].binding = 1;
					bindings[1].numDescriptors = 1;
					bindings[1].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					bindings[1].ppTextures = &mesh->mSubMeshes[i].mMaterial->mTextures[TEXTURE_SLOT_NORMAL];
					bindings[1].range = 0;
					bindings[1].offset = 0;

					bindings[2].set = 1;
					bindings[2].binding = 2;
					bindings[2].numDescriptors = 1;
					bindings[2].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					bindings[2].ppTextures = &mesh->mSubMeshes[i].mMaterial->mTextures[TEXTURE_SLOT_METALLIC];
					bindings[2].range = 0;
					bindings[2].offset = 0;

					bindings[3].set = 1;
					bindings[3].binding = 3;
					bindings[3].numDescriptors = 1;
					bindings[3].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					bindings[3].ppTextures = &mesh->mSubMeshes[i].mMaterial->mTextures[TEXTURE_SLOT_ROUGHNESS];
					bindings[3].range = 0;
					bindings[3].offset = 0;

					if (*bindings[0].ppTextures == nullptr)
					{
						bindings[0].ppTextures = &mBlackTexture;
					}

					if (*bindings[1].ppTextures == nullptr)
					{
						bindings[1].ppTextures = &mBlackTexture;
					}

					if (*bindings[2].ppTextures == nullptr)
					{
						bindings[2].ppTextures = &mBlackTexture;
					}

					if (*bindings[3].ppTextures == nullptr)
					{
						bindings[3].ppTextures = &mBlackTexture;
					}

					// Bind resources.
					renderer->cmdBindDescriptors(cmd, 4, &bindings[0]);

					// Draw mesh.
					renderer->cmdDrawIndexed(cmd, mesh->mSubMeshes[i].mIndexCount, mesh->mSubMeshes[i].mIndexOffset, mesh->mSubMeshes[i].mVertexOffset);
				}
			}

			// Finish rendering pass.
			renderer->cmdEndRenderPass(cmd);
		});

		return output;
	}

	/*! Acquire next swapchain image and import the target into the render graph system. */
	FRGMDeferredComposeTargets RGMMesh::RGDrawMeshesDeferredCompose(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* pOutput, FVirtualResourceRef* pAlbedo, FVirtualResourceRef* pDepth, FVirtualResourceRef* pPosition, FVirtualResourceRef* pNormals, FVirtualResourceRef* pSsao, FVirtualResourceRef* pShadowMap, FVirtualResourceRef* pVolumetricFog, RCamera* pCamera, RCamera* pShadowCamera)
	{
		FRGMDeferredComposeTargets output;

		// Add the pass and setup its inputs and outputs.
		auto pass2 = pBuilder->add_pass("fill_gbuffer", [&](RRenderPassBuilder& builder)
		{
			if (pOutput != nullptr)
			{
				output.pOutput = builder.write_resource("output", pOutput, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT);
			}

			if (pAlbedo != nullptr)
			{
				output.pAlbedo = builder.read_resource("albedo", pAlbedo, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			}

			if (pNormals != nullptr)
			{
				output.pNormal = builder.read_resource("normals", pNormals, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			}

			if (pPosition != nullptr)
			{
				output.pPosition = builder.read_resource("positions", pPosition, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			}

			if (pSsao != nullptr)
			{
				output.pSSAO = builder.read_resource("ssao", pSsao, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			}

			if (pDepth != nullptr)
			{
				output.pDepth = builder.read_resource("depth", pDepth, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			}

			if (pShadowMap != nullptr)
			{
				output.pShadowMap = builder.read_resource("shadowMap", pShadowMap, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			}

			if (pVolumetricFog != nullptr)
			{
				output.pVolumetricFog = builder.read_resource("volumetricFog", pVolumetricFog, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			}

		});

		// Set execute function.
		pass2->setExecuteFunc([=](IRenderer* renderer, RRenderGraph& graph, RCompiledPass& pass, ICommandList* cmd)
		{
			FLoadStoreOps loadStoreOps = {};
			loadStoreOps.colorLoadOps[0] = LOAD_OP_CLEAR;
			loadStoreOps.colorStoreOps[0] = STORE_OP_STORE;
			
			loadStoreOps.depthLoadOp = LOAD_OP_DONTCARE;
			loadStoreOps.depthStoreOp = STORE_OP_DONTCARE;
			loadStoreOps.depthClearValue = 1.0f;

			IRenderTarget* output = pass.getRenderTargetResource("output");
			IRenderTarget* albedo = pass.getRenderTargetResource("albedo");
			IRenderTarget* position = pass.getRenderTargetResource("positions");
			IRenderTarget* normal = pass.getRenderTargetResource("normals");
			IRenderTarget* depth = pass.getRenderTargetResource("depth");
			IRenderTarget* ssao = pass.getRenderTargetResource("ssao");
			IRenderTarget* shadow = pass.getRenderTargetResource("shadowMap");
			IRenderTarget* volumeFog = pass.getRenderTargetResource("volumetricFog");

			// Begin rendering pass.
			renderer->cmdBeginRenderPass(cmd, 1, &output, nullptr, &loadStoreOps);

			// Bind graphics pipeline.
			renderer->cmdBindPipeline(cmd, mDeferredComposePipeline);
			renderer->cmdSetViewport(cmd, FRect({ output->GetDesc()->width,  output->GetDesc()->height }, { 0, 0 }));
			renderer->cmdSetScissorRects(cmd, FRect({ output->GetDesc()->width,  output->GetDesc()->height }, { 0, 0 }));

			// Bind index/vertex buffer
			renderer->cmdBindIndexBuffer(cmd, mFSIndexBuffer);
			renderer->cmdBindVertexBuffer(cmd, 1, &mFSVertexBuffer);

			ITexture* albedoTexture = albedo->GetTexture();
			ITexture* normalTexture = normal->GetTexture();
			ITexture* positionTexture = position->GetTexture();
			ITexture* ssaoTexture = ssao->GetTexture();
			ITexture* shadowTexture = shadow->GetTexture();
			ITexture* volumeFogTexture = volumeFog->GetTexture();

			FDescriptorBinding bindings[7];

			bindings[0].set = 1;
			bindings[0].binding = 0;
			bindings[0].numDescriptors = 1;
			bindings[0].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[0].ppTextures = &albedoTexture;
			bindings[0].range = 0; // range 0 means full range.
			bindings[0].offset = 0;

			bindings[1].set = 1;
			bindings[1].binding = 1;
			bindings[1].numDescriptors = 1;
			bindings[1].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[1].ppTextures = &positionTexture;
			bindings[1].range = 0; // range 0 means full range.
			bindings[1].offset = 0;

			bindings[2].set = 1;
			bindings[2].binding = 2;
			bindings[2].numDescriptors = 1;
			bindings[2].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[2].ppTextures = &normalTexture;
			bindings[2].range = 0; // range 0 means full range.
			bindings[2].offset = 0;

			bindings[3].set = 1;
			bindings[3].binding = 3;
			bindings[3].numDescriptors = 1;
			bindings[3].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[3].ppTextures = &ssaoTexture;
			bindings[3].range = 0; // range 0 means full range.
			bindings[3].offset = 0;

			bindings[4].set = 1;
			bindings[4].binding = 4;
			bindings[4].numDescriptors = 1;
			bindings[4].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[4].ppTextures = &shadowTexture;
			bindings[4].range = 0; // range 0 means full range.
			bindings[4].offset = 0;

			bindings[5].set = 1;
			bindings[5].binding = 5;
			bindings[5].numDescriptors = 1;
			bindings[5].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[5].ppTextures = &volumeFogTexture;
			bindings[5].range = 0; // range 0 means full range.
			bindings[5].offset = 0;


			FDynamicUniformAllocation uniformAlloc;

			FDeferredComposeUniforms composeUniforms;
			composeUniforms.cView = pCamera->getTransform()->getMatrix();
			composeUniforms.cProj = pCamera->getProjectionMatrix();
			composeUniforms.cInvView = glm::inverse(pCamera->getTransform()->getMatrix());
			composeUniforms.cShadowMatrix =  pShadowCamera->getProjectionMatrix() *  pShadowCamera->getTransform()->getMatrix();
			composeUniforms.cLightDir = glm::vec4(pShadowCamera->getTransform()->GetForward(), 0.0f);
			composeUniforms.cFarZ = pCamera->getFarZ();
			composeUniforms.cNearZ = pCamera->getNearZ();
			composeUniforms.cScreenSize = glm::vec2(output->GetDesc()->width, output->GetDesc()->height);

			renderer->cmdAllocateDynamicUniformBuffer(cmd, &uniformAlloc, sizeof(FDeferredComposeUniforms), &composeUniforms);

			bindings[6].set = 0;
			bindings[6].binding = 0;
			bindings[6].numDescriptors = 1;
			bindings[6].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			bindings[6].ppBuffers = &uniformAlloc.buffer;
			bindings[6].range = uniformAlloc.range;
			bindings[6].offset = uniformAlloc.offset;

			// Bind resources.
			renderer->cmdBindDescriptors(cmd, 7, &bindings[0]);

			// Draw mesh.
			renderer->cmdDrawIndexed(cmd, 6, 0, 0);

			// Finish rendering pass.
			renderer->cmdEndRenderPass(cmd);
		});

		return output;
	}

}
