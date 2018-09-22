// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#import <Carbon/Carbon.h>
#import <objc/message.h>
#include "InputSystemMacOS.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

typedef struct CF_BRIDGED_TYPE(id) __IOHIDServiceClient * IOHIDServiceClientRef;
extern "C" CFTypeRef _Nullable IOHIDServiceClientCopyProperty(IOHIDServiceClientRef service, CFStringRef key);

@interface ConnectDelegate: NSObject
{
    ouzel::input::InputSystemMacOS* input;
}

-(void)handleControllerConnected:(NSNotification*)notification;
-(void)handleControllerDisconnected:(NSNotification*)notification;

@end

@implementation ConnectDelegate

-(id)initWithInput:(ouzel::input::InputSystemMacOS*)initInput
{
    if (self = [super init])
        input = initInput;

        return self;
}

-(void)handleControllerConnected:(NSNotification*)notification
{
    input->handleGamepadConnected(notification.object);
}

-(void)handleControllerDisconnected:(NSNotification*)notification
{
    input->handleGamepadDisconnected(notification.object);
}

@end

static void deviceAdded(void* ctx, IOReturn, void*, IOHIDDeviceRef device)
{
    ouzel::input::InputSystemMacOS* inputMacOS = reinterpret_cast<ouzel::input::InputSystemMacOS*>(ctx);
    inputMacOS->handleGamepadConnected(device);
}

static void deviceRemoved(void* ctx, IOReturn, void*, IOHIDDeviceRef device)
{
    ouzel::input::InputSystemMacOS* inputMacOS = reinterpret_cast<ouzel::input::InputSystemMacOS*>(ctx);
    inputMacOS->handleGamepadDisconnected(device);
}

