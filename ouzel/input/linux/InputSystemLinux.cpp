// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>
#include <linux/joystick.h>
#if OUZEL_SUPPORTS_X11
#  include <X11/cursorfont.h>
#endif
#include "InputSystemLinux.hpp"
#include "NativeCursorLinux.hpp"
#include "core/linux/EngineLinux.hpp"
#include "core/linux/NativeWindowLinux.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace input
    {
        InputSystemLinux::InputSystemLinux(EventHandler& initEventHandler):
#if OUZEL_SUPPORTS_X11
            InputSystem(initEventHandler),
            keyboardDevice(new KeyboardDeviceLinux(*this, ++lastDeviceId)),
            mouseDevice(new MouseDeviceLinux(*this, ++lastDeviceId)),
            touchpadDevice(new TouchpadDevice(*this, ++lastDeviceId))
#else
            InputSystem(initEventHandler)
#endif
        {
#if OUZEL_SUPPORTS_X11
            EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
            Display* display = engineLinux->getDisplay();

            char data[1] = {0};

            Pixmap pixmap = XCreateBitmapFromData(display, DefaultRootWindow(display), data, 1, 1);
            if (pixmap)
            {
                XColor color;
                color.red = color.green = color.blue = 0;

                emptyCursor = XCreatePixmapCursor(display, pixmap, pixmap, &color, &color, 0, 0);
                XFreePixmap(display, pixmap);
            }
#endif
            DIR* dir = opendir("/dev/input");

            if (!dir)
                throw SystemError("Failed to open directory");

            dirent ent;
            dirent* p;

            while (readdir_r(dir, &ent, &p) == 0 && p)
            {
                if (strncmp("event", ent.d_name, 5) == 0)
                {
                    try
                    {
                        std::string filename = std::string("/dev/input/") + ent.d_name;
                        std::unique_ptr<EventDevice> eventDevice(new EventDevice(*this, filename));
                        eventDevices.insert(std::make_pair(eventDevice->getFd(), std::move(eventDevice)));
                    }
                    catch (const std::exception&)
                    {
                    }
                }
            }

            closedir(dir);
        }

        InputSystemLinux::~InputSystemLinux()
        {
#if OUZEL_SUPPORTS_X11
            if (engine)
            {
                EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
                if (emptyCursor != None) XFreeCursor(engineLinux->getDisplay(), emptyCursor);
            }
#endif
        }

        void InputSystemLinux::executeCommand(const Command& command)
        {
            switch (command.type)
            {
                case Command::Type::START_DEVICE_DISCOVERY:
                    discovering = true;
                    break;
                case Command::Type::STOP_DEVICE_DISCOVERY:
                    discovering = false;
                    break;
                case Command::Type::SET_PLAYER_INDEX:
                {
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
                        std::unique_ptr<NativeCursorLinux> cursor(new NativeCursorLinux(command.systemCursor));
                        cursors[command.cursorResource - 1] = std::move(cursor);
                    }
                    else
                    {
                        std::unique_ptr<NativeCursorLinux> cursor(new NativeCursorLinux(command.data, command.size,
                                                                                        command.pixelFormat, command.hotSpot));
                        cursors[command.cursorResource - 1] = std::move(cursor);
                    }
                    break;
                }
                case Command::Type::DESTROY_CURSOR:
                {
#if OUZEL_SUPPORTS_X11
                    NativeCursorLinux* cursor = cursors[command.cursorResource - 1].get();

                    if (mouseDevice->getCursor() == cursor)
                    {
                        mouseDevice->setCursor(nullptr);
                        updateCursor();
                    }
#endif

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
#if OUZEL_SUPPORTS_X11
                            updateCursor();
#endif
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

        void InputSystemLinux::update()
        {
            fd_set rfds;
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;

            FD_ZERO(&rfds);

            int maxFd = 0;

            for (const auto& i : eventDevices)
            {
                FD_SET(i.first, &rfds);
                if (i.first > maxFd) maxFd = i.first;
            }

            int retval = select(maxFd + 1, &rfds, nullptr, nullptr, &tv);

            if (retval == -1)
                throw SystemError("Select failed");
            else if (retval > 0)
            {
                for (auto i = eventDevices.begin(); i != eventDevices.end();)
                {
                    if (FD_ISSET(i->first, &rfds))
                    {
                        try
                        {
                            i->second->update();
                            ++i;
                        }
                        catch (const std::exception&)
                        {
                            i = eventDevices.erase(i);
                        }
                    }
                    else
                        ++i;
                }
            }

            if (discovering)
            {
                DIR* dir = opendir("/dev/input");

                if (!dir)
                    throw SystemError("Failed to open directory");

                dirent ent;
                dirent* p;

                while (readdir_r(dir, &ent, &p) == 0 && p)
                {
                    if (strncmp("event", ent.d_name, 5) == 0)
                    {
                        try
                        {
                            std::string filename = std::string("/dev/input/") + ent.d_name;
                            std::unique_ptr<EventDevice> eventDevice(new EventDevice(*this, filename));
                            eventDevices.insert(std::make_pair(eventDevice->getFd(), std::move(eventDevice)));
                        }
                        catch (const std::exception&)
                        {
                        }
                    }
                }

                closedir(dir);
            }
        }

#if OUZEL_SUPPORTS_X11
        void InputSystemLinux::updateCursor() const
        {
            EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
            NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(engine->getWindow()->getNativeWindow());
            Display* display = engineLinux->getDisplay();
            ::Window window = windowLinux->getNativeWindow();

            if (mouseDevice->isCursorVisible())
            {
                if (mouseDevice->getCursor())
                    XDefineCursor(display, window, mouseDevice->getCursor()->getNativeCursor());
                else
                    XUndefineCursor(display, window);
            }
            else
                XDefineCursor(display, window, emptyCursor);
        }
#endif
    } // namespace input
} // namespace ouzel
