#include "core/os/window.h"

class WindowsWindow : public Window {
protected:
    friend class WindowSystem;
    friend class OSWindows;
    
    WindowsWindow(const String title, const Vector2Int size, WindowCreationFlags creationFlags);

    static Window* create_window_windows(const String title, const Vector2Int size, WindowCreationFlags creationFlags);
    static void make_current();

public:
    virtual WindowHandle get_handle() const override;

    virtual void show() override;
    virtual void minimize() override;

    virtual void set_title(String title) override;
    virtual const String& get_title() const override;
private:
    WindowHandle _handle;
    String _title;
};