#include "GameWindow.h"
#include "Engine/Globals.h"
#include "Engine/Engine.h"
#include "Core/Debug/LogManager.h"
#include "Core/Debug/Profiler.h"
#include "Rendering/RenderGraph/RenderGraph.h"
#include "Rendering/RenderGraph/Modules/RGMCommon.h"
#include "Rendering/RenderGraph/Modules/RGMMesh.h"
#include "Rendering/RenderGraph/Modules/RGMSSAO.h"
#include "Rendering/RenderGraph/Modules/RGMVolumetric.h"

#include "Rendering/Game/Mesh.h"

#include <algorithm>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp> // logger
#include <assimp/DefaultLogger.hpp> // logger
#include <chrono>
#include "UI/UI.h"
#undef max

namespace Raven
{
	RRenderGraphManager* gRenderGraphManager = nullptr;
	IQueue* gGraphicsQueue = nullptr;
	IQueue* gComputeQueue = nullptr;

	IPipeline* gRenderPipeline = nullptr;
	IRootSignature* gRootSignature = nullptr;
	IShader* gShader = nullptr;
	IRenderTarget* gDepthStencil = nullptr;
	ISampler* gSampler = nullptr;
	IRenderTarget* gBackbuffer = nullptr;
	RMesh* gRealMesh;

	RGMCommon gRGMCommon;
	RGMMesh gRGMMesh;
	RGMSSAO gRGMSSAO;
	RGMVolumetricFog gRGMVolumetric;

	RCamera gCamera;
	RCamera gShadowCamera;

	RUIRenderer	gUIRenderer;

	FSSAOVars gSSAOVars;
	FVolumetricFogVars gVolumetricVars;

	ImVec2 gameWindowSize = {1, 1};
	ImVec2 gameWindowBackbufferSize = { 1, 1 };
	ImVec2 gameWindowPos = {0, 0};
	float gProfilerBlockHeight = 40;
	float gProfilerZoom = 1.0f;

	bool bKeepAspectRatio = true;
	float gAspectWidth = 16.0f;
	float gAspectHeight = 9.0f;

	float gLightRotX = 80.0f;
	float gLightRotY = 40.0f;

	bool d_demoWindowOpen = false;
	bool d_gameWindowOpen = true;
	bool d_consoleWindowOpen = true;
	bool d_propertyWindowOpen = true;
	bool d_profilerWindowOpen = true;

    RGameWindow::RGameWindow(const char* title, FVector2 size, FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler)
            : RRenderableWindow(title, size, position, flags, handler)
    {

    }

    void RGameWindow::OnCreate()
    {
		GLogManager->RegisterLogger(&mLogger);

        // Call super.
		RRenderableWindow::OnCreate();
		renderer = GEngine->GetRenderer();

		// Retrieve graphics queue.
		FQueueCreateDesc queueDesc;
		queueDesc.type = ECommandType::COMMAND_TYPE_DIRECT;
		renderer->createQueue(&gGraphicsQueue, &queueDesc);

		// Retrieve compute queue.
		FQueueCreateDesc queueDesc2;
		queueDesc2.type = ECommandType::COMMAND_TYPE_COMPUTE;
		renderer->createQueue(&gComputeQueue, &queueDesc2);

		// Create rneder graph manager.
		gRenderGraphManager = new RRenderGraphManager();
		gRenderGraphManager->initialize(renderer, gGraphicsQueue, gComputeQueue);

		// Create Render Graph Modules.
		gRGMCommon.Init(renderer);
		gRGMCommon.Load(renderer, _SwapChain);
		gRGMMesh.Init(renderer);
		gRGMMesh.Load(renderer, _SwapChain);
		gRGMSSAO.Init(renderer);
		gRGMSSAO.Load(renderer, _SwapChain);
		gRGMVolumetric.Init(renderer);
		gRGMVolumetric.Load(renderer, _SwapChain);
		
		// Create game backbuffer.
		createGameWindow();

		// Initialize UI renderer.
		gUIRenderer.initialize(renderer, this, _SwapChain);

		gRealMesh = new RMesh(renderer, "models/sponzaPBR/sponza.obj", 0.05f);

		gSSAOVars.cSampleRadius = 2.4f;
		gSSAOVars.cIntensity = 1.1f;
		gSSAOVars.cScale = 1.0f;
		gSSAOVars.cBias = 0.1f;

		gVolumetricVars.EnableHeightFog = false;
		gVolumetricVars.LightCol = glm::vec4(0.91, 0.61f, 0.35f, 1.0f);
		gVolumetricVars.AmbientFog = 0.2f;
		gVolumetricVars.LightMieG = 0.05f;
		gVolumetricVars.LightIntensity = 1.0f;
		gVolumetricVars.LightScatteringCoef = 0.020f;
		gVolumetricVars.LightExtinctionCoef = 0.0005f;
		gVolumetricVars.SkyboxExtinctionCoef = 0.005f;
		gVolumetricVars.NumSteps = 25;
		gVolumetricVars.MaxRayLength = 1000.0f;
		gVolumetricVars.EnableHeightFog = false;
		gVolumetricVars.HeightFog = 1.0f;
		gVolumetricVars.HeightFogScale = 0.007f;
		gVolumetricVars.NoiseSpeed = 1.0f;
		gVolumetricVars.FogDensity = 1.0f;
		gVolumetricVars.EnableNoise = false;

    }

