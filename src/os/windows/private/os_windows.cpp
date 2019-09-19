#include "windows/os_windows.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShellScalingApi.h>
#endif

// Convert a wide Unicode string to an UTF8 string
String utf8_encode(const WString& wstr)
{
    if (wstr.empty()) return String();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Convert an UTF8 string to a wide Unicode String
WString utf8_decode(const String& str)
{
    if (str.empty()) return WString();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

Error OSWindows::initialize_core()
{
    return Error::SUCCESS;
}

Error OSWindows::initialize()
{
    return Error::SUCCESS;
}

static BOOL CALLBACK _EnumMonitorsCount(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {

    int* data = (int*)dwData;
    (*data)++;
    return true;
}

int32_t OSWindows::get_monitor_count()
{
    int data = 0;
    EnumDisplayMonitors(nullptr, nullptr, _EnumMonitorsCount, (LPARAM)& data);
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
    EnumDisplayMonitors(NULL, NULL, _MonitorEnumProcScreenIndex, (LPARAM)& data);

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
    EnumDisplayMonitors(NULL, NULL, _MonitorEnumProcDescription, (LPARAM)& data);

    _EnumDpiData dpiData = { 0, index, 72 };
    EnumDisplayMonitors(NULL, NULL, _MonitorEnumProcDpi, (LPARAM)& dpiData);

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

int OSWindows::get_exit_code() const
{
    return 0;
}

