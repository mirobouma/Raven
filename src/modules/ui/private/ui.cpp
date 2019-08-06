#include "ui/ui.h"
#include "core/debug/logmanager.h"
#include <chrono>

#ifdef PLATFORM_LINUX
#include "SDL_keycode.h"
#endif

#include "hal/platform.h"
#include "hal/platform_window.h"

namespace Raven
{
	void RUIRenderer::initialize(IRenderInterface* renderer, IPlatformWindow* window, ISwapChain* swapChain)
	{
		RASSERT(renderer != nullptr);

		if (!renderer->isInitialized())
		{
			RLOG_WARNING("Failed to initialize UI renderer because the renderer implementation is not initialized yet.");
			return;
		}

		mWindow = window;
		mCurrentFrameId = 0;
		mRenderer = renderer;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); 

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;   

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

#ifdef PLATFORM_WINDOWS
		io.KeyMap[ImGuiKey_Tab] = VK_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
		io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
		io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
		io.KeyMap[ImGuiKey_Home] = VK_HOME;
		io.KeyMap[ImGuiKey_End] = VK_END;
		io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
		io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
		io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
		io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
		io.KeyMap[ImGuiKey_Space] = VK_SPACE;
		io.KeyMap[ImGuiKey_A] = 'A';
		io.KeyMap[ImGuiKey_C] = 'C';
		io.KeyMap[ImGuiKey_V] = 'V';
		io.KeyMap[ImGuiKey_X] = 'X';
		io.KeyMap[ImGuiKey_Y] = 'Y';
		io.KeyMap[ImGuiKey_Z] = 'Z';
		
		ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		main_viewport->PlatformHandle = (void*)mWindow->GetNativeHandle();

		//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			//ImGui_ImplWin32_InitPlatformInterface();
#else
		io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
		io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
		io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
		io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
		io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
		io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
		io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
		io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
		io.KeyMap[ImGuiKey_A] = 'A';
		io.KeyMap[ImGuiKey_C] = 'C';
		io.KeyMap[ImGuiKey_V] = 'V';
		io.KeyMap[ImGuiKey_X] = 'X';
		io.KeyMap[ImGuiKey_Y] = 'Y';
		io.KeyMap[ImGuiKey_Z] = 'Z';
#endif
		// Create shaders.
		FShaderCreateDesc shaderDesc;
		shaderDesc.shaders[0].path = "shaders/imgui.vert.spv";
		shaderDesc.shaders[0].stage = SHADER_STAGE_VERTEX;
		shaderDesc.shaders[1].path = "shaders/imgui.frag.spv";
		shaderDesc.shaders[1].stage = SHADER_STAGE_FRAGMENT;

		renderer->createShader(&mShader, &shaderDesc);

		// Create font sampler.
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

		renderer->createSampler(&mFontSampler, &samplerDesc);

		// Create input assembler layout
		FInputAttributeDescription iaAttributes[3];
		iaAttributes[0].binding = 0;
		iaAttributes[0].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[0].location = 0;
		iaAttributes[0].offset = 0;

		iaAttributes[1].binding = 0;
		iaAttributes[1].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[1].location = 1;
		iaAttributes[1].offset = sizeof(glm::vec2);

		iaAttributes[2].binding = 0;
		iaAttributes[2].format = TEXTURE_FORMAT_RGBA8;
		iaAttributes[2].location = 2;
		iaAttributes[2].offset = sizeof(glm::vec2) * 2;

		// Create input assembler binding
		FInputBindingDescription iaBinding;
		iaBinding.binding = 0;
		iaBinding.inputRate = EVertexInputRate::VERTEX_INPUT_RATE_VERTEX;
		iaBinding.stride = (sizeof(glm::vec2) * 2) + sizeof(float);

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
		bindings[0].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings[0].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[1].set = 0;
		bindings[1].binding = 1;
		bindings[1].descriptorCount = 1;
		bindings[1].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings[1].ppImmutableSamplers = &mFontSampler;
		bindings[1].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		// Create root signature.
		FRootSignatureCreateDesc rsDesc;
		rsDesc.pShaderProgram = mShader;
		rsDesc.pInputAssemblerLayout = &iaLayout;
		rsDesc.numDescriptorBindings = 2;
		rsDesc.pDescriptorBindings = &bindings[0];

		renderer->createRootSignature(&mRootSignature, &rsDesc);

