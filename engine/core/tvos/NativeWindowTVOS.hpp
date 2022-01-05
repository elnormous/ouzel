// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_NATIVEWINDOWTVOS_HPP
#define OUZEL_CORE_NATIVEWINDOWTVOS_HPP

#ifdef __OBJC__
#  import <UIKit/UIKit.h>
using UIScreenPtr = UIScreen*;
using UIWindowPtr = UIWindow*;
using UIViewPtr = UIView*;
using UIViewControllerPtr = UIViewController*;
using UITextFieldPtr = UITextField*;
#else
#  include <objc/objc.h>
using UIScreenPtr = id;
using UIWindowPtr = id;
using UIViewPtr = id;
using UIViewControllerPtr = id;
using UITextFieldPtr = id;
#endif

#include "../NativeWindow.hpp"
#include "../../graphics/Graphics.hpp"

namespace ouzel::core::tvos
{
    class NativeWindow final: public core::NativeWindow
    {
    public:
        NativeWindow(const std::string& newTitle,
                     graphics::Driver graphicsDriver,
                     bool newHighDpi);
        ~NativeWindow() override;

        auto getNativeWindow() const noexcept { return window; }
        auto getNativeView() const noexcept { return view; }
        auto getTextField() const noexcept { return textField; }

        void handleResize(const math::Size<std::uint32_t, 2>& newSize);

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
