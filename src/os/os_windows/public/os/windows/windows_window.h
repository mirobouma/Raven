#include "os/api/native_window.h"

namespace Raven
{
    class WindowsWindow : public INativeWindow {
    protected:
        friend class WindowSystem;
        friend class OSWindows;

        WindowsWindow(const std::string title, const Vector2Int size, WindowCreationFlags creationFlags);
    public:
        virtual WindowHandle get_handle() const override;

        virtual void show() override;
        virtual void minimize() override;

        virtual void set_title(std::string title) override;
        virtual const std::string& get_title() const override;
    private:
        WindowHandle _handle;
        std::string _title;
    };

    typedef WindowsWindow NativeWindow;
}