namespace ouzel
{
    namespace input
    {
        InputSystemMacOS::InputSystemMacOS()
        {
            connectDelegate = [[ConnectDelegate alloc] initWithInput:this];

            [[NSNotificationCenter defaultCenter] addObserver:connectDelegate
                                                     selector:@selector(handleControllerConnected:)
                                                         name:GCControllerDidConnectNotification
                                                       object:nil];

            [[NSNotificationCenter defaultCenter] addObserver:connectDelegate
                                                     selector:@selector(handleControllerDisconnected:)
                                                         name:GCControllerDidDisconnectNotification
                                                       object:nil];

            for (GCController* controller in [GCController controllers])
                handleGamepadConnected(controller);

            NSArray* criteria = @[
                                  @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey: @(kHIDUsage_GD_Joystick)},
                                  @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey: @(kHIDUsage_GD_GamePad)},
                                  @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey : @(kHIDUsage_GD_MultiAxisController)}
                                  ];

            hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

            IOHIDManagerSetDeviceMatchingMultiple(hidManager, (CFArrayRef)criteria);
            IOReturn ret = IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone);
            if (ret != kIOReturnSuccess)
                throw SystemError("Failed to initialize HID manager, error: " + std::to_string(ret));

            IOHIDManagerRegisterDeviceMatchingCallback(hidManager, deviceAdded, this);
            IOHIDManagerRegisterDeviceRemovalCallback(hidManager, deviceRemoved, this);
            IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

            discovering = true;

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        InputSystemMacOS::~InputSystemMacOS()
        {
            if (connectDelegate)
                [connectDelegate release];

            if (hidManager)
            {
                IOHIDManagerClose(hidManager, kIOHIDOptionsTypeNone);
                CFRelease(hidManager);
            }
        }

        void InputSystemMacOS::setCursorPosition(const Vector2& position)
        {
            /*ouzel::Vector2 windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);

            engine->executeOnMainThread([windowLocation] {
                CGPoint screenOrigin = [[NSScreen mainScreen] visibleFrame].origin;

                NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(engine->getWindow()->getNativeWindow());
                CGPoint windowOrigin = [windowMacOS->getNativeWindow() frame].origin;

                CGWarpMouseCursorPosition(CGPointMake(screenOrigin.x + windowOrigin.x + windowLocation.x,
                                                      screenOrigin.y + windowOrigin.y + windowLocation.y));
            });*/
        }

        void InputSystemMacOS::setCursorLocked(bool locked)
        {
            /*engine->executeOnMainThread([locked] {
                CGAssociateMouseAndMouseCursorPosition(!locked);
            });
            cursorLocked = locked;*/
        }

        bool InputSystemMacOS::isCursorLocked() const
        {
            return cursorLocked;
        }

        void InputSystemMacOS::startDeviceDiscovery()
        {
            Log(Log::Level::INFO) << "Started gamepad discovery";

            discovering = true;

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        void InputSystemMacOS::stopDeviceDiscovery()
        {
            if (discovering)
            {
                Log(Log::Level::INFO) << "Stopped gamepad discovery";

                [GCController stopWirelessControllerDiscovery];

                discovering = false;
            }
        }

        void InputSystemMacOS::handleGamepadDiscoveryCompleted()
        {
            Log(Log::Level::INFO) << "Gamepad discovery completed";
            discovering = false;
        }

        void InputSystemMacOS::handleGamepadConnected(GCControllerPtr controller)
        {
            int32_t vendorId = 0;
            int32_t productId = 0;

            if (class_respondsToSelector(object_getClass(controller), sel_getUid("hidServices")))
            {
                NSArray* hidServices = reinterpret_cast<NSArray* (*)(id, SEL)>(&objc_msgSend)(controller, sel_getUid("hidServices"));

                if (hidServices && [hidServices count] > 0)
                {
                    IOHIDServiceClientRef service = reinterpret_cast<IOHIDServiceClientRef (*)(id, SEL)>(&objc_msgSend)([hidServices firstObject], sel_getUid("service"));

                    CFNumberRef vendor = static_cast<CFNumberRef>(IOHIDServiceClientCopyProperty(service, CFSTR(kIOHIDVendorIDKey)));
                    if (vendor)
                    {
                        CFNumberGetValue(vendor, kCFNumberSInt32Type, &vendorId);
                        CFRelease(vendor);
                    }

                    CFNumberRef product = static_cast<CFNumberRef>(IOHIDServiceClientCopyProperty(service, CFSTR(kIOHIDProductIDKey)));
                    if (product)
                    {
                        CFNumberGetValue(product, kCFNumberSInt32Type, &productId);
                        CFRelease(product);
                    }
                }
            }

            bool supportsGameController = (vendorId == 0x1038 && productId == 0x1420) || // SteelSeries Nimbus
            (vendorId == 0x0F0D && productId == 0x0090); // Horipad Ultimate

            // Use IOKit only if the controller does not support GameController framework
            if (supportsGameController)
            {
                std::unique_ptr<GamepadGC> gamepad(new GamepadGC(*this, ++lastDeviceId, controller));

                Event event;
                event.type = Event::Type::GAMEPAD_CONNECT;
                event.deviceId = gamepad->getDeviceId();

                gamepadsGC.insert(std::make_pair(controller, gamepad.get()));
                gamepads.insert(std::make_pair(gamepad->getDeviceId(), std::move(gamepad)));

                addEvent(event);
            }
        }

        void InputSystemMacOS::handleGamepadDisconnected(GCControllerPtr controller)
        {
            auto i = gamepadsGC.find(controller);

            if (i != gamepadsGC.end())
            {
                GamepadGC* gamepadGC = i->second;

                Event event;
                event.type = Event::Type::GAMEPAD_DISCONNECT;
                event.deviceId = gamepadGC->getDeviceId();
                addEvent(event);

                gamepadsGC.erase(i);

                auto gamepadIterator = std::find_if(gamepads.begin(), gamepads.end(), [gamepadGC](const std::pair<const uint32_t, std::unique_ptr<GamepadMacOS>>& gamepad) {
                    return gamepad.second.get() == gamepadGC;
                });

                if (gamepadIterator != gamepads.end())
                    gamepads.erase(gamepadIterator);
            }
        }

        void InputSystemMacOS::handleGamepadConnected(IOHIDDeviceRef device)
        {
            int32_t vendorId = 0;
            int32_t productId = 0;

            CFNumberRef vendor = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey)));
            if (vendor)
                CFNumberGetValue(vendor, kCFNumberSInt32Type, &vendorId);

            CFNumberRef product = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey)));
            if (product)
                CFNumberGetValue(product, kCFNumberSInt32Type, &productId);

            bool supportsGameController = (vendorId == 0x1038 && productId == 0x1420) || // SteelSeries Nimbus
            (vendorId == 0x0F0D && productId == 0x0090); // Horipad Ultimate

            // Use IOKit only if the controller does not support GameController framework
            if (!supportsGameController)
            {
                std::unique_ptr<GamepadIOKit> gamepad(new GamepadIOKit(*this, ++lastDeviceId, device));

                Event event;
                event.type = Event::Type::GAMEPAD_CONNECT;
                event.deviceId = gamepad->getDeviceId();

                gamepadsIOKit.insert(std::make_pair(device, gamepad.get()));
                gamepads.insert(std::make_pair(gamepad->getDeviceId(), std::move(gamepad)));

                addEvent(event);
            }
        }

        void InputSystemMacOS::handleGamepadDisconnected(IOHIDDeviceRef device)
        {
            auto i = gamepadsIOKit.find(device);

            if (i != gamepadsIOKit.end())
            {
                GamepadIOKit* gamepadIOKit = i->second;

                Event event;
                event.type = Event::Type::GAMEPAD_DISCONNECT;
                event.deviceId = gamepadIOKit->getDeviceId();

                addEvent(event);

                gamepadsIOKit.erase(i);

                auto gamepadIterator = std::find_if(gamepads.begin(), gamepads.end(), [gamepadIOKit](const std::pair<const uint32_t, std::unique_ptr<GamepadMacOS>>& gamepad) {
                    return gamepad.second.get() == gamepadIOKit;
                });

                if (gamepadIterator != gamepads.end())
                    gamepads.erase(gamepadIterator);
            }
        }

        void InputSystemMacOS::handleButtonValueChange(const GamepadMacOS& gamepad, Gamepad::Button button, bool pressed, float value)
        {
            Event event;
            event.type = Event::Type::GAMEPAD_BUTTON_CHANGE;

            event.deviceId = gamepad.getDeviceId();
            event.gamepadButton = button;
            event.pressed = pressed;
            event.value = value;

            addEvent(event);
        }
    }
}
