#pragma once

namespace Raven
{
	/*! \enum ESwapChainPresentMode
	 \brief Enumeration describing swapchain present modes.
	 */
	typedef enum ESwapChainPresentMode 
	{
		SWAPCHAIN_PRESENT_MODE_FIFO,
		SWAPCHAIN_PRESENT_MODE_MAILBOX,
		SWAPCHAIN_PRESENT_MODE_IMMEDIATE,
		MAX_SWAPCHAIN_PRESENT_MODE
	} ESwapChainPresentMode;


	/*! \struct FShaderCreateDesc
	 \brief Structure describing the parameters for shader creation.
	 */
	struct FSwapChainCreateDesc
	{
		ESwapChainPresentMode presentMode;
	};

	struct IRenderTarget;

	/*! \class ISwapChain
	\brief The ISwapChain class is an abstraction layer on top of the underlying graphics API.
	*/
	struct ISwapChain 
	{
		/*! Get the total amount of backbuffers for the specified swapchain.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual uint32_t getImageCount() const = 0;

		/*! Get the index of the current backbuffer image.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual uint32_t getCurrentImageIndex() const = 0;

		/*! Get the width of the current backbuffer image.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual uint32_t getWidth() const = 0;

		/*! Get the height of the current backbuffer image.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual uint32_t getHeight() const = 0;

		/*! Acquire next image on the swapchain.
		\param[in] index Index of the render target.
		\sa getImageCount
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual IRenderTarget* getRenderTarget(uint32_t index) const = 0;

	};
}