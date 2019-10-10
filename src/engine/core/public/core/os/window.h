#pragma once
#include "core/string.h"
#include "math/vector.h"
#include "util/enum_as_flags.h"

typedef void* WindowHandle;

typedef enum WindowCreationFlags
{
    WindowCreationFlags_None = 0, /*!< No special window creation flags specified. */
    WindowCreationFlags_Borderless = 1 << 0, /*!< Create the window without borders. */
    WindowCreationFlags_NoResize = 1 << 1, /*!< Do not allow the window to be resized. */
    WindowCreationFlags_Shown = 1 << 2, /*!< Show the window on creation by default. */
    WindowCreationFlags_AlwaysOnTop = 1 << 3, /*!< The window should always be on top. */
    WindowCreationFlags_AutoPosition = 1 << 4 /*!< Automatically choose location of the window. */
} WindowCreationFlags;
DECL_ENUM_AS_FLAGS(WindowCreationFlags);

class Window {
protected:
    friend class WindowSystem;

    Window() = default;
    Window(const WindowSystem&) = delete; /*!< Delete copy constuctor */
    Window& operator= (const WindowSystem&) = delete; /*!< Delete assignment constuctor */
    virtual ~Window() = default;

    static Window* create(const String title, const Vector2Int size, WindowCreationFlags creationFlags);
    static Window* (*create_window_func)(const String, const Vector2Int, WindowCreationFlags);

public:
    virtual WindowHandle get_handle() const = 0;

    virtual void show() = 0;
    virtual void minimize() = 0;

    virtual void set_title(String title) = 0;
    virtual const String& get_title() const = 0;
};