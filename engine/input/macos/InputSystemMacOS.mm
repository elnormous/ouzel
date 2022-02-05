// Ouzel by Elviss Strazdins

#include <objc/message.h>
#include "InputSystemMacOS.hpp"
#include "CursorMacOS.hpp"
#include "../../core/macos/NativeWindowMacOS.hpp"
#include "../../core/Engine.hpp"
#include "../../events/Event.hpp"
#include "../../platform/iokit/ErrorCategory.hpp"
#include "../../utils/Log.hpp"

typedef struct CF_BRIDGED_TYPE(id) __IOHIDServiceClient* IOHIDServiceClientRef;
extern "C" CFTypeRef _Nullable IOHIDServiceClientCopyProperty(IOHIDServiceClientRef service, CFStringRef key);

@interface ConnectDelegate: NSObject

- (void)handleControllerConnected:(NSNotification*)notification;
- (void)handleControllerDisconnected:(NSNotification*)notification;
@end

@implementation ConnectDelegate
{
    ouzel::input::macos::InputSystem* input;
}

- (id)initWithInput:(ouzel::input::macos::InputSystem*)initInput
{
    if (self = [super init])
        input = initInput;

        return self;
}

- (void)handleControllerConnected:(NSNotification*)notification
{
    input->handleGamepadConnected(notification.object);
}

- (void)handleControllerDisconnected:(NSNotification*)notification
{
    input->handleGamepadDisconnected(notification.object);
}
@end

namespace ouzel::input::macos
{
    namespace
    {
        void deviceAdded(void* ctx, IOReturn, void*, IOHIDDeviceRef device)
        {
            const auto inputSystemMacOS = static_cast<InputSystem*>(ctx);
            inputSystemMacOS->handleGamepadConnected(device);
        }

        void deviceRemoved(void* ctx, IOReturn, void*, IOHIDDeviceRef device)
        {
            const auto inputSystemMacOS = static_cast<InputSystem*>(ctx);
            inputSystemMacOS->handleGamepadDisconnected(device);
        }
    }