    void RGameWindow::OnResize(int width, int height)
    {
        // Call super.
		RRenderableWindow::OnResize(width, height);
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
		renderer->waitForDevice();

		if (gameWindowBackbufferSize.x == 0 || gameWindowBackbufferSize.y == 0)
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
		texDesc.memoryType = MEMORY_TYPE_GPU;
		texDesc.format = TEXTURE_FORMAT_RGBA8;
		texDesc.width = (uint32_t)std::max((int32_t)gameWindowBackbufferSize.x, (int32_t)1);
		texDesc.height = (uint32_t)std::max((int32_t)gameWindowBackbufferSize.y, (int32_t)1);
		texDesc.sampleCount = 1;
		texDesc.sampleQuality = 0;
		texDesc.tiling = IMAGE_TILING_OPTIMAL;
		texDesc.usage = FRenderTargetUsage::RENDER_TARGET_USAGE_COLOR_ATTACHMENT;
			
		renderer->createRenderTarget(&gBackbuffer, &texDesc);


		FRenderTargetCreateDesc dsDesc = {};
		dsDesc.memoryType = MEMORY_TYPE_GPU;
		dsDesc.format = TEXTURE_FORMAT_D32_FLOAT;
		dsDesc.width = (uint32_t)std::max((int32_t)gameWindowBackbufferSize.x, (int32_t)1);
		dsDesc.height = (uint32_t)std::max((int32_t)gameWindowBackbufferSize.y, (int32_t)1);
		dsDesc.sampleCount = 1;
		dsDesc.sampleQuality = 0;
		dsDesc.tiling = IMAGE_TILING_OPTIMAL;
		dsDesc.usage = RENDER_TARGET_USAGE_DEPTH_STENCIL_ATTACHMENT;

		renderer->createRenderTarget(&gDepthStencil, &dsDesc);

		//renderer->waitForDevice();
	}

    void RGameWindow::OnClose()
    {
		renderer->waitForDevice();

		renderer->destroyQueue(gGraphicsQueue);
		renderer->destroyQueue(gComputeQueue);

		gRGMCommon.Unload(renderer);
		gRGMMesh.Unload(renderer);
		gRGMSSAO.Unload(renderer);
		gRGMVolumetric.Unload(renderer);

		gRGMCommon.Destroy(renderer);
		gRGMMesh.Destroy(renderer);
		gRGMSSAO.Destroy(renderer);
		gRGMVolumetric.Destroy(renderer);

		gRenderGraphManager->destroy();
		delete gRenderGraphManager;

		delete gRealMesh;

		if (gDepthStencil != nullptr)
		{
			renderer->destroyRenderTarget(gDepthStencil);
		}

		if (gBackbuffer != nullptr)
		{
			renderer->destroyRenderTarget(gBackbuffer);
		}
	
		gUIRenderer.destroy();

        // Call super.
		RRenderableWindow::OnClose();

		GLogManager->DeregisterLogger(&mLogger);
    }