		IRenderTarget* target = swapChain->getRenderTarget(swapChain->getCurrentImageIndex());

		mBlendState = {};
		mBlendState.logicOpEnable = false;
		mBlendState.renderTargetBlendStates[0].blendEnable = true;
		mBlendState.renderTargetBlendStates[0].srcColorBlendFactor = BLEND_FACTOR_SRC_ALPHA;
		mBlendState.renderTargetBlendStates[0].dstColorBlendFactor = BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		mBlendState.renderTargetBlendStates[0].colorBlendOp = BLEND_OP_ADD;
		mBlendState.renderTargetBlendStates[0].srcAlphaBlendFactor = BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		mBlendState.renderTargetBlendStates[0].dstAlphaBlendFactor = BLEND_FACTOR_ZERO;
		mBlendState.renderTargetBlendStates[0].alphaBlendOp = BLEND_OP_ADD;

		mDepthStencilState = {};
		mDepthStencilState.depthTestEnable = false;
		mDepthStencilState.depthWriteEnable = false;
		mDepthStencilState.depthCompareOp = ECompareOp::COMPARE_OP_NEVER;

		mRasterizerState = {};
		mRasterizerState.cullMode = CULL_MODE_NONE;
		mRasterizerState.depthBiasEnable = false;
		mRasterizerState.polygonMode = POLYGON_MODE_FILL;

		FPushConstantRange pushRange;
		pushRange.stageFlags = SHADER_STAGE_FLAG_VERTEX;
		pushRange.offset = 0;
		pushRange.size = sizeof(float) * 4;

		// Create graphics pipeline.
		FGraphicsPipelineCreateDesc pipelineDesc;
		pipelineDesc.pRootSignature = mRootSignature;
		pipelineDesc.pShader = mShader;
		pipelineDesc.pBlendState = &mBlendState;
		pipelineDesc.pDepthStencilState = &mDepthStencilState;
		pipelineDesc.pRasterizerState = &mRasterizerState;
		pipelineDesc.depthStencilFormat = TEXTURE_FORMAT_UNKNOWN;
		pipelineDesc.numPushConstantRanges = 1;
		pipelineDesc.pPushConstantRanges = &pushRange;
		pipelineDesc.numRenderTargets = 1;
		pipelineDesc.renderTargetFormats[0] = target->GetDesc()->format;
		pipelineDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE;
		pipelineDesc.sampleCount = target->GetDesc()->sampleCount;
		pipelineDesc.sampleQuality = target->GetDesc()->sampleQuality;

		renderer->createPipeline(&mPipeline, &pipelineDesc);

