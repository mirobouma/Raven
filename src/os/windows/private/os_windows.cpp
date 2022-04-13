#include "windows/os_windows.h"
#include "windows/util/wstring.h"
#include "windows/windows_window.h"
#include "windows/windows_thread.h"
#include "core/logging_macros.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShellScalingApi.h>
#endif

namespace Raven
{
    HINSTANCE gHInstance = nullptr;

    Error OSWindows::initialize_core()
    {
        gHInstance = GetModuleHandle(nullptr);
        LogManager::get().register_logger(new ConsoleLogger());

        WindowsThread::make_current();
        WindowsWindow::make_current();

        return Error::SUCCESS;
    }

    // this is the main message handler for the program
    LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        WindowsWindow* wnd = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        // Process window messages.
        switch (message)
        {
        case WM_CREATE: {
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
        } break;
        }

        // Handle any messages the switch statement didn't
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    Error OSWindows::initialize()
    {
        // Register window type.
        WNDCLASSEXW winClass;

        // clear out the window class for use
        ZeroMemory(&winClass, sizeof(WNDCLASSEXW));

        // fill in the struct with the needed information
        winClass.cbSize = sizeof(WNDCLASSEXW);
        winClass.style = CS_HREDRAW | CS_VREDRAW;
        winClass.lpfnWndProc = WindowProc;
        winClass.hInstance = gHInstance == nullptr ? GetModuleHandle(nullptr) : gHInstance;
        winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        winClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
        winClass.lpszClassName = L"RavenWindow";

        // register the window class
        RegisterClassExW(&winClass);

        return Error::SUCCESS;
    }

