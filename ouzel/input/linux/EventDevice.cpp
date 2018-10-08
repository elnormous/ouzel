// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "EventDevice.hpp"
#include "InputSystemLinux.hpp"
#include "input/KeyboardDevice.hpp"
#include "input/GamepadDevice.hpp"
#include "input/MouseDevice.hpp"
#include "input/TouchpadDevice.hpp"
#include "core/Engine.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static const uint32_t BITS_PER_LONG = 8 * sizeof(long);
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_LONG)

static inline bool isBitSet(const unsigned long* array, int bit)
{
    return (array[bit / BITS_PER_LONG] & (1LL << (bit % BITS_PER_LONG))) != 0;
}

namespace ouzel
{
    namespace input
    {
        EventDevice::EventDevice(InputSystemLinux& inputSystem, const std::string& initFilename):
            filename(initFilename)
        {
            fd = open(filename.c_str(), O_RDONLY);

            if (fd == -1)
                throw SystemError("Failed to open device file");

            if (ioctl(fd, EVIOCGRAB, 1) == -1)
                Log(Log::Level::WARN) << "Failed to grab device";

            char deviceName[256];
            if (ioctl(fd, EVIOCGNAME(sizeof(deviceName) - 1), deviceName) == -1)
                Log(Log::Level::WARN) << "Failed to get device name";
            else
            {
                name = deviceName;
                Log(Log::Level::INFO) << "Got device: " << name;
            }

            unsigned long eventBits[BITS_TO_LONGS(EV_CNT)];
            unsigned long absBits[BITS_TO_LONGS(ABS_CNT)];
            unsigned long relBits[BITS_TO_LONGS(REL_CNT)];
            unsigned long keyBits[BITS_TO_LONGS(KEY_CNT)];

            if (ioctl(fd, EVIOCGBIT(0, sizeof(eventBits)), eventBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBits)), absBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_REL, sizeof(relBits)), relBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits) == -1)
                throw SystemError("Failed to get device event bits");

            if (isBitSet(eventBits, EV_KEY) && (
                    isBitSet(keyBits, KEY_1) ||
                    isBitSet(keyBits, KEY_2) ||
                    isBitSet(keyBits, KEY_3) ||
                    isBitSet(keyBits, KEY_4) ||
                    isBitSet(keyBits, KEY_5) ||
                    isBitSet(keyBits, KEY_6) ||
                    isBitSet(keyBits, KEY_7) ||
                    isBitSet(keyBits, KEY_8) ||
                    isBitSet(keyBits, KEY_9) ||
                    isBitSet(keyBits, KEY_0)
                ))
                keyboardDevice.reset(new KeyboardDevice(inputSystem, inputSystem.getNextDeviceId()));

            if (isBitSet(eventBits, EV_ABS) && isBitSet(absBits, ABS_X) && isBitSet(absBits, ABS_Y))
            {
                if ((isBitSet(keyBits, BTN_STYLUS) || isBitSet(keyBits, BTN_TOOL_PEN)) || // tablet
                    (isBitSet(keyBits, BTN_TOOL_FINGER) && !isBitSet(keyBits, BTN_TOOL_PEN)) || // touchpad
                    isBitSet(keyBits, BTN_TOUCH)) // touchscreen
                {
                    touchpadDevice.reset(new TouchpadDevice(inputSystem, inputSystem.getNextDeviceId()));

                    input_absinfo info;

                    if (ioctl(fd, EVIOCGABS(ABS_MT_SLOT), &info) == -1)
                        throw SystemError("Failed to get device info");

                    touchSlots.resize(info.maximum + 1);

                    if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &info) == -1)
                        throw SystemError("Failed to get device info");
                    
                    touchMinX = info.minimum;
                    touchMaxX = info.maximum;
                    touchRangeX = touchMaxX - touchMinX;

                    if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &info) == -1)
                        throw SystemError("Failed to get device info");

                    touchMinY = info.minimum;
                    touchMaxY = info.maximum;
                    touchRangeY = touchMaxY - touchMinY;
                }
                else if (isBitSet(keyBits, BTN_MOUSE)) // mouse
                    mouseDevice.reset(new MouseDevice(inputSystem, inputSystem.getNextDeviceId()));
                
            }
            else if (isBitSet(eventBits, EV_REL) && isBitSet(relBits, REL_X) && isBitSet(relBits, REL_Y))
            {
                if (isBitSet(keyBits, BTN_MOUSE))
                    mouseDevice.reset(new MouseDevice(inputSystem, inputSystem.getNextDeviceId()));
            }

            if (isBitSet(keyBits, BTN_JOYSTICK) || // joystick
                isBitSet(keyBits, BTN_GAMEPAD)) // gamepad
                gamepadDevice.reset(new GamepadDevice(inputSystem, inputSystem.getNextDeviceId()));

            struct input_id id;
            ioctl(fd, EVIOCGID, &id);
            vendor = id.vendor;
            product = id.product;
        }

        EventDevice::~EventDevice()
        {
            if (fd != -1)
            {
                if (ioctl(fd, EVIOCGRAB, 0) == -1)
                    Log(Log::Level::WARN) << "Failed to release device";

                close(fd);
            }
        }

        void EventDevice::update()
        {
            input_event events[32];
            ssize_t bytesRead = read(fd, events, sizeof(events));

            if (bytesRead == -1)
                throw SystemError("Failed to read from " + filename); // TODO: disconnect the device

            int count = bytesRead / sizeof(input_event);

            for (int i = 0; i < count; ++i)
            {
                input_event& event = events[i];

                if (keyboardDevice)
                {
                    switch (event.type)
                    {
                        case EV_KEY:
                            if (event.value == 1 || event.value == 2) // press or repeat
                                keyboardDevice->handleKeyPress(InputSystemLinux::convertKeyCode(event.code));
                            else if (event.value == 0) // release
                                keyboardDevice->handleKeyRelease(InputSystemLinux::convertKeyCode(event.code));
                            break;
                    }
                }
                if (mouseDevice)
                {
                    switch (event.type)
                    {
                        case EV_ABS:
                        {
                            switch (event.code)
                            {
                                case ABS_X:
                                    cursorPosition.x = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(event.value), 0.0F)).x;
                                    break;
                                case ABS_Y:
                                    cursorPosition.y = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(0.0F, static_cast<float>(event.value))).y;
                                    break;
                            }

                            mouseDevice->handleMove(cursorPosition);
                            break;
                        }
                        case EV_REL:
                        {
                            Vector2 relativePos;

                            switch (event.code)
                            {
                                case REL_X:
                                    relativePos.x = static_cast<float>(event.value);
                                    break;
                                case REL_Y:
                                    relativePos.y = static_cast<float>(event.value);
                                    break;
                                case REL_WHEEL:
                                    mouseDevice->handleScroll(Vector2(0.0F, static_cast<float>(event.value)), cursorPosition);
                                    break;
                                case REL_HWHEEL:
                                    mouseDevice->handleScroll(Vector2(static_cast<float>(event.value), 0.0F), cursorPosition);
                                    break;
                            }

                            mouseDevice->handleRelativeMove(engine->getWindow()->convertWindowToNormalizedLocation(relativePos));
                            break;
                        }
                        case EV_KEY:
                        {
                            Mouse::Button button = Mouse::Button::NONE;

                            switch (event.code)
                            {
                            case BTN_LEFT:
                                button = Mouse::Button::LEFT;
                                break;
                            case BTN_RIGHT:
                                button = Mouse::Button::RIGHT;
                                break;
                            case BTN_MIDDLE:
                                button = Mouse::Button::MIDDLE;
                                break;
                            }

                            if (event.value == 1)
                                mouseDevice->handleButtonPress(button, cursorPosition);
                            else if (event.value == 0)
                                mouseDevice->handleButtonRelease(button, cursorPosition);
                            break;
                        }
                    }
                }
                if (touchpadDevice)
                {
                    switch (event.type)
                    {
                        case EV_ABS:
                        {
                            switch (event.code)
                            {
                                case ABS_MT_SLOT:
                                {
                                    currentTouchSlot = event.value;
                                    break;
                                }
                                case ABS_MT_TRACKING_ID:
                                {
                                    if (event.value >= 0)
                                    {
                                        touchSlots[currentTouchSlot].trackingId = event.value;
                                        touchSlots[currentTouchSlot].action = Slot::Action::BEGIN;
                                    }
                                    else
                                        touchSlots[currentTouchSlot].action = Slot::Action::END;
                                    break;
                                }
                                case ABS_MT_POSITION_X:
                                {
                                    touchSlots[currentTouchSlot].positionX = event.value;
                                    touchSlots[currentTouchSlot].action = Slot::Action::MOVE;
                                    break;
                                }
                                case ABS_MT_POSITION_Y:
                                {
                                    touchSlots[currentTouchSlot].positionY = event.value;
                                    touchSlots[currentTouchSlot].action = Slot::Action::MOVE;
                                    break;
                                }
                            }
                            break;
                        }
                        case EV_SYN:
                        {
                            switch (event.code)
                            {
                                case SYN_REPORT:
                                {
                                    for (Slot& slot : touchSlots)
                                    {
                                        if (slot.action != Slot::Action::NONE)
                                        {
                                            Vector2 position(static_cast<float>(slot.positionX - touchMinX) / touchRangeX,
                                                             static_cast<float>(slot.positionY - touchMinY) / touchRangeY);

                                            switch (slot.action)
                                            {
                                                case Slot::Action::NONE:
                                                    break;
                                                case Slot::Action::BEGIN:
                                                    touchpadDevice->handleTouchBegin(static_cast<uint64_t>(slot.trackingId), position, 1.0F);
                                                    break;
                                                case Slot::Action::END:
                                                    touchpadDevice->handleTouchEnd(static_cast<uint64_t>(slot.trackingId), position, 1.0F);
                                                    break;
                                                case Slot::Action::MOVE:
                                                    touchpadDevice->handleTouchMove(static_cast<uint64_t>(slot.trackingId), position, 1.0F);
                                                    break;
                                            }

                                            slot.action = Slot::Action::NONE;
                                        }
                                    }
                                    break;
                                }
                                case SYN_DROPPED:
                                {
                                    for (Slot& slot : touchSlots)
                                        slot.action = Slot::Action::NONE;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                if (gamepadDevice)
                {
                    switch(event.type)
                    {
                        case EV_ABS:
                        {
                            // TODO: implement
                            break;
                        }
                        case EV_REL:
                        {
                            // TODO: implement
                            break;
                        }
                        case EV_KEY:
                        {
                            // TODO: implement
                            break;
                        }
                    }
                }
            }
        }
    } // namespace input
} // namespace ouzel
