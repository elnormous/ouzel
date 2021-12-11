// Ouzel by Elviss Strazdins

#import <Carbon/Carbon.h>
#include "MouseDeviceMacOS.hpp"
#include "InputSystemMacOS.hpp"
#include "../../core/Engine.hpp"
#include "../../core/macos/NativeWindowMacOS.hpp"

namespace ouzel::input::macos
{
    MouseDevice::MouseDevice(InputSystem& initInputSystem, DeviceId initId):
        input::MouseDevice{initInputSystem, initId}
    {
    }

    void MouseDevice::setPosition(const math::Vector<float, 2>& position)
    {
        const auto windowLocation = engine->getWindow().convertNormalizedToWindowLocation(position);

        const CGPoint screenOrigin = [[NSScreen mainScreen] visibleFrame].origin;

        const auto& windowMacOs = engine->getWindow().getNativeWindow();
        const CGPoint windowOrigin = [windowMacOs.getNativeWindow() frame].origin;

        CGWarpMouseCursorPosition(CGPointMake(screenOrigin.x + windowOrigin.x + windowLocation.v[0],
                                              screenOrigin.y + windowOrigin.y + windowLocation.v[1]));
    }

    void MouseDevice::setCursorVisible(bool visible)
    {
        cursorVisible = visible;
    }

    void MouseDevice::setCursorLocked(bool locked)
    {
        CGAssociateMouseAndMouseCursorPosition(!locked);
    }

    void MouseDevice::setCursor(Cursor* newCursor)
    {
        cursor = newCursor;
    }
}
