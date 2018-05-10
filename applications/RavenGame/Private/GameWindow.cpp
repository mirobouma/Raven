#include "GameWindow.h"
#include "Engine/Globals.h"
#include "Engine/Engine.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp> // logger
#include <assimp/DefaultLogger.hpp> // logger

namespace Raven
{
	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	IQueue* gGraphicsQueue = nullptr;
	ICommandPool* gCommandPool = nullptr;
	ICommandList* gCommandLists[3];
	ISemaphore* gImageAquiredSemaphore;
	ISemaphore* gRenderCompleteSemaphores[3];
	IFence* gRenderCompleteFences[3];
	uint32_t gCurrentImage = 0;
	uint32_t gImageCount = 0;
	IPipeline* gRenderPipeline = nullptr;
	IRootSignature* gRootSignature = nullptr;
	IShader* gShader = nullptr;
	IBuffer* gUniformBuffers[3];
	UniformBufferObject gUniformBuffer;
	IBuffer* gIndexBuffer = nullptr;
	IBuffer* gVertexBuffer = nullptr;
	ISampler* gSampler = nullptr;
	std::vector<glm::vec3> gVertices;
	std::vector<uint32_t> gIndices;

    RGameWindow::RGameWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler)
            : RRenderableWindow(title, size, position, flags, handler)
    {

    }

    void RGameWindow::OnCreate()
    {
        // Call super.
		RRenderableWindow::OnCreate();
		renderer = GEngine->GetRenderer();

		// Get swapchain image count
		gImageCount = _SwapChain->getImageCount();

		// Retrieve graphics queue.
		FQueueCreateDesc queueDesc;
		queueDesc.type = ECommandType::COMMAND_TYPE_DIRECT;
		renderer->createQueue(&gGraphicsQueue, &queueDesc);

		// Create command pool.
		FCommandPoolCreateDesc poolDesc;
		poolDesc.type = ECommandType::COMMAND_TYPE_DIRECT;
		renderer->createCommandPool(&gCommandPool, gGraphicsQueue, &poolDesc);

		// Create command lists.
		renderer->createCommandLists(&gCommandLists[0], 3, gCommandPool);

		// Create synchronization primitives.
		for (uint32_t i = 0; i < 3; ++i)
		{
			renderer->createFence(&gRenderCompleteFences[i]);
			renderer->createSemaphore(&gRenderCompleteSemaphores[i]);
		}

		FSamplerCreateDesc samplerDesc;
		samplerDesc.addressModeU = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerDesc.addressModeV = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerDesc.addressModeW = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerDesc.anisotropyEnable = true;
		samplerDesc.comparisonFunc = ESamplerComparisonFunc::SAMPLER_COMPARISON_NONE;
		samplerDesc.anisotropyEnable = true;
		samplerDesc.borderColor = EBorderColor::BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		samplerDesc.maxAnisotropy = 16;
		samplerDesc.magFilter = ESamplerFilter::SAMPLER_FILTER_LINEAR;
		samplerDesc.minFilter = ESamplerFilter::SAMPLER_FILTER_LINEAR;
		samplerDesc.maxLod = 0.0f;
		samplerDesc.minLod = 0.0f;
		samplerDesc.mipLodBias = 0.0f;
		samplerDesc.mipmapMode = EMipMode::MIP_MODE_LINEAR;

		renderer->createSampler(&gSampler, &samplerDesc);

		// Create image aquired semaphore
		renderer->createSemaphore(&gImageAquiredSemaphore);

		// Create shaders.
		FShaderCreateDesc shaderDesc = {};
		shaderDesc.shaders[0].path = "shaders/test.vert.spv";
		shaderDesc.shaders[0].stage = EShaderStage::SHADER_STAGE_VERTEX;
		shaderDesc.shaders[1].path = "shaders/test.frag.spv";
		shaderDesc.shaders[1].stage = EShaderStage::SHADER_STAGE_FRAGMENT;

		renderer->createShader(&gShader, &shaderDesc);

		// Create input assembler layout
		FInputAttributeDescription iaAttributes[3];
		iaAttributes[0].binding = 0;
		iaAttributes[0].format = TEXTURE_FORMAT_RGB32_F;
		iaAttributes[0].location = 0;
		iaAttributes[0].offset = 0;

		iaAttributes[1].binding = 0;
		iaAttributes[1].format = TEXTURE_FORMAT_RGB32_F;
		iaAttributes[1].location = 1;
		iaAttributes[1].offset = sizeof(glm::vec3);

		iaAttributes[2].binding = 0;
		iaAttributes[2].format = TEXTURE_FORMAT_RGB32_F;
		iaAttributes[2].location = 2;
		iaAttributes[2].offset = sizeof(glm::vec3) * 2;

		// Create input assembler binding
		FInputBindingDescription iaBinding;
		iaBinding.binding = 0;
		iaBinding.inputRate = EVertexInputRate::VERTEX_INPUT_RATE_VERTEX;
		iaBinding.stride = sizeof(glm::vec3) * 3;

		// Create input assembler layout
		FInputAssemblerLayout iaLayout;
		iaLayout.numInputBindingDescriptions = 1;
		iaLayout.pInputBindingDescriptions = &iaBinding;
		iaLayout.numInputAttributeDescriptions = 3;
		iaLayout.pInputAttributeDescriptions = &iaAttributes[0];

		// Create descriptor bindings.
		FDescriptorLayoutBinding bindings[2];
        bindings[0].set = 0;
		bindings[0].binding = 0;
		bindings[0].descriptorCount = 1;
		bindings[0].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		bindings[0].stageFlags = SHADER_STAGE_FLAG_ALL_GRAPHICS;

        bindings[1].set = 0;
		bindings[1].binding = 1;
		bindings[1].descriptorCount = 1;
		bindings[1].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings[1].ppImmutableSamplers = &gSampler;
		bindings[1].stageFlags = SHADER_STAGE_FLAG_ALL_GRAPHICS;


		// Create root signature.
		FRootSignatureCreateDesc rsDesc;
		rsDesc.pShaderProgram = gShader;
		rsDesc.pInputAssemblerLayout = &iaLayout;
		rsDesc.numDescriptorBindings = 2;
		rsDesc.pDescriptorBindings = &bindings[0];

		renderer->createRootSignature(&gRootSignature, &rsDesc);

		IRenderTarget* target = _SwapChain->getRenderTarget(_SwapChain->getCurrentImageIndex());

		// Create graphics pipeline.
		FGraphicsPipelineCreateDesc pipelineDesc;
		pipelineDesc.pRootSignature = gRootSignature;
		pipelineDesc.pShader = gShader;
		pipelineDesc.pBlendState = nullptr;
		pipelineDesc.pDepthStencilState = nullptr;
		pipelineDesc.pRasterizerState = nullptr;
		pipelineDesc.depthStencilFormat = TEXTURE_FORMAT_UNKNOWN;
		pipelineDesc.numRenderTargets = 1;
		pipelineDesc.renderTargetFormats[0] = target->GetDesc()->format;
		pipelineDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE;
		pipelineDesc.sampleCount = target->GetDesc()->sampleCount;
		pipelineDesc.sampleQuality = target->GetDesc()->sampleQuality;

		renderer->createPipeline(&gRenderPipeline, &pipelineDesc);

		gUniformBuffer.model = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		gUniformBuffer.view = glm::lookAt(glm::vec3(5.0f, 7.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		gUniformBuffer.proj = glm::perspective(glm::radians(45.0f), _SwapChain->getWidth() / (float)_SwapChain->getHeight(), 0.1f, 20.0f);
		gUniformBuffer.proj[1][1] *= -1;

		for (uint32_t i = 0; i < 3; ++i)
		{
			// Create index buffer.
			FBufferCreateDesc uBufferDesc = {};
			uBufferDesc.memoryProperties = MEMORY_PROPERTY_HOST_COHERENT | MEMORY_PROPERTY_HOST_VISIBLE;
			uBufferDesc.size = sizeof(UniformBufferObject);
			uBufferDesc.usage = BUFFER_USAGE_TRANSFER_DST | BUFFER_USAGE_UNIFORM_BUFFER;
			uBufferDesc.data = &gUniformBuffer;

			renderer->createBuffer(&gUniformBuffers[i], &uBufferDesc);
		}


		// Load teapot model
		LoadTeapot();

		// Create index buffer.
		FBufferCreateDesc iBufferDesc = {};
		iBufferDesc.memoryProperties = MEMORY_PROPERTY_DEVICE_LOCAL;
		iBufferDesc.size = gIndices.size() * sizeof(uint32_t);
		iBufferDesc.usage = BUFFER_USAGE_TRANSFER_DST | BUFFER_USAGE_INDEX_BUFFER;
		iBufferDesc.data = gIndices.data();

		renderer->createBuffer(&gIndexBuffer, &iBufferDesc);

		// Create vertex buffer.
		FBufferCreateDesc vBufferDesc = {};
		vBufferDesc.memoryProperties = MEMORY_PROPERTY_DEVICE_LOCAL;
		vBufferDesc.size = gVertices.size() * sizeof(glm::vec3);
		vBufferDesc.usage = BUFFER_USAGE_TRANSFER_DST | BUFFER_USAGE_VERTEX_BUFFER;
		vBufferDesc.data = gVertices.data();

		renderer->createBuffer(&gVertexBuffer, &vBufferDesc);
    }

	void RGameWindow::LoadTeapot()
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(std::string("models/model.obj"),
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_SplitLargeMeshes |
			aiProcess_SortByPType |
			aiProcess_PreTransformVertices);

		auto error = importer.GetErrorString();

		if (scene != NULL)
		{
			int vertexAmount = 0;
			int indexAmount = 0;

			// Load vertices, indices, normals and UV co-ordinates.
			for (uint32_t i = 0; i < scene->mNumMeshes; i++)
			{
				int subMeshVertexAmount = 0;
				int subMeshIndexAmount = 0;

				for (uint32_t j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
				{
					vertexAmount++;
					subMeshVertexAmount++;
					aiVector3D vert = scene->mMeshes[i]->mVertices[j];
					aiVector3D normal = scene->mMeshes[i]->mNormals[j];

					aiVector2D uv = aiVector2D(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
					if (scene->mMeshes[i]->mTextureCoords[0] != nullptr)
					{
						uv = aiVector2D(scene->mMeshes[i]->mTextureCoords[0][j].x, scene->mMeshes[i]->mTextureCoords[0][j].y);
					}

					gVertices.push_back(glm::vec3(vert.x, vert.y, vert.z));
					gVertices.push_back(glm::vec3(normal.x, normal.y, normal.z));
					gVertices.push_back(glm::vec3(uv.x, uv.y, 0.0f));
				}

				for (uint32_t j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
				{
					aiFace face = scene->mMeshes[i]->mFaces[j];
					for (uint32_t k = 0; k < face.mNumIndices; k++)
					{
						indexAmount++;
						subMeshIndexAmount++;

						gIndices.push_back(face.mIndices[k]);
					}
				}
			}
		}
	}

    void RGameWindow::OnResize(int width, int height)
    {
        // Call super.
		RRenderableWindow::OnResize(width, height);

		if (GEngine->GetRenderer()->isInitialized())
		{
			GEngine->GetRenderer()->waitForFence(gRenderCompleteFences[gCurrentImage]);

			GEngine->GetRenderer()->destroySwapChain(_SwapChain);
			GEngine->GetRenderer()->createSwapChain(this, &_SwapChain);
		}
    }

    void RGameWindow::OnMove(int x, int y)
    {
        // Call super.
		RRenderableWindow::OnMove(x, y);
    }

    void RGameWindow::OnFocus()
    {
        // Call super.
		RRenderableWindow::OnFocus();
    }

    void RGameWindow::OnFocusLost()
    {
        // Call super.
		RRenderableWindow::OnFocusLost();
    }

    void RGameWindow::OnClose()
    {
		for (uint32_t i = 0; i < 3; ++i)
		{
			renderer->waitForFence(gRenderCompleteFences[i]);
		}

		renderer->destroyQueue(gGraphicsQueue);
		renderer->destroyCommandLists(gCommandPool, &gCommandLists[0], 3);
		renderer->destroyCommandPool(gCommandPool);

		renderer->destroyBuffer(gIndexBuffer);
		renderer->destroyBuffer(gVertexBuffer);
		renderer->destroyShader(gShader);
		renderer->destroySampler(gSampler);
		renderer->destroyRootSignature(gRootSignature);
		renderer->destroyPipeline(gRenderPipeline);

		for (uint32_t i = 0; i < 3; ++i)
		{
			renderer->destroyBuffer(gUniformBuffers[i]);
			renderer->destroyFence(gRenderCompleteFences[i]);
			renderer->destroySemaphore(gRenderCompleteSemaphores[i]);
		}

		renderer->destroySemaphore(gImageAquiredSemaphore);

        // Call super.
		RRenderableWindow::OnClose();
    }

    void RGameWindow::Render()
    {
		// Aquire next swapchain image.
		renderer->swapChainAcquireNextImage(_SwapChain, gImageAquiredSemaphore, NULL);

		// Retrieve current swapchain image index.
		gCurrentImage = _SwapChain->getCurrentImageIndex();

		// Retrieve current render target for backbuffer.
		IRenderTarget* pRenderTarget = _SwapChain->getRenderTarget(gCurrentImage);

		// Get semaphores/fences for this frame.
		ISemaphore* renderCompleteSemaphore = gRenderCompleteSemaphores[gCurrentImage];
		IFence* renderCompleteFence = gRenderCompleteFences[gCurrentImage];

		// Get command list for this frame.
		ICommandList* cmd = gCommandLists[gCurrentImage];

		// Begin recording this command list.
		renderer->cmdBegin(cmd);

		// Update uniform.
		renderer->cmdUpdateBuffer(cmd, gUniformBuffers[gCurrentImage], &gUniformBuffer, 0, sizeof(UniformBufferObject));

		FBufferBarrier uBarrier;
		uBarrier.buffer = gUniformBuffers[gCurrentImage];
		uBarrier.requiredState = RESOURCE_STATE_UNIFORM_BUFFER;

		FTextureBarrier rtBarrier;
		rtBarrier.texture = pRenderTarget->GetTexture();
		rtBarrier.requiredState = RESOURCE_STATE_RENDER_TARGET;

		renderer->cmdResourceBarriers(cmd, 1, &uBarrier, 1, &rtBarrier, true);

		FLoadStoreOps loadStoreOps;
		loadStoreOps.colorLoadOps[0] = LOAD_OP_CLEAR;
		loadStoreOps.colorStoreOps[0] = STORE_OP_STORE;
		loadStoreOps.colorClearValues[0][0] = 0.0f;
		loadStoreOps.colorClearValues[0][1] = 0.0f;
		loadStoreOps.colorClearValues[0][2] = 1.0f;
		loadStoreOps.colorClearValues[0][3] = 1.0f;

		// Begin rendering pass.
		renderer->cmdBeginRenderPass(cmd, 1, &pRenderTarget, nullptr, &loadStoreOps);
		
		// Bind graphics pipeline.
		renderer->cmdBindPipeline(cmd, gRenderPipeline);
		renderer->cmdSetViewport(cmd, Math::FRect({ pRenderTarget->GetDesc()->width,  pRenderTarget->GetDesc()->height }, { 0, 0 }));
		renderer->cmdSetScissorRects(cmd, Math::FRect({ pRenderTarget->GetDesc()->width,  pRenderTarget->GetDesc()->height }, { 0, 0 }));
		renderer->cmdBindIndexBuffer(cmd, gIndexBuffer);
		renderer->cmdBindVertexBuffer(cmd, 1, &gVertexBuffer);
		renderer->cmdDrawIndexed(cmd, (uint32_t)3, 0);

		FDescriptorBinding binding;
		binding.set = 0;
		binding.binding = 0;
		binding.numDescriptors = 1;
		binding.type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.ppBuffers = &gUniformBuffers[gCurrentImage];

		// Bind uniform buffer.
		renderer->cmdBindDescriptors(cmd, 1, &binding);

		// Finish rendering pass.
		renderer->cmdEndRenderPass(cmd);

		rtBarrier.texture = pRenderTarget->GetTexture();
		rtBarrier.requiredState = RESOURCE_STATE_PRESENT;

		renderer->cmdResourceBarriers(cmd, 0, nullptr, 1, &rtBarrier, true);

		// Stop recording.
		renderer->cmdEnd(cmd);

		renderer->queueSubmit(gGraphicsQueue, 1, &cmd, renderCompleteFence, 1, &gImageAquiredSemaphore, 1, &renderCompleteSemaphore);
		renderer->queuePresent(gGraphicsQueue, 1, &_SwapChain, 1, &renderCompleteSemaphore);

		IFence* pNextFence = gRenderCompleteFences[(gCurrentImage + 1) % gImageCount];
		renderer->waitForFence(pNextFence);
    }
}