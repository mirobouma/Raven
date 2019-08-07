#include "game_window.h"
#include "engine/globals.h"
#include "engine/engine.h"
#include "core/debug/log_manager.h"
#include "core/debug/profiler.h"
#include "rendering/render_graph/render_graph.h"
#include "rendering/render_graph/modules/rgm_common.h"
#include "rendering/render_graph/modules/rgm_mesh.h"
#include "rendering/render_graph/modules/rgm_ssao.h"
#include "rendering/render_graph/modules/rgm_volumetric.h"
#include "core/input/input_manager.h"
#include "core/utility/timer.h"
#include "rendering/game/mesh.h"
#include "ui/ui.h"

#include <algorithm>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp> // logger
#include <assimp/DefaultLogger.hpp> // logger
#include <chrono>
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
	RTimer		gTimer;

	FSSAOVars gSSAOVars;
	FVolumetricFogVars gVolumetricVars;
	RInputMapping* InputMap;

	ImVec2 gameWindowSize = { 1, 1 };
	ImVec2 gameWindowBackbufferSize = { 1, 1 };
	ImVec2 gameWindowPos = { 0, 0 };
	float gProfilerBlockHeight = 40;
	float gProfilerZoom = 1.0f;
	bool gShowEditor = true;

	bool bKeepAspectRatio = true;
	float gAspectWidth = 16.0f;
	float gAspectHeight = 9.0f;

	float gCameraMoveSpeed = 20.0f;
	float gCameraViewSpeed = 20.0f;

	float gLightRotX = -80.0f;
	float gLightRotY = -40.0f;

	bool d_demoWindowOpen = false;
	bool d_gameWindowOpen = true;
	bool d_consoleWindowOpen = true;
	bool d_propertyWindowOpen = true;
	bool d_profilerWindowOpen = true;

	enum InputMappings
	{
		F1,
		MouseX,
		MouseR,
		MouseY,
		PadLX,
		PadLY,
		PadRX,
		PadRY,
		PadRT,
		PadLT,
		W,
		A,
		S,
		D,
		E,
		Q
	};

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

		gRealMesh = new RMesh(renderer, "assets/models/SponzaPBR/sponza.obj", 0.05f);

		gSSAOVars.cSampleRadius = 2.4f;
		gSSAOVars.cIntensity = 1.1f;
		gSSAOVars.cScale = 1.0f;
		gSSAOVars.cBias = 0.1f;

		gVolumetricVars.EnableHeightFog = false;
		gVolumetricVars.LightCol = glm::vec4(0.91, 0.61f, 0.35f, 1.0f);
		gVolumetricVars.AmbientFog = 0.2f;
		gVolumetricVars.LightMieG = 0.05f;
		gVolumetricVars.LightIntensity = 1.0f;
		gVolumetricVars.LightScatteringCoef = 0.008f;
		gVolumetricVars.LightExtinctionCoef = 0.0003f;
		gVolumetricVars.SkyboxExtinctionCoef = 1.0f;
		gVolumetricVars.NumSteps = 25;
		gVolumetricVars.MaxRayLength = 1000.0f;
		gVolumetricVars.EnableHeightFog = false;
		gVolumetricVars.HeightFog = 1.0f;
		gVolumetricVars.HeightFogScale = 0.007f;
		gVolumetricVars.NoiseSpeed = 1.0f;
		gVolumetricVars.FogDensity = 0.8f;
		gVolumetricVars.EnableNoise = false;

		InputMap = GInputManager->CreateMapping();
		InputMap->getMap()->MapFloat(InputMappings::MouseX, GInputManager->GetMouseId(), gainput::MouseAxisX);
		InputMap->getMap()->MapFloat(InputMappings::MouseY, GInputManager->GetMouseId(), gainput::MouseAxisY);
		InputMap->getMap()->MapBool(InputMappings::MouseR, GInputManager->GetMouseId(), gainput::MouseButtonRight);
		InputMap->getMap()->MapBool(InputMappings::F1, GInputManager->GetKeyboardId(), gainput::KeyF1);
		InputMap->getMap()->MapBool(InputMappings::W, GInputManager->GetKeyboardId(), gainput::KeyW);
		InputMap->getMap()->MapBool(InputMappings::A, GInputManager->GetKeyboardId(), gainput::KeyA);
		InputMap->getMap()->MapBool(InputMappings::S, GInputManager->GetKeyboardId(), gainput::KeyS);
		InputMap->getMap()->MapBool(InputMappings::D, GInputManager->GetKeyboardId(), gainput::KeyD);
		InputMap->getMap()->MapBool(InputMappings::E, GInputManager->GetKeyboardId(), gainput::KeyE);
		InputMap->getMap()->MapBool(InputMappings::Q, GInputManager->GetKeyboardId(), gainput::KeyQ);

		InputMap->getMap()->MapFloat(InputMappings::PadLX, GInputManager->GetPadId(), gainput::PadButtonLeftStickX);
		InputMap->getMap()->MapFloat(InputMappings::PadLY, GInputManager->GetPadId(), gainput::PadButtonLeftStickY);
		InputMap->getMap()->MapFloat(InputMappings::PadRX, GInputManager->GetPadId(), gainput::PadButtonRightStickX);
		InputMap->getMap()->MapFloat(InputMappings::PadRY, GInputManager->GetPadId(), gainput::PadButtonRightStickY);
		InputMap->getMap()->MapFloat(InputMappings::PadRT, GInputManager->GetPadId(), gainput::PadButtonR2);
		InputMap->getMap()->MapFloat(InputMappings::PadLT, GInputManager->GetPadId(), gainput::PadButtonL2);

		gTimer = RTimer();

	}

	void RGameWindow::OnResize(int width, int height)
	{
		// Call super.
		RRenderableWindow::OnResize(width, height);

		createGameWindow();
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
			return;
		
		if (gBackbuffer != nullptr && (gBackbuffer->GetDesc()->width == gameWindowBackbufferSize.x && gBackbuffer->GetDesc()->height == gameWindowBackbufferSize.y))
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

		
		if (gShowEditor)
		{
			FRenderTargetCreateDesc texDesc = {};
			texDesc.memoryType = MEMORY_TYPE_GPU;
			texDesc.format = TEXTURE_FORMAT_BGRA8;
			texDesc.width = (uint32_t)std::max((int32_t)gameWindowBackbufferSize.x, (int32_t)1);
			texDesc.height = (uint32_t)std::max((int32_t)gameWindowBackbufferSize.y, (int32_t)1);
			texDesc.sampleCount = 1;
			texDesc.sampleQuality = 0;
			texDesc.tiling = IMAGE_TILING_OPTIMAL;
			texDesc.usage = FRenderTargetUsage::RENDER_TARGET_USAGE_COLOR_ATTACHMENT;

			renderer->createRenderTarget(&gBackbuffer, &texDesc);
		}

		FRenderTargetCreateDesc dsDesc = {};
		dsDesc.memoryType = MEMORY_TYPE_GPU;
		dsDesc.format = TEXTURE_FORMAT_D32_FLOAT;

		if (gShowEditor)
		{
			dsDesc.width = (uint32_t)std::max((int32_t)gameWindowBackbufferSize.x, (int32_t)1);
			dsDesc.height = (uint32_t)std::max((int32_t)gameWindowBackbufferSize.y, (int32_t)1);
		}
		else
		{
			dsDesc.width = (uint32_t)std::max((int32_t)_SwapChain->getWidth(), (int32_t)1);
			dsDesc.height = (uint32_t)std::max((int32_t)_SwapChain->getHeight(), (int32_t)1);
		}

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

	void RGameWindow::handleGameInput(float dt)
	{
		if (InputMap->getMap()->GetBool(InputMappings::MouseR))
		{
			gCamera.getTransform()->rotate(-InputMap->getMap()->GetFloatDelta(InputMappings::MouseY) * 100.0f, InputMap->getMap()->GetFloatDelta(InputMappings::MouseX) * 100.0f, 0.0f);
		}

		gCamera.getTransform()->translate(gCameraMoveSpeed * dt * gCamera.getTransform()->GetForward() * (float)InputMap->getMap()->GetBool(InputMappings::W));
		gCamera.getTransform()->translate(gCameraMoveSpeed * dt * gCamera.getTransform()->GetRight() * (float)InputMap->getMap()->GetBool(InputMappings::D));
		gCamera.getTransform()->translate(gCameraMoveSpeed * dt * -gCamera.getTransform()->GetRight() * (float)InputMap->getMap()->GetBool(InputMappings::A));
		gCamera.getTransform()->translate(gCameraMoveSpeed * dt * -gCamera.getTransform()->GetForward() * (float)InputMap->getMap()->GetBool(InputMappings::S));
		gCamera.getTransform()->translate(gCameraMoveSpeed * dt * gCamera.getTransform()->GetUp() * (float)InputMap->getMap()->GetBool(InputMappings::E));
		gCamera.getTransform()->translate(gCameraMoveSpeed * dt * -gCamera.getTransform()->GetUp() * (float)InputMap->getMap()->GetBool(InputMappings::Q));

		//gainput::InputDevicePad* pad = static_cast<gainput::InputDevicePad*>(GInputManager->GetInputManager()->GetDevice(GInputManager->GetPadId()));
		//pad->Vibrate(1.0f, 0.0f);

		gCamera.getTransform()->rotate(dt * InputMap->getMap()->GetFloat(InputMappings::PadRY) * gCameraViewSpeed, dt * InputMap->getMap()->GetFloat(InputMappings::PadRX) * gCameraViewSpeed, 0.0f);

		gCamera.getTransform()->translate(gCameraMoveSpeed * dt * gCamera.getTransform()->GetForward() * InputMap->getMap()->GetFloat(InputMappings::PadLY));
		gCamera.getTransform()->translate(gCameraMoveSpeed * dt * gCamera.getTransform()->GetRight() * InputMap->getMap()->GetFloat(InputMappings::PadLX));
		gCamera.getTransform()->translate(gCameraMoveSpeed * dt * gCamera.getTransform()->GetUp() * (float)InputMap->getMap()->GetFloat(InputMappings::PadRT));
		gCamera.getTransform()->translate(gCameraMoveSpeed * dt * -gCamera.getTransform()->GetUp() * (float)InputMap->getMap()->GetFloat(InputMappings::PadLT));
	}

	void RGameWindow::updateEditor()
	{
		gUIRenderer.newFrame(_SwapChain);
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(_SwapChain->getWidth(), _SwapChain->getHeight()));

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		bool open = true;
		ImGui::Begin("DockSpace", &open, window_flags);
		ImGui::PopStyleVar(3);

		if (ImGui::BeginMainMenuBar())
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
			ImGui::EndMainMenuBar();
		}

		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);

		ImGui::End();

		if (d_gameWindowOpen)
			DrawGameWindow();

		if (d_consoleWindowOpen)
			DrawConsole();

		if (d_propertyWindowOpen)
			DrawProperties();

		if (d_profilerWindowOpen)
			DrawProfiler();

		if (d_demoWindowOpen)
			ImGui::ShowDemoWindow();

	}

	void RGameWindow::DrawProfiler()
	{
		if (ImGui::Begin("Profiler", &d_profilerWindowOpen))
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
		ImGui::End();
	}

	void RGameWindow::DrawProperties()
	{
		if (!ImGui::Begin("Properties", &d_propertyWindowOpen))
		{
			ImGui::End();
		}
		else
		{
			if (ImGui::CollapsingHeader("Camera"))
			{
				ImGui::SliderFloat("Camera Move Speed", &gCameraMoveSpeed, 0.5f, 50.0f);
				ImGui::SliderFloat("Camera View Speed", &gCameraViewSpeed, 0.5f, 50.0f);
				ImGui::Separator();
				//ImGui::SliderFloat("Camera Rotation X", &gCamera.getTransform()->mRotation.x, -180.0f, 180.0f);
				//ImGui::SliderFloat("Camera Rotation Y", &gCamera.getTransform()->mRotation.y, -180.0f, 180.0f);
				//
				//ImGui::InputFloat("Camera Position X", &gCamera.getTransform()->mPosition.x, -180.0f, 180.0f);
				//ImGui::InputFloat("Camera Position Y", &gCamera.getTransform()->mPosition.y, -180.0f, 180.0f);
				//ImGui::InputFloat("Camera Position Z", &gCamera.getTransform()->mPosition.z, -180.0f, 180.0f);

				//ImGui::Coll();
			}

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
			ImGui::SliderFloat("Sky extinction coefficient", &gVolumetricVars.SkyboxExtinctionCoef, 0.0f, 1.0f);
			ImGui::SliderFloat("Fog Density", &gVolumetricVars.FogDensity, 0.0f, 2.0f);
			ImGui::SliderFloat("Ambient Fog", &gVolumetricVars.AmbientFog, 0.0f, 1.0f);
			ImGui::ColorPicker4("Light Color", (float*)&gVolumetricVars.LightCol);

			ImGui::End();
		}
	}

	void RGameWindow::DrawConsole()
	{
		if (ImGui::Begin("Console", &d_consoleWindowOpen))
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
		ImGui::End();
	}

	void RGameWindow::DrawGameWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
		if (ImGui::Begin("Game", &d_gameWindowOpen, ImGuiWindowFlags_MenuBar))
		{
			ImVec4 Col = ImGui::GetStyleColorVec4(ImGuiCol_TitleBgActive);
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, Col);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Display Options"))
				{
					if (ImGui::MenuItem("Retain Aspect Ratio", NULL, &bKeepAspectRatio))
					{
						ResizeGameView(gameWindowBackbufferSize);
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGui::PopStyleColor();

			ImVec2 ContentSizeAvailable = ImGui::GetContentRegionAvail();
			gameWindowSize = ContentSizeAvailable;

			ResizeGameView(ContentSizeAvailable);

			if (bKeepAspectRatio)
			{
				ImVec2 padding = ImVec2((ImGui::GetContentRegionAvail().x - gameWindowBackbufferSize.x) / 2.0f, (ImGui::GetContentRegionAvail().y - gameWindowBackbufferSize.y) / 2.0f);
				ImVec2 currentCursorPos = ImGui::GetCursorPos();
				ImGui::SetCursorPos(ImVec2(currentCursorPos.x + padding.x, currentCursorPos.y + padding.y));
			}
			if (ImGui::IsWindowFocused())
			{
				handleGameInput(gTimer.GetDelta());

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

				ImGui::PopStyleVar(2);

			}

			ImGui::Image(gBackbuffer->GetImage(), gameWindowBackbufferSize);

			if (ImGui::IsWindowFocused())
			{
				//ImGui::PopStyleVar();
			}

		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void RGameWindow::ResizeGameView(ImVec2 contentSize)
	{
		if (bKeepAspectRatio)
		{
			float aspect = gameWindowSize.x / gameWindowSize.y;
			float reqAspect = gAspectWidth / gAspectHeight;

			if (aspect > reqAspect)
				gameWindowBackbufferSize = ImVec2((contentSize.x * ((gAspectWidth / gAspectHeight) / (contentSize.x / contentSize.y))), contentSize.y);
			else if (aspect < reqAspect)
				gameWindowBackbufferSize = ImVec2(contentSize.x, (contentSize.y * ((gAspectHeight / gAspectWidth) / (contentSize.y / contentSize.x))));
			else
				gameWindowBackbufferSize = contentSize;

		}
		else
		{
			gameWindowBackbufferSize = contentSize;
		}

		createGameWindow();
	}

	void RGameWindow::Render()
	{
		gTimer.Tick();

		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() / 10.0f;

		if (_SwapChain->getWidth() == 0 && _SwapChain->getHeight() == 0)
		{
			return;
		}

		if (InputMap->getMap()->GetBoolIsNew(InputMappings::F1))
		{
			gShowEditor = !gShowEditor;
			createGameWindow();
		}

		if (gShowEditor)
		{
			// Update editor (Record IMGUI commands).
			updateEditor();
		}
		else
		{
			handleGameInput(gTimer.GetDelta());
		}


		// Create dynamic uniform allocation.
		if (gShowEditor)
		{
			gCamera.setPerspectiveProjection(45.0f, (float)gBackbuffer->GetDesc()->width / (float)gBackbuffer->GetDesc()->height, 0.1f, 1000.0f);
		}
		else
		{
			gCamera.setPerspectiveProjection(45.0f, (float)_SwapChain->getWidth() / (float)_SwapChain->getHeight(), 0.1f, 1000.0f);
		}

		// render graph builder for this
		RRenderGraphBuilder* builder = gRenderGraphManager->get_new_graph_builder();

		// Acquire swapchain target.
		FRGMCommonBackbufferTargets bb_data = gRGMCommon.RGAcquireBackbuffer(builder, renderer, _SwapChain);

		// Import offscreen render target.
		FVirtualResourceRef* offscreen;

		if (gShowEditor)
		{
			offscreen = gRGMCommon.RGImportTarget(builder, "offscreen", gBackbuffer);
		}
		else
		{
			offscreen = bb_data.pBackbuffer;
		}

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

			gShadowCamera.setOrthoProjection(300.0f, 300.0f, 0.1f, 700.0f);
			gShadowCamera.getTransform()->setRotation(glm::vec3(gLightRotX, gLightRotY, 0.0f));
			gShadowCamera.getTransform()->setPosition(-gShadowCamera.getTransform()->GetForward() * 400.0f);
			gShadowCamera.getTransform()->lookAt(FVector3(0.0f, 0.0f, 0.0f));

			FVirtualResourceRef* shadowMap = gRGMCommon.RGCreateTarget(builder, "shadowmap_1", shadowMapDesc);
			FRGMForwardTargets shadowTargets = gRGMMesh.RGDrawDepth(builder, shadowMap, &gShadowCamera, 1, &gRealMesh);

			FRGMVolumetricFogTargets volumeTargets = gRGMVolumetric.RGDrawVolumetricFog(builder, deferred_targets.pPosition, deferred_targets.pDepth, shadowTargets.pDepth, &gCamera, &gShadowCamera, gVolumetricVars);

			// Compose targets together.
			FRGMDeferredComposeTargets deferred_compose;
			deferred_compose = gRGMMesh.RGDrawMeshesDeferredCompose(builder, offscreen, deferred_targets.pAlbedo, volumeTargets.pSceneDepth, volumeTargets.pScenePosition, ssaotargets.pNormal, ssaotargets.pSSAO, volumeTargets.pShadowDepth, volumeTargets.pOutput, &gCamera, &gShadowCamera);

			if (!gShowEditor)
			{
				bb_data.pBackbuffer = deferred_compose.pOutput;
			}
		}

		if (gShowEditor)
		{
			// Draw UI
			bb_data.pBackbuffer = gUIRenderer.render(builder, bb_data.pBackbuffer);
		}


		// Submit command list to the graphics queue and present the backbuffer.
		gRGMCommon.RGSubmitAndPresent(builder, _SwapChain, bb_data.pBackbuffer);

		//Build graph.
		RRenderGraph* graph;
		gRenderGraphManager->validate_and_build(builder, &graph);

		// Execute render graph.
		graph->execute(renderer);


	}
}