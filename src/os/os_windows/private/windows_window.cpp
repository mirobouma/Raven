#include "os/windows/windows_window.h"
#include "os/windows/util/wstring.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace Raven
{
    WindowsWindow::WindowsWindow(const std::string title, const Vector2Int size, WindowCreationFlags creationFlags)
    {
        _title = title;

        Vector2Int position = Vector2Int(0, 0);
        long flags;

        // Set up proper variables based on flags.
        if (creationFlags & WindowCreationFlags_AutoPosition)
        {
            position.set_x(static_cast<int>(CW_USEDEFAULT));
            position.set_y(static_cast<int>(CW_USEDEFAULT));
        }

        if (creationFlags & WindowCreationFlags_Borderless)
        {
            flags = WS_POPUP;
        }
        else
        {
            flags = WS_OVERLAPPEDWINDOW;
        }

        if (creationFlags & WindowCreationFlags_AlwaysOnTop)
        {
            flags |= WS_EX_TOPMOST;
        }

        if (creationFlags & WindowCreationFlags_NoResize)
        {
            flags &= ~WS_THICKFRAME;
            flags &= ~WS_MAXIMIZEBOX;
        }

        RECT wr = { 0, 0, static_cast<long>(size.x()), static_cast<long>(size.y()) }; // set the size, but not the position
        AdjustWindowRect(&wr, flags, FALSE); // adjust the size to account for the window title bar.

        // create the window and use the result as the handle
        _handle = CreateWindowExW(NULL,
            L"RavenWindow",    // name of the window class
            utf8_decode(_title).c_str(), // title of the window
            flags,    // window style
            static_cast<int>(position.x()),    // x-position of the window
            static_cast<int>(position.y()),    // y-position of the window
            wr.right - wr.left,    // width of the window
            wr.bottom - wr.top,    // height of the window
            NULL,    // we have no parent window, NULL
            NULL,    // we aren't using menus, NULL
            GetModuleHandle(nullptr),    // application handle
            this);

        // Show the window if it should be shown by default.
        if (creationFlags & WindowCreationFlags_Shown)
        {
            show();
        }

        if (creationFlags & WindowCreationFlags_AlwaysOnTop)
        {
            SetWindowPos((HWND)_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
    }

    WindowHandle WindowsWindow::get_handle() const
    {
        return _handle;
    }

    void WindowsWindow::show()
    {
        ShowWindow((HWND)_handle, SW_SHOW);
    }

    void WindowsWindow::minimize()
    {
        ShowWindow((HWND)_handle, SW_HIDE);
    }

    void WindowsWindow::set_title(std::string title)
    {
        SetWindowTextW((HWND)_handle, utf8_decode(title).c_str());
        _title = title;
    }

    const std::string& WindowsWindow::get_title() const
    {
        return _title;
    }
}