    InputSystem::InputSystem():
        keyboardDevice{std::make_unique<KeyboardDevice>(*this, getNextDeviceId())},
        mouseDevice{std::make_unique<MouseDevice>(*this, getNextDeviceId())},
        touchpadDevice{std::make_unique<TouchpadDevice>(*this, getNextDeviceId(), false)}
    {
        defaultCursor = [NSCursor arrowCursor];

        auto data = emptyCursorData;

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
                                @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey: @(kHIDUsage_GD_MultiAxisController)}
                            ];

        hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

        IOHIDManagerSetDeviceMatchingMultiple(hidManager, (CFArrayRef)criteria);
        if (const auto result = IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone); result != kIOReturnSuccess)
            throw std::system_error{result, platform::iokit::errorCategory, "Failed to initialize HID manager"};

        IOHIDManagerRegisterDeviceMatchingCallback(hidManager, deviceAdded, this);
        IOHIDManagerRegisterDeviceRemovalCallback(hidManager, deviceRemoved, this);
        IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

        [GCController startWirelessControllerDiscoveryWithCompletionHandler:
            ^(void) { handleGamepadDiscoveryCompleted(); }];
    }

    InputSystem::~InputSystem()
    {
        if (emptyCursor)
            [emptyCursor release];

        if (connectDelegate)
            [connectDelegate release];

        if (hidManager)
            IOHIDManagerClose(hidManager, kIOHIDOptionsTypeNone);
    }

    void InputSystem::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::startDeviceDiscovery:
                startGamepadDiscovery();
                break;
            case Command::Type::stopDeviceDiscovery:
                stopGamepadDiscovery();
                break;
            case Command::Type::setAbsoluteDpadValues:
            {
                if (const auto inputDevice = getInputDevice(command.deviceId))
                {
                    const auto gamepadDevice = static_cast<GamepadDevice*>(inputDevice);
                    gamepadDevice->setAbsoluteDpadValues(command.absoluteDpadValues);
                }
                break;
            }
            case Command::Type::setRotationAllowed:
            {
                if (const auto inputDevice = getInputDevice(command.deviceId))
                {
                    const auto gamepadDevice = static_cast<GamepadDevice*>(inputDevice);
                    gamepadDevice->setRotationAllowed(command.rotationAllowed);
                }
                break;
            }
            case Command::Type::setPlayerIndex:
            {
                if (auto inputDevice = getInputDevice(command.deviceId))
                {
                    const auto gamepadDevice = static_cast<GamepadDevice*>(inputDevice);
                    gamepadDevice->setPlayerIndex(command.playerIndex);
                }
                break;
            }
            case Command::Type::setVibration:
            {
                break;
            }
            case Command::Type::setPosition:
            {
                if (const auto inputDevice = getInputDevice(command.deviceId))
                    if (inputDevice == mouseDevice.get())
                        mouseDevice->setPosition(command.position);
                break;
            }
            case Command::Type::initCursor:
            {
                if (command.cursorResource > cursors.size())
                    cursors.resize(command.cursorResource);

                if (command.data.empty())
                {
                    auto cursor = std::make_unique<Cursor>(command.systemCursor);
                    cursors[command.cursorResource - 1] = std::move(cursor);
                }
                else
                {
                    auto cursor = std::make_unique<Cursor>(command.data, command.size,
                                                           command.pixelFormat, command.hotSpot);
                    cursors[command.cursorResource - 1] = std::move(cursor);
                }
                break;
            }
            case Command::Type::destroyCursor:
            {
                auto cursor = cursors[command.cursorResource - 1].get();

                if (mouseDevice->getCursor() == cursor)
                {
                    mouseDevice->setCursor(nullptr);
                    const auto& windowMacOs = engine->getWindow().getNativeWindow();
                    [windowMacOs.getNativeWindow() invalidateCursorRectsForView:windowMacOs.getNativeView()];
                }

                cursors[command.cursorResource - 1].reset();
                break;
            }
            case Command::Type::setCursor:
            {
                if (auto inputDevice = getInputDevice(command.deviceId))
                {
                    if (inputDevice == mouseDevice.get())
                    {
                        if (command.cursorResource)
                            mouseDevice->setCursor(cursors[command.cursorResource - 1].get());
                        else
                            mouseDevice->setCursor(nullptr);

                        const auto& windowMacOs = engine->getWindow().getNativeWindow();
                        [windowMacOs.getNativeWindow() invalidateCursorRectsForView:windowMacOs.getNativeView()];
                    }
                }
                break;
            }
            case Command::Type::setCursorVisible:
            {
                if (auto inputDevice = getInputDevice(command.deviceId))
                {
                    if (inputDevice == mouseDevice.get())
                        mouseDevice->setCursorVisible(command.visible);
                }
                break;
            }
            case Command::Type::setCursorLocked:
            {
                if (auto inputDevice = getInputDevice(command.deviceId))
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

    void InputSystem::startGamepadDiscovery()
    {
        [GCController startWirelessControllerDiscoveryWithCompletionHandler:
            ^(void) { handleGamepadDiscoveryCompleted(); }];
    }

    void InputSystem::stopGamepadDiscovery()
    {
        [GCController stopWirelessControllerDiscovery];
    }

    void InputSystem::handleGamepadDiscoveryCompleted()
    {
        sendEvent(Event(Event::Type::deviceDiscoveryComplete));
    }

    void InputSystem::handleGamepadConnected(GCControllerPtr controller)
    {
        std::int32_t vendorId = 0;
        std::int32_t productId = 0;

        if (class_respondsToSelector(object_getClass(controller), sel_getUid("hidServices")))
        {
            NSArray* hidServices = reinterpret_cast<NSArray* (*)(id, SEL)>(&objc_msgSend)(controller, sel_getUid("hidServices"));

            if (hidServices && [hidServices count] > 0)
            {
                auto service = reinterpret_cast<IOHIDServiceClientRef (*)(id, SEL)>(&objc_msgSend)([hidServices firstObject], sel_getUid("service"));

                if (const platform::corefoundation::Pointer vendor = static_cast<CFNumberRef>(IOHIDServiceClientCopyProperty(service,
                                                                                                                             CFSTR(kIOHIDVendorIDKey))))
                    CFNumberGetValue(vendor, kCFNumberSInt32Type, &vendorId);

                if (const platform::corefoundation::Pointer product = static_cast<CFNumberRef>(IOHIDServiceClientCopyProperty(service,
                                                                                                                              CFSTR(kIOHIDProductIDKey))))
                    CFNumberGetValue(product, kCFNumberSInt32Type, &productId);
            }
        }

        const bool supportsGameController = (vendorId == 0x1038 && productId == 0x1420) || // SteelSeries Nimbus
            (vendorId == 0x0F0D && productId == 0x0090); // Horipad Ultimate

        // Use IOKit only if the controller does not support GameController framework
        if (supportsGameController)
        {
            std::vector<std::int32_t> playerIndices = {0, 1, 2, 3};

            for (const auto& i : gamepadDevicesGC)
                if (const auto n = std::find(playerIndices.begin(), playerIndices.end(), i.second->getPlayerIndex()); n != playerIndices.end())
                    playerIndices.erase(n);

            if (!playerIndices.empty())
                controller.playerIndex = static_cast<GCControllerPlayerIndex>(playerIndices.front());

            auto gamepadDevice = std::make_unique<GamepadDeviceGC>(*this, getNextDeviceId(), controller);
            gamepadDevicesGC.insert(std::make_pair(controller, std::move(gamepadDevice)));
        }
    }

    void InputSystem::handleGamepadDisconnected(GCControllerPtr controller)
    {
        if (const auto i = gamepadDevicesGC.find(controller); i != gamepadDevicesGC.end())
            gamepadDevicesGC.erase(i);
    }

    void InputSystem::handleGamepadConnected(IOHIDDeviceRef device)
    {
        std::int32_t vendorId = 0;
        std::int32_t productId = 0;

        if (const auto vendor = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey))))
            CFNumberGetValue(vendor, kCFNumberSInt32Type, &vendorId);

        if (const auto product = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey))))
            CFNumberGetValue(product, kCFNumberSInt32Type, &productId);

        const bool supportsGameController = (vendorId == 0x1038 && productId == 0x1420) || // SteelSeries Nimbus
            (vendorId == 0x0F0D && productId == 0x0090); // Horipad Ultimate

        // Use IOKit only if the controller does not support GameController framework
        if (!supportsGameController)
        {
            auto gamepadDevice = std::make_unique<GamepadDeviceIOKit>(*this, getNextDeviceId(), device);
            gamepadDevicesIOKit.insert(std::make_pair(device, std::move(gamepadDevice)));
        }
    }

    void InputSystem::handleGamepadDisconnected(IOHIDDeviceRef device)
    {
        if (const auto i = gamepadDevicesIOKit.find(device); i != gamepadDevicesIOKit.end())
            gamepadDevicesIOKit.erase(i);
    }

    NSCursorPtr InputSystem::getCursor() const
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
}