	void RGameWindow::updateEditor()
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
					ImGui::MenuItem("Profiler", NULL, &d_profilerWindowOpen);
					ImGui::MenuItem("ImGui Demo", NULL, &d_demoWindowOpen);

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			// dock layout by hard-coded or .ini file
			ImGui::BeginDockspace();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
			if (ImGui::BeginDock("Game", &d_gameWindowOpen, ImGuiWindowFlags_MenuBar))
			{
				ImVec4 Col = ImGui::GetStyleColorVec4(ImGuiCol_TitleBgActive);
				ImGui::PushStyleColor(ImGuiCol_MenuBarBg, Col);
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("Display Options"))
					{
						if (ImGui::MenuItem("Retain Aspect Ratio", NULL, &bKeepAspectRatio))
						{
							if (bKeepAspectRatio)
							{
								float aspect = gameWindowSize.x / gameWindowSize.y;
								float reqAspect = gAspectWidth / gAspectHeight;
								float minSize = ImGui::IsWindowFocused() ? 10.0f : 0.0f;

								if (aspect > reqAspect)
									gameWindowBackbufferSize = ImVec2((ImGui::GetContentRegionAvail().x * ((gAspectWidth / gAspectHeight) / (ImGui::GetContentRegionAvail().x / ImGui::GetContentRegionAvail().y))) - minSize, ImGui::GetContentRegionAvail().y - minSize);
								else if (aspect < reqAspect)
									gameWindowBackbufferSize = ImVec2(ImGui::GetContentRegionAvail().x - minSize, (ImGui::GetContentRegionAvail().y * ((gAspectHeight / gAspectWidth) / (ImGui::GetContentRegionAvail().y / ImGui::GetContentRegionAvail().x))) - minSize);
								else
									gameWindowBackbufferSize = ImGui::GetContentRegionAvail();

							}
							else
							{
								if (ImGui::IsWindowFocused())
								{
									gameWindowBackbufferSize = ImVec2(ImGui::GetContentRegionAvail().x - 10.0f, ImGui::GetContentRegionAvail().y - 10.0f);
								}
								else
								{
									gameWindowBackbufferSize = ImGui::GetContentRegionAvail();
								}
							}

							createGameWindow();
						}

						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}
				ImGui::PopStyleColor();

				ImVec2 ContentSizeAvailable;
				if (ImGui::IsWindowFocused())
				{
					ContentSizeAvailable = ImVec2(ImGui::GetContentRegionAvail().x - 10.0f, ImGui::GetContentRegionAvail().y - 10.0f);
				}
				else
				{
					ContentSizeAvailable = ImGui::GetContentRegionAvail();
				}

				if ((gameWindowSize.x != ContentSizeAvailable.x || gameWindowSize.y != ContentSizeAvailable.y) &&
					ContentSizeAvailable.x > 1 && ContentSizeAvailable.y > 1)
				{
					gameWindowSize = ContentSizeAvailable;

					if (bKeepAspectRatio)
					{
						float aspect = gameWindowSize.x / gameWindowSize.y;
						float reqAspect = gAspectWidth / gAspectHeight;
						float minSize = ImGui::IsWindowFocused() ? 10.0f : 0.0f;

						if (aspect > reqAspect)
							gameWindowBackbufferSize = ImVec2((ImGui::GetContentRegionAvail().x * ((gAspectWidth / gAspectHeight) / (ImGui::GetContentRegionAvail().x / ImGui::GetContentRegionAvail().y))) - minSize, ImGui::GetContentRegionAvail().y - minSize);
						else if (aspect < reqAspect)
							gameWindowBackbufferSize = ImVec2(ImGui::GetContentRegionAvail().x - minSize, (ImGui::GetContentRegionAvail().y * ((gAspectHeight / gAspectWidth) / (ImGui::GetContentRegionAvail().y / ImGui::GetContentRegionAvail().x))) - minSize);
						else
							gameWindowBackbufferSize = ImGui::GetContentRegionAvail();

					}
					else
					{
						if (ImGui::IsWindowFocused())
						{
							gameWindowBackbufferSize = ImVec2(ImGui::GetContentRegionAvail().x - 10.0f, ImGui::GetContentRegionAvail().y - 10.0f);
						}
						else
						{
							gameWindowBackbufferSize = ImGui::GetContentRegionAvail();
						}
					}

					createGameWindow();
				}

