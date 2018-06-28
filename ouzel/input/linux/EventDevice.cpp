// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "EventDevice.hpp"
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
        EventDevice::EventDevice(const std::string& filename)
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
                deviceClass = EventDevice::CLASS_GAMEPAD;
            }

            if (isBitSet(keyBits, BTN_GAMEPAD))
            {
                Log(Log::Level::INFO) << "Device class: gamepad";
                deviceClass = EventDevice::CLASS_GAMEPAD;
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
    } // namespace input
} // namespace ouzel
