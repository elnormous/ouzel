// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "EventDevice.hpp"
#include "InputSystemLinux.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define BITS_PER_LONG (8 * sizeof(long))
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_LONG)

static inline bool isBitSet(const unsigned long* array, int bit)
{
    return !!(array[bit / BITS_PER_LONG] & (1LL << (bit % BITS_PER_LONG)));
}

namespace ouzel
{
    namespace input
    {
        EventDevice::EventDevice(const std::string& initFilename):
            filename(initFilename)
        {
            fd = open(filename.c_str(), O_RDONLY);

            if (fd == -1)
                throw SystemError("Failed to open device file");

            if (ioctl(fd, EVIOCGRAB, reinterpret_cast<void*>(1)) == -1)
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
            {
                Log(Log::Level::INFO) << "Device class: keyboard";
                deviceClass |= EventDevice::CLASS_KEYBOARD;
            }

            if (isBitSet(eventBits, EV_ABS) && isBitSet(absBits, ABS_X) && isBitSet(absBits, ABS_Y))
            {
                if (isBitSet(keyBits, BTN_STYLUS) || isBitSet(keyBits, BTN_TOOL_PEN))
                {
                    Log(Log::Level::INFO) << "Device class: tablet";
                    deviceClass |= EventDevice::CLASS_TOUCHPAD;
                }
                else if (isBitSet(keyBits, BTN_TOOL_FINGER) && !isBitSet(keyBits, BTN_TOOL_PEN))
                {
                    Log(Log::Level::INFO) << "Device class: touchpad";
                    deviceClass |= EventDevice::CLASS_TOUCHPAD;
                }
                else if (isBitSet(keyBits, BTN_MOUSE))
                {
                    Log(Log::Level::INFO) << "Device class: mouse";
                    deviceClass |= EventDevice::CLASS_MOUSE;
                }
                else if (isBitSet(keyBits, BTN_TOUCH))
                {
                    Log(Log::Level::INFO) << "Device class: touchscreen";
                    deviceClass |= EventDevice::CLASS_TOUCHPAD;
                }
            }
            else if (isBitSet(eventBits, EV_REL) && isBitSet(relBits, REL_X) && isBitSet(relBits, REL_Y))
            {
                if (isBitSet(keyBits, BTN_MOUSE))
                {
                    Log(Log::Level::INFO) << "Device class: mouse";
                    deviceClass |= EventDevice::CLASS_MOUSE;
                }
            }

            if (isBitSet(keyBits, BTN_JOYSTICK))
            {
                Log(Log::Level::INFO) << "Device class: joystick";
                deviceClass |= EventDevice::CLASS_GAMEPAD;
            }

            if (isBitSet(keyBits, BTN_GAMEPAD))
            {
                Log(Log::Level::INFO) << "Device class: gamepad";
                deviceClass |= EventDevice::CLASS_GAMEPAD;
            }

            struct input_id id;
            ioctl(fd, EVIOCGID, &id);
            vendor = id.vendor;
            product = id.product;
        }

        EventDevice::~EventDevice()
        {
            if (fd != -1)
            {
                if (ioctl(fd, EVIOCGRAB, reinterpret_cast<void*>(0)) == -1)
                    Log(Log::Level::WARN) << "Failed to release device";

                close(fd);
            }
        }

        void EventDevice::update()
        {
            // TODO: buffer data
            // TODO: move this code to EventDevice
            static char TEMP_BUFFER[256];
            ssize_t bytesRead = read(fd, TEMP_BUFFER, sizeof(TEMP_BUFFER));

            if (bytesRead == -1)
                throw SystemError("Failed to read from " + filename); // TODO: disconnect the device

            for (ssize_t i = 0; i < bytesRead - static_cast<ssize_t>(sizeof(input_event)) + 1; i += sizeof(input_event))
            {
                input_event* event = reinterpret_cast<input_event*>(TEMP_BUFFER + i);

                if (deviceClass & EventDevice::CLASS_KEYBOARD)
                {
                    if (event->type == EV_KEY)
                    {
                        if (event->value == 1 || event->value == 2) // press or repeat
                        {
                            //keyPress(convertKeyCode(event->code), getModifiers());
                        }
                        else if (event->value == 0) // release
                        {
                            //keyRelease(convertKeyCode(event->code), getModifiers());
                        }
                    }
                }
                if (deviceClass & EventDevice::CLASS_MOUSE)
                {
                    if (event->type == EV_ABS)
                    {
                        /*Vector2 absolutePos = cursorPosition;

                        if (event->code == ABS_X)
                            absolutePos.x = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(event->value), 0.0F)).x;
                        else if (event->code == ABS_Y)
                            absolutePos.y = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(0.0F, static_cast<float>(event->value))).y;*/

                        //mouseMove(absolutePos, getModifiers());
                    }
                    else if (event->type == EV_REL)
                    {
                        Vector2 relativePos;

                        if (event->code == REL_X)
                            relativePos.x = static_cast<float>(event->value);
                        else if (event->code == REL_Y)
                            relativePos.y = static_cast<float>(event->value);

                        //mouseRelativeMove(engine->getWindow()->convertWindowToNormalizedLocation(relativePos), getModifiers());
                    }
                    else if (event->type == EV_KEY)
                    {
                        Mouse::Button button;
                        int buttonIndex = -1;

                        switch (event->code)
                        {
                        case BTN_LEFT:
                            button = Mouse::Button::LEFT;
                            buttonIndex = 0;
                            break;
                        case BTN_RIGHT:
                            button = Mouse::Button::RIGHT;
                            buttonIndex =  1;
                            break;
                        case BTN_MIDDLE:
                            button = Mouse::Button::MIDDLE;
                            buttonIndex = 2;
                            break;
                        default:
                            button = Mouse::Button::NONE;
                        }

                        if (event->value == 1)
                        {
                            //mouseButtonPress(button, cursorPosition, getModifiers());
                        }
                        else if (event->value == 0)
                        {
                            //mouseButtonRelease(button, cursorPosition, getModifiers());
                        }
                    }
                }
                if (deviceClass & EventDevice::CLASS_TOUCHPAD)
                {
                    // TODO: implement
                }
                if (deviceClass & EventDevice::CLASS_GAMEPAD)
                {
                    // TODO: implement
                }
            }
        }
    } // namespace input
} // namespace ouzel
