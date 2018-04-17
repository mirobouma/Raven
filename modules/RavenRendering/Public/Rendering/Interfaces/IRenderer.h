#pragma once
#include "Platform/Interfaces/IPlatformWindow.h"

// Structure interfaces.
#include "Rendering/Interfaces/IBuffer.h"
#include "Rendering/Interfaces/ICommandList.h"
#include "Rendering/Interfaces/IPipeline.h"
#include "Rendering/Interfaces/IRenderPass.h"
#include "Rendering/Interfaces/IRenderTarget.h"
#include "Rendering/Interfaces/IRootSignature.h"
#include "Rendering/Interfaces/ISampler.h"
#include "Rendering/Interfaces/IShader.h"
#include "Rendering/Interfaces/ISwapChain.h"
#include "Rendering/Interfaces/ITexture.h"

namespace Raven {

	/*! Enumeration of rendering results. */
	typedef enum ERenderResult {
		RR_SUCCESS = 0,
		RR_UNKNOWN_FAILURE = 0,
		RR_ALREADY_INITIALIZED,
		RR_OBJECT_DOES_NOT_EXIST,
		RR_OUT_OF_HOST_MEMORY,
		RR_OUT_OF_DEVICE_MEMORY,
		RR_INITIALIZATION_FAILED,
		RR_DEVICE_LOST,
		RR_MEMORY_MAP_FAILED,
		RR_LAYER_NOT_PRESENT,
		RR_EXTENSION_NOT_PRESENT,
		RR_FEATURE_NOT_PRESENT,
		RR_NO_SUITABLE_DEVICE,
		RR_INCOMPATIBLE_DRIVER,
		RR_TOO_MANY_OBJECTS,
		RR_FORMAT_NOT_SUPPORTED,

	} ERenderResult;

	/*! \class IRenderer
	\brief The IRenderer class in an abstract interface for sending rendering commands to the underlying graphics API.
	*/
	class IRenderer {
	public:
		/*! Initialize the renderer. 
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult initialize() = 0;

		/*! Destroy the renderer. 
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroy() = 0;

		/*! Create a swapchain for the specified window. 
		\param[in] nativeWindow Reference to the native window instance this swapchain has to be created for.
		\param[in,out] ppSwapChain ppSwapChain points to an ISwapChain handle in which the resulting swapchain is returned.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createSwapChain(IPlatformWindow* nativeWindow, ISwapChain** ppSwapChain) = 0;

		/*! Destroy a swapchain. 
		\param[in] pSwapChain pSwapChain points a ISwapChain handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroySwapChain(ISwapChain* pSwapchain) = 0;

		/*! Check if the renderer has been initialized.
		\returns If initialized, this command will return true.
		*/
		virtual bool isInitialized() const = 0;
	};
}