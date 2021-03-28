// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_NATIVEWINDOWTVOS_HPP
#define OUZEL_CORE_NATIVEWINDOWTVOS_HPP

#ifdef __OBJC__
#  import <UIKit/UIKit.h>
typedef UIScreen* UIScreenPtr;
typedef UIWindow* UIWindowPtr;
typedef UIView* UIViewPtr;
typedef UIViewController* UIViewControllerPtr;
typedef UITextField* UITextFieldPtr;
#else
#  include <objc/objc.h>
typedef id UIScreenPtr;
typedef id UIWindowPtr;
typedef id UIViewPtr;
typedef id UIViewControllerPtr;
typedef id UITextFieldPtr;
#endif

#include "../NativeWindow.hpp"
#include "../../graphics/Graphics.hpp"

namespace ouzel::core::tvos
{
    class NativeWindow final: public core::NativeWindow
    {
    public:
        NativeWindow(const std::function<void(const Event&)>& initCallback,
                     const std::string& newTitle,
                     graphics::Driver graphicsDriver,
                     bool newHighDpi);
        ~NativeWindow() override;

        auto getNativeWindow() const noexcept { return window; }
        auto getNativeView() const noexcept { return view; }
        auto getTextField() const noexcept { return textField; }

        void handleResize(const Size<std::uint32_t, 2>& newSize);

    private:
        void executeCommand(const Command& command) final;

        UIScreenPtr screen = nil;
        UIWindowPtr window = nil;
        UIViewPtr view = nil;
        UIViewControllerPtr viewController = nil;

        UITextFieldPtr textField = nil;
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWTVOS_HPP