				if (bKeepAspectRatio)
				{
					ImVec2 padding = ImVec2((ImGui::GetContentRegionAvail().x - gameWindowBackbufferSize.x - (ImGui::IsWindowFocused() ? 10.0f : 0.0f)) / 2.0f, (ImGui::GetContentRegionAvail().y - gameWindowBackbufferSize.y - (ImGui::IsWindowFocused() ? 10.0f : 0.0f)) / 2.0f);
					ImVec2 currentCursorPos = ImGui::GetCursorPos();
					ImGui::SetCursorPos(ImVec2(currentCursorPos.x + padding.x, currentCursorPos.y + padding.y));
				}
				if (ImGui::IsWindowFocused())
				{
					ImVec4 borderColor = ImVec4(1.0, 0.6f, 0.0f, 1.0f);

					ImGui::PushStyleColor(ImGuiCol_Button, borderColor);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, borderColor);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, borderColor);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

					ImGui::ImageButton(gBackbuffer->GetTexture(), ImVec2(gameWindowBackbufferSize.x, gameWindowBackbufferSize.y), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), 5, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

					ImGui::PopStyleVar(2);
					ImGui::PopStyleColor(3);
				}
				else
				{
					ImGui::Image(gBackbuffer->GetTexture(), gameWindowBackbufferSize);
				}
				if (ImGui::IsWindowFocused())
				{
					//ImGui::PopStyleVar();
				}

			}
			ImGui::EndDock();
			ImGui::PopStyleVar();


			// Console.
			if (ImGui::BeginDock("Console", &d_consoleWindowOpen))
			{
				static ImGuiTextFilter Filter;

				if (ImGui::Button("Clear"))
				{
					mLogger.mMessages.clear();
				}
				ImGui::SameLine();
				Filter.Draw("Filter", -40.0f);
				ImGui::Separator();
				ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

				if (Filter.IsActive())
				{

					for (uint32_t i = 0; i < mLogger.mMessages.size(); ++i)
					{

						if (Filter.PassFilter(mLogger.mMessages[i].c_str()))
						{
							ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

							if (strstr(mLogger.mMessages[i].c_str(), "[Error]"))
							{
								col = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
							}
							else if (strstr(mLogger.mMessages[i].c_str(), "[Warning]"))
							{
								col = ImVec4(1.0f, 0.78f, 0.58f, 1.0f);
							}

							ImGui::PushStyleColor(ImGuiCol_Text, col);
							ImGui::TextUnformatted(mLogger.mMessages[i].c_str(), 0);
							ImGui::PopStyleColor();
						}

					}

				}
				else
				{
					for (uint32_t i = 0; i < mLogger.mMessages.size(); ++i)
					{
						ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

						if (strstr(mLogger.mMessages[i].c_str(), "[Error]"))
						{
							col = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
						}
						else if (strstr(mLogger.mMessages[i].c_str(), "[Warning]"))
						{
							col = ImVec4(1.0f, 0.78f, 0.58f, 1.0f);
						}

						ImGui::PushStyleColor(ImGuiCol_Text, col);
						ImGui::TextUnformatted(mLogger.mMessages[i].c_str(), 0);
						ImGui::PopStyleColor();
					}
				}

				ImGui::EndChild();
			}
			ImGui::EndDock();

			if (ImGui::BeginDock("Properties", &d_propertyWindowOpen)) {
				ImGui::Text("SSAO");
				ImGui::Separator();
				ImGui::SliderFloat("Bias", &gSSAOVars.cBias, 0.001f, 0.3f);
				ImGui::SliderFloat("Intensity", &gSSAOVars.cIntensity, 0.0f, 5.0f);
				ImGui::SliderFloat("Sample Radius", &gSSAOVars.cSampleRadius, 0.01f, 5.0f);
				ImGui::Text("Light");
				ImGui::Separator();
				ImGui::SliderFloat("Rotation X", &gLightRotX, -360.0f, 360.0f);
				ImGui::SliderFloat("Rotation Y", &gLightRotY, -360.0f, 360.0f);
				ImGui::Text("Fog");
				ImGui::Separator();
				ImGui::SliderFloat("Fog Intensity", &gVolumetricVars.LightIntensity, 0.0f, 2.0f);
				ImGui::SliderInt("Steps", (int*)&gVolumetricVars.NumSteps, 1, 256);
				ImGui::SliderFloat("Max ray length", &gVolumetricVars.MaxRayLength, 1.0f, 1000.0f);
				ImGui::SliderFloat("Mie", &gVolumetricVars.LightMieG, 0.0f, 1.0f);
				ImGui::SliderFloat("Light extiction coefficient", &gVolumetricVars.LightExtinctionCoef, 0.0f, 0.02f);
				ImGui::SliderFloat("Light scattering coefficien", &gVolumetricVars.LightScatteringCoef, 0.0f, 0.02f);
				ImGui::SliderFloat("Sky extinction coefficient", &gVolumetricVars.SkyboxExtinctionCoef, 0.0f, 0.02f);
				ImGui::SliderFloat("Fog Density", &gVolumetricVars.FogDensity, 0.0f, 2.0f);
				ImGui::SliderFloat("Ambient Fog", &gVolumetricVars.AmbientFog, 0.0f, 1.0f);
				ImGui::ColorPicker4("Light Color", (float*)&gVolumetricVars.LightCol);
			}
			ImGui::EndDock();

			if (ImGui::BeginDock("Profiler", &d_profilerWindowOpen))
			{
				if (GProfiler == nullptr)
				{
					ImGui::Text("The profiler is not initialized.");
				}
				else
				{
					if (ImGui::Button("Capture"))
					{
						GProfiler->ProfileNextFrame();
					}
					ImGui::SameLine();
					ImGui::SliderFloat("Zoom", &gProfilerZoom, 1.0f, 50.0f);
					ImGui::Separator();

					if (GProfiler->mSavedThreadProfilers.size() != 0)
					{
						std::chrono::time_point<std::chrono::high_resolution_clock> beginTime;
						std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

						for (uint32_t i = 0; i < GProfiler->mSavedThreadProfilers.size(); ++i)
						{
							FThreadProfiler& prof = GProfiler->mSavedThreadProfilers[i];
							if (prof._startTime > beginTime)
								beginTime = prof._startTime;

							if (prof._endTime > endTime)
								endTime = prof._endTime;
						}

						float windowSize = ImGui::GetWindowSize().x - (ImGui::GetStyle().ScrollbarSize * 2.0f);

						float scale = windowSize / std::chrono::duration<float, std::chrono::seconds::period>(endTime - beginTime).count();

						ImGui::Text("Frame time %7.5f ms", std::chrono::duration<float, std::chrono::seconds::period>(endTime - beginTime).count() * 1000.0f);

						ImGui::Separator();
						ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

						for (uint32_t i = 0; i < GProfiler->mSavedThreadProfilers.size(); ++i)
						{
							ImGui::BeginGroup();
							ImGui::Text("Thread %i", GProfiler->mSavedThreadProfilers[i].threadId);
							ImGui::Separator();
							ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);

							ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
							ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2);
							ImGui::SameLine();

							FThreadProfiler& prof = GProfiler->mSavedThreadProfilers[i];
							ImGui::InvisibleButton("", ImVec2(gProfilerZoom * scale * std::chrono::duration<float, std::chrono::seconds::period>(beginTime - prof._startTime).count(), gProfilerBlockHeight));
							ImGui::SameLine();

							std::chrono::time_point<std::chrono::high_resolution_clock> prevEnd = prof._startTime;
							for (uint32_t j = 0; j < prof.data.size(); ++j)
							{
								ImGui::InvisibleButton("", ImVec2(gProfilerZoom* scale * std::chrono::duration<float, std::chrono::seconds::period>(prof.data[j].begin - prevEnd).count(), gProfilerBlockHeight));
								prevEnd = prof.data[j].end;
								ImGui::SameLine();
								ImGui::Button(prof.data[j].name.c_str(), ImVec2(gProfilerZoom* scale * std::chrono::duration<float, std::chrono::seconds::period>(prof.data[j].end - prof.data[j].begin).count(), gProfilerBlockHeight));
								if (ImGui::IsItemHovered())
								{
									ImGui::SetTooltip("%s\nDuration %7.5f ms", prof.data[j].name.c_str(), std::chrono::duration<float, std::chrono::seconds::period>(prof.data[j].end - prof.data[j].begin).count() * 1000);
								}
								ImGui::SameLine();
							}
							ImGui::PopStyleColor(3);
							ImGui::PopStyleVar(3);
							ImGui::Separator();
							ImGui::EndGroup();

						}
						ImGui::EndChild();
					}
				}
			}
			ImGui::EndDock();

			ImGui::EndDockspace();
		}
		else
		{
			ImGui::PopStyleVar(2);
		}

		ImGui::End();

		if (d_demoWindowOpen)
			ImGui::ShowDemoWindow();
	}

    void RGameWindow::Render()
    {
		if (_SwapChain->getWidth() == 0 && _SwapChain->getHeight() == 0)
		{
			return;
		}

		// Update editor (Record IMGUI commands).
		updateEditor();

		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() / 10.0f;

		// Create dynamic uniform allocation.
		gCamera.setPerspectiveProjection(45.0f, (float)gBackbuffer->GetDesc()->width / (float)gBackbuffer->GetDesc()->height, 0.1f, 1000.0f);
		gCamera.getTransform()->setPosition(FVector3(sin(time * glm::radians(90.0f)) * 25.0f, -35.0f, cos(time * glm::radians(90.0f)) * 25.0f));
		gCamera.getTransform()->lookAt(FVector3(0.0f, -25.0f, 0.0f));

		// render graph builder for this
		RRenderGraphBuilder* builder = gRenderGraphManager->get_new_graph_builder();
		
		// Acquire swapchain target.
		FRGMCommonBackbufferTargets bb_data = gRGMCommon.RGAcquireBackbuffer(builder, renderer, _SwapChain);
	
		// Import offscreen render target.
		FVirtualResourceRef* offscreen = gRGMCommon.RGImportTarget(builder, "offscreen", gBackbuffer);

		// Import depth-stencil.
		FVirtualResourceRef* depth = gRGMCommon.RGImportTarget(builder, "depth", gDepthStencil);

		// Draw Meshes (forward).
		if (d_gameWindowOpen)
		{
			// Draw G-Buffer
			FRGMDeferredTargets deferred_targets;
			deferred_targets = gRGMMesh.RGDrawMeshesDeferredGBuffer(builder, depth, &gCamera, 1, &gRealMesh);

			// Compute SSAO
			FRGMDeferredSSAOTargets ssaotargets;
			ssaotargets = gRGMSSAO.RGDrawSSAO_Deferred(builder, deferred_targets.pDepth, deferred_targets.pNormal, deferred_targets.pPosition, &gCamera, gSSAOVars);

			// Create shadow map and draw shadows;
			FVirtualResourceDesc shadowMapDesc;
			shadowMapDesc.mType = VIRTUAL_RESOURCE_TYPE_TEXTURE;
			shadowMapDesc.mTextureDesc.depth = 1;
			shadowMapDesc.mTextureDesc.format = TEXTURE_FORMAT_D32_FLOAT;
			shadowMapDesc.mTextureDesc.width = 2048;
			shadowMapDesc.mTextureDesc.height = 2048;
			shadowMapDesc.mTextureDesc.sampleCount = 1;
			shadowMapDesc.mTextureDesc.sampleQuality = 1;

			gShadowCamera.setOrthoProjection(200.0f, 200.0f, 0.1f, 700.0f);
			gShadowCamera.getTransform()->setRotation(glm::vec3(gLightRotX * glm::pi<float>() / 180.0f, gLightRotY * glm::pi<float>() / 180.0f, 0.0f));
			gShadowCamera.getTransform()->setPosition(-gShadowCamera.getTransform()->GetForward() * 400.0f);
			gShadowCamera.getTransform()->lookAt(FVector3(0.0f, 0.0f, 0.0f));

			FVirtualResourceRef* shadowMap = gRGMCommon.RGCreateTarget(builder, "shadowmap_1", shadowMapDesc);
			FRGMForwardTargets shadowTargets = gRGMMesh.RGDrawDepth(builder, shadowMap, &gShadowCamera, 1, &gRealMesh);

			FRGMVolumetricFogTargets volumeTargets = gRGMVolumetric.RGDrawVolumetricFog(builder, deferred_targets.pPosition, deferred_targets.pDepth, shadowTargets.pDepth, &gCamera, &gShadowCamera, gVolumetricVars);

			// Compose targets together.
			FRGMDeferredComposeTargets deferred_compose;
			deferred_compose = gRGMMesh.RGDrawMeshesDeferredCompose(builder, offscreen, deferred_targets.pAlbedo, volumeTargets.pSceneDepth, volumeTargets.pScenePosition, ssaotargets.pNormal, ssaotargets.pSSAO, volumeTargets.pShadowDepth, volumeTargets.pOutput, &gCamera, &gShadowCamera);
		}

		// Draw UI
		bb_data.pBackbuffer = gUIRenderer.render(builder, bb_data.pBackbuffer);
	

		// Submit command list to the graphics queue and present the backbuffer.
		gRGMCommon.RGSubmitAndPresent(builder, _SwapChain, bb_data.pBackbuffer);

		//Build graph.
		RRenderGraph* graph;
		gRenderGraphManager->validate_and_build(builder, &graph);
		
		// Execute render graph.
		graph->execute(renderer);


    }
}