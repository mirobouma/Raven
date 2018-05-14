#include "GameWindow.h"
#include "Engine/Globals.h"
#include "Engine/Engine.h"
#include "Core/Debug/LogManager.h"

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
#include <chrono>
#include "UI/UI.h"

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
	IRenderTarget* gDepthStencil = nullptr;
	ISampler* gSampler = nullptr;
	ITexture* gTexture = nullptr;
	IRenderTarget* gBackbuffer = nullptr;
	std::vector<glm::vec3> gVertices;
	std::vector<uint32_t> gIndices;
	RUIRenderer	gUIRenderer;

	ImVec2 gameWindowSize = {1, 1};
	ImVec2 gameWindowPos = {0, 0};

	bool d_gameWindowOpen = true;
	bool d_consoleWindowOpen = true;
	bool d_propertyWindowOpen = true;

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
		FDescriptorLayoutBinding bindings[3];
        bindings[0].set = 0;
		bindings[0].binding = 0;
		bindings[0].descriptorCount = 1;
		bindings[0].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		bindings[0].stageFlags = SHADER_STAGE_FLAG_VERTEX;

        bindings[1].set = 1;
		bindings[1].binding = 0;
		bindings[1].descriptorCount = 1;
		bindings[1].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings[1].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[2].set = 1;
		bindings[2].binding = 1;
		bindings[2].descriptorCount = 1;
		bindings[2].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings[2].ppImmutableSamplers = &gSampler;
		bindings[2].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;


		// Create root signature.
		FRootSignatureCreateDesc rsDesc;
		rsDesc.pShaderProgram = gShader;
		rsDesc.pInputAssemblerLayout = &iaLayout;
		rsDesc.numDescriptorBindings = 3;
		rsDesc.pDescriptorBindings = &bindings[0];

		renderer->createRootSignature(&gRootSignature, &rsDesc);

		// Create game backbuffer.
		createGameWindow();

		IRenderTarget* target = _SwapChain->getRenderTarget(_SwapChain->getCurrentImageIndex());

		// Create graphics pipeline.
		FGraphicsPipelineCreateDesc pipelineDesc = {};
		pipelineDesc.pRootSignature = gRootSignature;
		pipelineDesc.pShader = gShader;
		pipelineDesc.pBlendState = nullptr;
		pipelineDesc.pDepthStencilState = nullptr;
		pipelineDesc.pRasterizerState = nullptr;
		pipelineDesc.depthStencilFormat = TEXTURE_FORMAT_D32_FLOAT;
		pipelineDesc.numRenderTargets = 1;
		pipelineDesc.renderTargetFormats[0] = gBackbuffer->GetDesc()->format;
		pipelineDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE;
		pipelineDesc.sampleCount = gBackbuffer->GetDesc()->sampleCount;
		pipelineDesc.sampleQuality = gBackbuffer->GetDesc()->sampleQuality;

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

		// Create texture.
		
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load("textures/pepe.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		uint32_t imageSize = texWidth * texHeight * 4;

		if (pixels == nullptr) 
		{
			RLOG_ERROR("failed to load texture image!");
		}

		FTextureCreateDesc texDesc = {};
		texDesc.type = TEXTURE_TYPE_2D;
		texDesc.memoryProperties = MEMORY_PROPERTY_DEVICE_LOCAL;
		texDesc.arrayLayers = 1;
		texDesc.baseArrayLayer = 0;
		texDesc.baseMipLevel = 0;
		texDesc.depth = 1;
		texDesc.format = TEXTURE_FORMAT_RGBA8;
		texDesc.width = texWidth;
		texDesc.height = texHeight;
		texDesc.mipLevels = 1;
		texDesc.pData = pixels;
		texDesc.sampleCount = 1;
		texDesc.sampleQuality = 0;
		texDesc.tiling = IMAGE_TILING_OPTIMAL;
		texDesc.usage = TEXTURE_USAGE_SAMPLED | TEXTURE_USAGE_TRANSFER_DST;

		renderer->createTexture(&gTexture, &texDesc);

		gUIRenderer.initialize(renderer, this, _SwapChain);
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
			for (uint32_t i = 0; i < 3; ++i)
			{
				renderer->waitForFence(gRenderCompleteFences[i]);
			}

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

	void RGameWindow::createGameWindow()
	{
		for (uint32_t i = 0; i < 3; ++i)
		{
			renderer->waitForFence(gRenderCompleteFences[i]);
		}

		if (gameWindowSize.x == 0 || gameWindowSize.y == 0)
		{
			return;
		}

		if (gBackbuffer != nullptr)
		{
			renderer->destroyRenderTarget(gBackbuffer);
			gBackbuffer = nullptr;
		}

		if (gDepthStencil != nullptr)
		{
			renderer->destroyRenderTarget(gDepthStencil);
			gDepthStencil = nullptr;
		}


		FRenderTargetCreateDesc texDesc = {};
		texDesc.memoryProperties = MEMORY_PROPERTY_DEVICE_LOCAL;
		texDesc.format = TEXTURE_FORMAT_RGBA8;
		texDesc.width = (uint32_t)gameWindowSize.x;
		texDesc.height = (uint32_t)gameWindowSize.y;
		texDesc.sampleCount = 1;
		texDesc.sampleQuality = 0;
		texDesc.tiling = IMAGE_TILING_OPTIMAL;
		texDesc.usage = FRenderTargetUsage::RENDER_TARGET_USAGE_COLOR_ATTACHMENT;
			
		renderer->createRenderTarget(&gBackbuffer, &texDesc);


		FRenderTargetCreateDesc dsDesc = {};
		dsDesc.memoryProperties = MEMORY_PROPERTY_DEVICE_LOCAL;
		dsDesc.format = TEXTURE_FORMAT_D32_FLOAT;
		dsDesc.width = gameWindowSize.x;
		dsDesc.height = gameWindowSize.y;
		dsDesc.sampleCount = 1;
		dsDesc.sampleQuality = 0;
		dsDesc.tiling = IMAGE_TILING_OPTIMAL;
		dsDesc.usage = RENDER_TARGET_USAGE_DEPTH_STENCIL_ATTACHMENT;

		renderer->createRenderTarget(&gDepthStencil, &dsDesc);
	}

    void RGameWindow::OnClose()
    {
		renderer->waitForDevice();

		renderer->destroyQueue(gGraphicsQueue);
		renderer->destroyCommandLists(gCommandPool, &gCommandLists[0], 3);
		renderer->destroyCommandPool(gCommandPool);

		renderer->destroyBuffer(gIndexBuffer);
		renderer->destroyBuffer(gVertexBuffer);
		renderer->destroyTexture(gTexture);
		renderer->destroyShader(gShader);
		renderer->destroySampler(gSampler);
		renderer->destroyRootSignature(gRootSignature);
		renderer->destroyPipeline(gRenderPipeline);

		if (gDepthStencil != nullptr)
		{
			renderer->destroyRenderTarget(gDepthStencil);
		}

		if (gBackbuffer != nullptr)
		{
			renderer->destroyRenderTarget(gBackbuffer);
		}

		for (uint32_t i = 0; i < 3; ++i)
		{
			renderer->destroyBuffer(gUniformBuffers[i]);
			renderer->destroyFence(gRenderCompleteFences[i]);
			renderer->destroySemaphore(gRenderCompleteSemaphores[i]);
		}

		renderer->destroySemaphore(gImageAquiredSemaphore);

		gUIRenderer.destroy();

        // Call super.
		RRenderableWindow::OnClose();
    }

    void RGameWindow::Render()
    {
		gUIRenderer.newFrame(_SwapChain);
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(_SwapChain->getWidth(), _SwapChain->getHeight()));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		if (ImGui::Begin("Editor", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav))
		{
			ImGui::PopStyleVar(2);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Windows"))
				{
					ImGui::MenuItem("Game", NULL, &d_gameWindowOpen);
					ImGui::MenuItem("Console", NULL, &d_consoleWindowOpen);
					ImGui::MenuItem("Properties", NULL, &d_propertyWindowOpen);
					ImGui::MenuItem("Profiler", NULL, nullptr);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			// dock layout by hard-coded or .ini file
			ImGui::BeginDockspace();

			if (ImGui::BeginDock("Game", &d_gameWindowOpen)) {
				gameWindowPos = ImGui::GetCursorPos();


				if ((gameWindowSize.x != ImGui::GetContentRegionAvail().x || gameWindowSize.y != ImGui::GetContentRegionAvail().y) &&
					ImGui::GetContentRegionAvail().x > 1 && ImGui::GetContentRegionAvail().y > 1)
				{
					gameWindowSize = ImGui::GetContentRegionAvail();
					createGameWindow();
				}

				ImGui::Image(gBackbuffer->GetTexture(), ImGui::GetContentRegionAvail());
			}
			ImGui::EndDock();

			// Console.
			if (ImGui::BeginDock("Console", &d_consoleWindowOpen)) 
			{
				static ImGuiTextFilter Filter;

				if (ImGui::Button("Clear"))
				{ 
					//clear
				}
				ImGui::SameLine();
				Filter.Draw("Filter", -40.0f);
				ImGui::Separator();
				ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
				
				if (Filter.IsActive())
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
					ImGui::TextUnformatted("Hi log", 0);
					ImGui::PopStyleColor();
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
					ImGui::TextUnformatted("Hi log", 0);
					ImGui::PopStyleColor();
				}

				ImGui::EndChild();
			}
			ImGui::EndDock();

			if (ImGui::BeginDock("Properties", &d_propertyWindowOpen)) {
				ImGui::Text("I'm LonelyWaiting!");
			}
			ImGui::EndDock();

			ImGui::EndDockspace();
		}
		else
		{
			ImGui::PopStyleVar(2);
		}
		
		ImGui::End();
		ImGui::ShowDemoWindow();

		// update uniforms.
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() / 10.0f;

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
		gUniformBuffer.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		gUniformBuffer.view = glm::lookAt(glm::vec3(5.0f, 7.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		gUniformBuffer.proj = glm::perspective(glm::radians(45.0f), gameWindowSize.x / gameWindowSize.y, 0.1f, 20.0f);
		gUniformBuffer.proj[1][1] *= -1;

		renderer->cmdUpdateBuffer(cmd, gUniformBuffers[gCurrentImage], &gUniformBuffer, 0, sizeof(UniformBufferObject));

		FBufferBarrier uBarrier;
		uBarrier.buffer = gUniformBuffers[gCurrentImage];
		uBarrier.requiredState = RESOURCE_STATE_UNIFORM_BUFFER;

		FTextureBarrier tBarriers[4];
		tBarriers[0].requiredState = RESOURCE_STATE_RENDER_TARGET;
		tBarriers[0].texture = pRenderTarget->GetTexture();

		tBarriers[1].requiredState = RESOURCE_STATE_RENDER_TARGET;
		tBarriers[1].texture = gBackbuffer->GetTexture();

		tBarriers[2].requiredState = RESOURCE_STATE_DEPTH_WRITE;
		tBarriers[2].texture = gDepthStencil->GetTexture();

		tBarriers[3].requiredState = RESOURCE_STATE_SHADER_RESOURCE;
		tBarriers[3].texture = gTexture;

		renderer->cmdResourceBarriers(cmd, 1, &uBarrier, 4, &tBarriers[0], true);

		if (d_gameWindowOpen)
		{
			FLoadStoreOps loadStoreOps = {};
			loadStoreOps.colorLoadOps[0] = LOAD_OP_CLEAR;
			loadStoreOps.colorStoreOps[0] = STORE_OP_STORE;
			loadStoreOps.colorClearValues[0][0] = 0.0f;
			loadStoreOps.colorClearValues[0][1] = 0.0f;
			loadStoreOps.colorClearValues[0][2] = 0.0f;
			loadStoreOps.colorClearValues[0][3] = 1.0f;
			loadStoreOps.depthLoadOp = LOAD_OP_CLEAR;
			loadStoreOps.depthStoreOp = STORE_OP_STORE;
			loadStoreOps.depthClearValue = 1.0f;
			
			// Begin rendering pass.
			renderer->cmdBeginRenderPass(cmd, 1, &gBackbuffer, gDepthStencil, &loadStoreOps);
			
			// Bind graphics pipeline.
			renderer->cmdBindPipeline(cmd, gRenderPipeline);
			renderer->cmdSetViewport(cmd, Math::FRect({ gBackbuffer->GetDesc()->width,  gBackbuffer->GetDesc()->height }, { 0, 0 }));
			renderer->cmdSetScissorRects(cmd, Math::FRect({ gBackbuffer->GetDesc()->width,  gBackbuffer->GetDesc()->height }, { 0, 0 }));
			renderer->cmdBindIndexBuffer(cmd, gIndexBuffer);
			renderer->cmdBindVertexBuffer(cmd, 1, &gVertexBuffer);
			
			FDescriptorBinding bindings[2];
			bindings[0].set = 0;
			bindings[0].binding = 0;
			bindings[0].numDescriptors = 1;
			bindings[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			bindings[0].ppBuffers = &gUniformBuffers[gCurrentImage];
			
			bindings[1].set = 1;
			bindings[1].binding = 0;
			bindings[1].numDescriptors = 1;
			bindings[1].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[1].ppTextures = &gTexture;
			
			// Bind uniform buffer.
			renderer->cmdBindDescriptors(cmd, 2, &bindings[0]);
			renderer->cmdDrawIndexed(cmd, (uint32_t)gIndices.size(), 0, 0);
			
			// Finish rendering pass.
			renderer->cmdEndRenderPass(cmd);
		}

		// Draw UI
		gUIRenderer.render(cmd, pRenderTarget);

		FTextureBarrier rtBarrier;
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