    void OSWindows::update()
    {
        // this struct holds Windows event messages
        MSG msg;

        // Check to see if any messages are waiting in the queue
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // translate keystroke messages into the right format
            TranslateMessage(&msg);

            // send the message to the WindowProc function
            DispatchMessage(&msg);
        }
    }

    static BOOL CALLBACK _EnumMonitorsCount(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {

        int* data = (int*)dwData;
        (*data)++;
        return true;
    }

    int32_t OSWindows::get_monitor_count()
    {
        int data = 0;
        EnumDisplayMonitors(nullptr, nullptr, _EnumMonitorsCount, (LPARAM)&data);
        return data;
    }

    typedef struct {
        int count;
        int screen;
        HMONITOR monitor;
    } _EnumMonitorIndexData;

    static BOOL CALLBACK _MonitorEnumProcScreenIndex(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {

        _EnumMonitorIndexData* data = (_EnumMonitorIndexData*)dwData;

        if (data->monitor == hMonitor)
        {
            data->screen = data->count;
        }

        data->count++;
        return true;
    }

    MonitorDescription OSWindows::get_main_monitor()
    {
        const POINT ptZero = { 0, 0 };
        HMONITOR mainMonitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

        _EnumMonitorIndexData data = { 0, 0, MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY) };
        EnumDisplayMonitors(NULL, NULL, _MonitorEnumProcScreenIndex, (LPARAM)&data);

        return get_monitor_description(data.screen);
    }

    typedef struct {
        int count;
        int screen;
        int width;
        int height;
        HMONITOR monitor;
    } _EnumMonitorData;

    static BOOL CALLBACK _MonitorEnumProcDescription(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {

        _EnumMonitorData* data = (_EnumMonitorData*)dwData;
        if (data->count == data->screen)
        {
            data->width = lprcMonitor->right - lprcMonitor->left;
            data->height = lprcMonitor->bottom - lprcMonitor->top;
            data->monitor = hMonitor;
        }

        data->count++;
        return TRUE;
    }

    typedef struct {
        int count;
        int screen;
        int dpi;
    } _EnumDpiData;

    static BOOL CALLBACK _MonitorEnumProcDpi(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {

        _EnumDpiData* data = (_EnumDpiData*)dwData;

        if (data->count == data->screen)
        {
            uint32_t dpiX = 72;
            uint32_t dpiY = 72;

            //GetDpiForMonitor(hMonitor, MONITOR_DPI_TYPE::MDT_DEFAULT, &dpiX, &dpiY);
            data->dpi = (dpiX + dpiY) / 2;
        }

        data->count++;
        return TRUE;
    }

    MonitorDescription OSWindows::get_monitor_description(int32_t index)
    {
        if (index == -1)
        {
            return get_main_monitor();
        }

        _EnumMonitorData data = { 0, index, 0, 0 };
        EnumDisplayMonitors(NULL, NULL, _MonitorEnumProcDescription, (LPARAM)&data);

        _EnumDpiData dpiData = { 0, index, 72 };
        EnumDisplayMonitors(NULL, NULL, _MonitorEnumProcDpi, (LPARAM)&dpiData);

        MonitorDescription description{ index, data.width, data.height, dpiData.dpi };
        return description;
    }

    Path OSWindows::get_working_directory() const
    {
        wchar_t buffer[4096];
        GetModuleFileNameW(NULL, buffer, 4096);

        WString executable_path_wstring = WString(buffer);
        String executable_path = utf8_encode(executable_path_wstring);

        String directory = executable_path.substr(0,
            executable_path.rfind('\\', executable_path.length()));

        return Path(directory);
    }


    bool OSWindows::has_environment_variable(const String& key) const
    {
        wchar_t* wValue = new wchar_t[0x7Fff];
        int length = GetEnvironmentVariableW(utf8_decode(key).c_str(), wValue, 0x7Fff);

        if (length <= 0)
        {
            return !(GetLastError() == ERROR_ENVVAR_NOT_FOUND);
        }

        delete[] wValue;
        return true;
    }

    String OSWindows::get_environment_variable(const String& key) const
    {
        wchar_t* wValue = new wchar_t[0x7Fff];
        int length = GetEnvironmentVariableW(utf8_decode(key).c_str(), wValue, 0x7Fff);

        if (length > 0)
        {
            String utf8 = utf8_encode(wValue);
            delete[] wValue;
            return utf8;
        }

        return String();
    }

    bool OSWindows::set_environment_variable(const String& key, const String& value) const
    {
        bool isSet = SetEnvironmentVariableW(utf8_decode(key).c_str(),
            utf8_decode(value).c_str());

        return isSet;
    }

    uint64_t OSWindows::get_unix_time() const
    {
        FILETIME ft;
        SYSTEMTIME st;
        GetSystemTime(&st);
        SystemTimeToFileTime(&st, &ft);

        SYSTEMTIME ep;
        ep.wYear = 1970;
        ep.wMonth = 1;
        ep.wDayOfWeek = 4;
        ep.wDay = 1;
        ep.wHour = 0;
        ep.wMinute = 0;
        ep.wSecond = 0;
        ep.wMilliseconds = 0;
        FILETIME fep;
        SystemTimeToFileTime(&ep, &fep);

        ULARGE_INTEGER ft_punning;
        ft_punning.LowPart = ft.dwLowDateTime;
        ft_punning.HighPart = ft.dwHighDateTime;

        ULARGE_INTEGER fep_punning;
        fep_punning.LowPart = fep.dwLowDateTime;
        fep_punning.HighPart = fep.dwHighDateTime;

        return (ft_punning.QuadPart - fep_punning.QuadPart) / 10000000;

    }

    uint64_t OSWindows::get_system_time() const
    {
        const uint64_t WINDOWS_TICK = 10000;
        const uint64_t MSEC_TO_UNIX_EPOCH = 11644473600000LL;

        SYSTEMTIME st;
        GetSystemTime(&st);
        FILETIME ft;
        SystemTimeToFileTime(&st, &ft);
        uint64_t ret;
        ret = ft.dwHighDateTime;
        ret <<= 32;
        ret |= ft.dwLowDateTime;

        return (uint64_t)((ret / WINDOWS_TICK - MSEC_TO_UNIX_EPOCH) / 1000);
    }

    uint64_t OSWindows::get_page_size() const
    {
        // Get system information and fetch the page size.
        SYSTEM_INFO info;
        GetSystemInfo(&info);

        return info.dwPageSize;
    }


    int OSWindows::get_exit_code(Error error) const
    {
        return 0;
    }
}
