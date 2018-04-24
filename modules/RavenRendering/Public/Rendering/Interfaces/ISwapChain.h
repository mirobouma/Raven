#pragma once

namespace Raven
{
	/*! \enum ESwapChainPresentMode
	 \brief Enumeration describing swapchain present modes.
	 */
	typedef enum ESwapChainPresentMode {
		SWAPCHAIN_PRESENT_MODE_FIFO,
		SWAPCHAIN_PRESENT_MODE_MAILBOX,
		SWAPCHAIN_PRESENT_MODE_IMMEDIATE
		MAX_SWAPCHAIN_PRESENT_MODE
	} ESwapChainPresentMode;


	/*! \struct FShaderCreateDesc
	 \brief Structure describing the parameters for shader creation.
	 */
	struct FSwapChainCreateDesc
	{
		ESwapChainPresentMode presentMode;
	};

	/*! \class ISwapChain
	\brief The ISwapChain class is an abstraction layer on top of the underlying graphics API.
	*/
	struct ISwapChain {

	};
}