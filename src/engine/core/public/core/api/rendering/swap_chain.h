#pragma once

namespace Raven
{
    /*! \enum ESwapChainPresentMode
     \brief Enumeration describing swapchain present modes.
     */
    typedef enum SwapChainPresentMode
    {
        SWAPCHAIN_PRESENT_MODE_FIFO,
        SWAPCHAIN_PRESENT_MODE_MAILBOX,
        SWAPCHAIN_PRESENT_MODE_IMMEDIATE,
        MAX_SWAPCHAIN_PRESENT_MODE
    } SwapChainPresentMode;


    /*! \struct FShaderCreateDesc
     \brief Structure describing the parameters for shader creation.
     */
    struct SwapChainCreateDesc
    {
        SwapChainPresentMode presentMode;
    };

    struct IRenderTarget;
    struct ISemaphore;

    /*! \class ISwapChain
    \brief The ISwapChain class is an abstraction layer on top of the underlying graphics API.
    */
    struct ISwapChain
    {
        /*! Get the total amount of backbuffers for the specified swapchain.
        \returns On success, this command returns RR_SUCCESS.
        */
        virtual uint32_t get_image_count() const = 0;

        /*! Get the index of the current backbuffer image.
        \returns On success, this command returns RR_SUCCESS.
        */
        virtual uint32_t get_current_image_index() const = 0;

        /*! Get the width of the current backbuffer image.
        \returns On success, this command returns RR_SUCCESS.
        */
        virtual uint32_t get_width() const = 0;

        /*! Get the height of the current backbuffer image.
        \returns On success, this command returns RR_SUCCESS.
        */
        virtual uint32_t get_height() const = 0;

        /*! Acquire next image on the swapchain.
        \param[in] index Index of the render target.
        \sa getImageCount
        \returns On success, this command returns RR_SUCCESS.
        */
        virtual IRenderTarget* get_render_target(uint32_t index) const = 0;

        /*! Acquire the image acquired semaphore for this swapchain. */
        virtual ISemaphore* get_image_acquired_semaphore() const = 0;
    };
}