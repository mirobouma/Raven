#pragma once
#include "core/string.h"
#include "math/math.h"

class Window {
protected:
    friend class WindowSystem;

    Window() = default;
    Window(const WindowSystem&) = delete; /*!< Delete copy constuctor */
    Window& operator= (const WindowSystem&) = delete; /*!< Delete assignment constuctor */

    virtual ~Window() = default;

    virtual void update() = 0;
    virtual void destroy() = 0;

    virtual void set_title(String title) = 0;
    virtual String& get_title() = 0;

    virtual void resize(uint32_t width, uint32_t height) = 0;
    virtual Vector2 get_size() = 0;
};