		// Load roboto font.
		io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 18);

		// Create font image.
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		size_t upload_size = width * height * 4 * sizeof(char);

		FImageCreateDesc fontDesc = {};
		fontDesc.type = IMAGE_TYPE_2D;
		fontDesc.memoryType = MEMORY_TYPE_GPU;
		fontDesc.arrayLayers = 1;
		fontDesc.baseArrayLayer = 0;
		fontDesc.baseMipLevel = 0;
		fontDesc.depth = 1;
		fontDesc.format = TEXTURE_FORMAT_RGBA8;
		fontDesc.width = width;
		fontDesc.height = height;
		fontDesc.mipLevels = 1;
		fontDesc.pData = pixels;
		fontDesc.sampleCount = 1;
		fontDesc.sampleQuality = 0;
		fontDesc.tiling = IMAGE_TILING_OPTIMAL;
		fontDesc.usage = IMAGE_USAGE_SAMPLED | IMAGE_USAGE_TRANSFER_DST;

		renderer->createImage(&mFontTexture, &fontDesc);
		io.Fonts->TexID = (void *)mFontTexture;

		mInitialized = true;
	}

	void RUIRenderer::newFrame(ISwapChain* pSwapChain)
	{
		ImGuiIO& io = ImGui::GetIO();

		// Setup display size (every frame to accommodate for window resizing)
		io.DisplaySize = ImVec2((float)pSwapChain->getWidth(), (float)pSwapChain->getHeight());
		io.DisplayFramebufferScale = ImVec2(1, 1);

		// Setup time step
		static auto startTime = std::chrono::high_resolution_clock::now();
		static auto currentTime = std::chrono::high_resolution_clock::now();
		static auto prevTime = std::chrono::high_resolution_clock::now();
		prevTime = currentTime;
		currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - prevTime).count() / 1.0f;

		io.DeltaTime = time;

		// Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
		ImGui::NewFrame();
	}

	FVirtualResourceRef* RUIRenderer::render(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* pTarget)
	{
		ImGui::Render();

		FVirtualResourceRef* output = pTarget;

		// Add the pass and setup its inputs and outputs.
		auto pass = pBuilder->add_pass("draw_meshes_forward", [&](RRenderPassBuilder& builder)
		{
			if (pTarget != nullptr)
			{
				output = builder.write_resource("backbuffer", pTarget, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT);
			}
		});

		// Set execute function.
		pass->setExecuteFunc([=](IRenderInterface* renderer, RRenderGraph& graph, RCompiledPass& pass, ICommandList* cmd)
		{
			ImDrawData* draw_data = ImGui::GetDrawData();

			ImGuiIO& io = ImGui::GetIO();
			if (draw_data->TotalVtxCount == 0)
				return;

			std::vector<ImDrawVert> vertices(draw_data->TotalVtxCount);
			std::vector<ImDrawIdx> indices(draw_data->TotalIdxCount);

			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;

			for (int i = 0; i < draw_data->CmdListsCount; ++i)
			{
				memcpy(&vertices[vertexCount], draw_data->CmdLists[i]->VtxBuffer.Data, draw_data->CmdLists[i]->VtxBuffer.Size * sizeof(ImDrawVert));
				memcpy(&indices[indexCount], draw_data->CmdLists[i]->IdxBuffer.Data, draw_data->CmdLists[i]->IdxBuffer.Size * sizeof(ImDrawIdx));

				vertexCount += draw_data->CmdLists[i]->VtxBuffer.Size;
				indexCount += draw_data->CmdLists[i]->IdxBuffer.Size;
			}

			// Update or create the vertex buffer.
			size_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
			if (!mVertexBuffers[mCurrentFrameId] || mVertexBuffers[mCurrentFrameId]->getSize() < vertex_size)
			{
				if (mVertexBuffers[mCurrentFrameId])
				{
					mRenderer->destroyBuffer(mVertexBuffers[mCurrentFrameId]);
					mVertexBuffers[mCurrentFrameId] = nullptr;
				}

				FBufferCreateDesc vertexDesc = {};
				vertexDesc.pData = nullptr;
				vertexDesc.size = vertex_size;
				vertexDesc.memoryType = MEMORY_TYPE_CPU;
				vertexDesc.usage = BUFFER_USAGE_VERTEX_BUFFER | BUFFER_USAGE_TRANSFER_DST;

				mRenderer->createBuffer(&mVertexBuffers[mCurrentFrameId], &vertexDesc);
			}

			// Update or create the index buffer.
			size_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
			if (!mIndexBuffers[mCurrentFrameId] || mIndexBuffers[mCurrentFrameId]->getSize() < index_size)
			{
				if (mIndexBuffers[mCurrentFrameId])
				{
					mRenderer->destroyBuffer(mIndexBuffers[mCurrentFrameId]);
					mIndexBuffers[mCurrentFrameId] = nullptr;
				}

				FBufferCreateDesc indexDesc = {};
				indexDesc.pData = nullptr;
				indexDesc.size = index_size;
				indexDesc.memoryType = MEMORY_TYPE_CPU;
				indexDesc.usage = BUFFER_USAGE_INDEX_BUFFER | BUFFER_USAGE_TRANSFER_DST;

				mRenderer->createBuffer(&mIndexBuffers[mCurrentFrameId], &indexDesc);
			}

			mRenderer->cmdUpdateBuffer(cmd, mVertexBuffers[mCurrentFrameId], vertices.data(), 0, (uint32_t)vertex_size);
			mRenderer->cmdUpdateBuffer(cmd, mIndexBuffers[mCurrentFrameId], indices.data(), 0, (uint32_t)index_size);

			FBufferBarrier bBarriers[2];
			bBarriers[0].requiredState = RESOURCE_STATE_INDEX_BUFFER;
			bBarriers[0].buffer = mIndexBuffers[mCurrentFrameId];

			bBarriers[1].requiredState = RESOURCE_STATE_VERTEX_BUFFER;
			bBarriers[1].buffer = mVertexBuffers[mCurrentFrameId];

			mRenderer->cmdResourceBarriers(cmd, 2, &bBarriers[0], 0, nullptr, true);

			FLoadStoreOps loadStoreOps = {};
			loadStoreOps.colorLoadOps[0] = LOAD_OP_CLEAR;
			loadStoreOps.colorStoreOps[0] = STORE_OP_STORE;

			IRenderTarget* color = pass.getRenderTargetResource("backbuffer");

			// Begin rendering pass.
			mRenderer->cmdBeginRenderPass(cmd, 1, &color, nullptr, &loadStoreOps);

			// Bind graphics pipeline.
			mRenderer->cmdBindPipeline(cmd, mPipeline);
			mRenderer->cmdSetViewport(cmd, FRect({ color->GetDesc()->width,  color->GetDesc()->height }, { 0, 0 }));

			mRenderer->cmdBindIndexBuffer(cmd, mIndexBuffers[mCurrentFrameId], INDEX_TYPE_UINT16);
			mRenderer->cmdBindVertexBuffer(cmd, 1, &mVertexBuffers[mCurrentFrameId]);

			float scale[2];
			scale[0] = 2.0f / io.DisplaySize.x;
			scale[1] = 2.0f / io.DisplaySize.y;
			float translate[2];
			translate[0] = -1.0f;
			translate[1] = -1.0f;

			mRenderer->cmdPushConstant(cmd, SHADER_STAGE_FLAG_VERTEX, sizeof(float) * 2, sizeof(float) * 0, scale);
			mRenderer->cmdPushConstant(cmd, SHADER_STAGE_FLAG_VERTEX, sizeof(float) * 2, sizeof(float) * 2, translate);

			IImage* imageToDraw = nullptr;

			//Render the command lists:
			int vtx_offset = 0;
			int idx_offset = 0;
			for (int n = 0; n < draw_data->CmdListsCount; n++)
			{
				const ImDrawList* cmd_list = draw_data->CmdLists[n];
				for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
				{
					const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
					if (pcmd->UserCallback)
					{
						pcmd->UserCallback(cmd_list, pcmd);
					}
					else
					{
						if (pcmd->TextureId != nullptr && pcmd->TextureId != imageToDraw)
						{
							FDescriptorBinding binding;
							binding.set = 0;
							binding.binding = 0;
							binding.numDescriptors = 1;
							binding.type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
							binding.ppImages = (IImage**)&pcmd->TextureId;

							// Bind font image.
							mRenderer->cmdBindDescriptors(cmd, 1, &binding);

							imageToDraw = (IImage*)pcmd->TextureId;
						}

						FRect scissor;
						scissor.Position.x = (int32_t)(pcmd->ClipRect.x) > 0 ? (int32_t)(pcmd->ClipRect.x) : 0;
						scissor.Position.y = (int32_t)(pcmd->ClipRect.y) > 0 ? (int32_t)(pcmd->ClipRect.y) : 0;
						scissor.Size.x = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
						scissor.Size.y = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y + 1); // FIXME: Why +1 here?

						mRenderer->cmdSetScissorRects(cmd, scissor);
						mRenderer->cmdDrawIndexed(cmd, pcmd->ElemCount, idx_offset, vtx_offset);
					}
					idx_offset += pcmd->ElemCount;
				}
				vtx_offset += cmd_list->VtxBuffer.Size;
			}

			// Finish rendering pass.
			mRenderer->cmdEndRenderPass(cmd);

		});

		mCurrentFrameId = (mCurrentFrameId + 1) % MAX_UI_FRAMES;

		ImGuiIO& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		return output;
	}

	void RUIRenderer::destroy()
	{
		RASSERT(mRenderer != nullptr);
		RASSERT(mInitialized);

		if (!mRenderer->isInitialized())
		{
			RLOG_WARNING("Failed to destroy UI renderer because the renderer implementation is not initialized yet.");
			return;
		}

		ImGui::DestroyContext();

		mRenderer->destroyPipeline(mPipeline);
		mRenderer->destroyRootSignature(mRootSignature);
		mRenderer->destroyShader(mShader);
		mRenderer->destroyImage(mFontTexture);
		mRenderer->destroySampler(mFontSampler);

		for (uint32_t i = 0; i < MAX_UI_FRAMES; ++i)
		{
			if (mVertexBuffers[i])
			{
				mRenderer->destroyBuffer(mVertexBuffers[i]);
			}

			if (mIndexBuffers[i])
			{
				mRenderer->destroyBuffer(mIndexBuffers[i]);
			}
		}

		mInitialized = false;
	}
}
