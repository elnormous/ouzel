// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <objc/message.h>
#include "InputSystemMacOS.hpp"
#include "CursorMacOS.hpp"
#include "core/macos/NativeWindowMacOS.hpp"
#include "core/Engine.hpp"
#include "events/Event.hpp"
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
        const IOKitErrorCategory ioKitErrorCategory {};

        InputSystemMacOS::InputSystemMacOS(const std::function<std::future<bool>(const Event&)>& initCallback):
            InputSystem(initCallback),
            keyboardDevice(new KeyboardDevice(*this, ++lastDeviceId)),
            mouseDevice(new MouseDeviceMacOS(*this, ++lastDeviceId)),
            touchpadDevice(new TouchpadDevice(*this, ++lastDeviceId, false))
        {
            defaultCursor = [NSCursor arrowCursor];

            unsigned char* data = emptyCursorData;

            NSImage* image = [[[NSImage alloc] initWithSize:NSMakeSize(1, 1)] autorelease];
            NSBitmapImageRep* imageRep = [[[NSBitmapImageRep alloc]
                                           initWithBitmapDataPlanes:&data
                                           pixelsWide:1
                                           pixelsHigh:1
                                           bitsPerSample:8
                                           samplesPerPixel:4
                                           hasAlpha:YES
                                           isPlanar:NO
                                           colorSpaceName:NSDeviceRGBColorSpace
                                           bitmapFormat:NSAlphaNonpremultipliedBitmapFormat
                                           bytesPerRow:4
                                           bitsPerPixel:32] autorelease];

            [image addRepresentation:imageRep];
            emptyCursor = [[NSCursor alloc] initWithImage:image hotSpot:NSMakePoint(0, 0)];

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
            IOReturn ret;
            if ((ret = IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone)) != kIOReturnSuccess)
                throw std::system_error(ret, ioKitErrorCategory, "Failed to initialize HID manager");

            IOHIDManagerRegisterDeviceMatchingCallback(hidManager, deviceAdded, this);
            IOHIDManagerRegisterDeviceRemovalCallback(hidManager, deviceRemoved, this);
            IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        InputSystemMacOS::~InputSystemMacOS()
        {
            if (emptyCursor)
                [emptyCursor release];

            if (connectDelegate)
                [connectDelegate release];

            if (hidManager)
            {
                IOHIDManagerClose(hidManager, kIOHIDOptionsTypeNone);
                CFRelease(hidManager);
            }
        }

        void InputSystemMacOS::executeCommand(const Command& command)
        {
            switch (command.type)
            {
                case Command::Type::START_DEVICE_DISCOVERY:
                    startGamepadDiscovery();
                    break;
                case Command::Type::STOP_DEVICE_DISCOVERY:
                    stopGamepadDiscovery();
                    break;
                case Command::Type::SET_PLAYER_INDEX:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        GamepadDeviceMacOS* gamepadDevice = static_cast<GamepadDeviceMacOS*>(inputDevice);
                        gamepadDevice->setPlayerIndex(command.playerIndex);
                    }
                    break;
                }
                case Command::Type::SET_VIBRATION:
                {
                    break;
                }
                case Command::Type::SET_POSITION:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice.get())
                            mouseDevice->setPosition(command.position);
                    }
                    break;
                }
                case Command::Type::INIT_CURSOR:
                {
                    if (command.cursorResource > cursors.size())
                        cursors.resize(command.cursorResource);

                    if (command.data.empty())
                    {
                        std::unique_ptr<CursorMacOS> cursor(new CursorMacOS(command.systemCursor));
                        cursors[command.cursorResource - 1] = std::move(cursor);
                    }
                    else
                    {
                        std::unique_ptr<CursorMacOS> cursor(new CursorMacOS(command.data, command.size,
                                                                                        command.pixelFormat, command.hotSpot));
                        cursors[command.cursorResource - 1] = std::move(cursor);
                    }
                    break;
                }
                case Command::Type::DESTROY_CURSOR:
                {
                    CursorMacOS* cursor = cursors[command.cursorResource - 1].get();

                    if (mouseDevice->getCursor() == cursor)
                    {
                        mouseDevice->setCursor(nullptr);
                        NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(engine->getWindow()->getNativeWindow());
                        [windowMacOS->getNativeWindow() invalidateCursorRectsForView:windowMacOS->getNativeView()];
                    }

                    cursors[command.cursorResource - 1].reset();
                    break;
                }
                case Command::Type::SET_CURSOR:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice.get())
                        {
                            if (command.cursorResource)
                                mouseDevice->setCursor(cursors[command.cursorResource - 1].get());
                            else
                                mouseDevice->setCursor(nullptr);

                            NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(engine->getWindow()->getNativeWindow());
                            [windowMacOS->getNativeWindow() invalidateCursorRectsForView:windowMacOS->getNativeView()];
                        }
                    }
                    break;
                }
                case Command::Type::SET_CURSOR_VISIBLE:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice.get())
                            mouseDevice->setCursorVisible(command.visible);
                    }
                    break;
                }
                case Command::Type::SET_CURSOR_LOCKED:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice.get())
                            mouseDevice->setCursorLocked(command.locked);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        void InputSystemMacOS::startGamepadDiscovery()
        {
            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        void InputSystemMacOS::stopGamepadDiscovery()
        {
            [GCController stopWirelessControllerDiscovery];
        }

        void InputSystemMacOS::handleGamepadDiscoveryCompleted()
        {
            sendEvent(Event(Event::Type::DEVICE_DISCOVERY_COMPLETE));
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
                std::unique_ptr<GamepadDeviceGC> gamepadDevice(new GamepadDeviceGC(*this, ++lastDeviceId, controller));
                gamepadDevicesGC.insert(std::make_pair(controller, std::move(gamepadDevice)));
            }
        }

        void InputSystemMacOS::handleGamepadDisconnected(GCControllerPtr controller)
        {
            auto i = gamepadDevicesGC.find(controller);

            if (i != gamepadDevicesGC.end())
                gamepadDevicesGC.erase(i);
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
                std::unique_ptr<GamepadDeviceIOKit> gamepadDevice(new GamepadDeviceIOKit(*this, ++lastDeviceId, device));
                gamepadDevicesIOKit.insert(std::make_pair(device, std::move(gamepadDevice)));
            }
        }

        void InputSystemMacOS::handleGamepadDisconnected(IOHIDDeviceRef device)
        {
            auto i = gamepadDevicesIOKit.find(device);

            if (i != gamepadDevicesIOKit.end())
                gamepadDevicesIOKit.erase(i);
        }

        NSCursorPtr InputSystemMacOS::getCursor() const
        {
            if (mouseDevice->isCursorVisible())
            {
                if (mouseDevice->getCursor())
                    return mouseDevice->getCursor()->getCursor();
                else
                    return defaultCursor;
            }
            else
                return emptyCursor;
        }
    } // namespace input
} // namespace ouzel
