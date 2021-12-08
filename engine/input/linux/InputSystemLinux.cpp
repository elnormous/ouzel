// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"
#include <cstdio>
#include <cstring>
#include <system_error>
#include <unordered_map>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#if OUZEL_SUPPORTS_X11
#  include <X11/cursorfont.h>
#endif
#include "InputSystemLinux.hpp"
#include "CursorLinux.hpp"
#include "../../core/linux/EngineLinux.hpp"
#include "../../core/linux/NativeWindowLinux.hpp"

namespace ouzel::input::linux
{
#if OUZEL_SUPPORTS_X11
    InputSystem::InputSystem():
        keyboardDevice{std::make_unique<KeyboardDevice>(*this, getNextDeviceId())},
        mouseDevice{std::make_unique<MouseDevice>(*this, getNextDeviceId())},
        touchpadDevice{std::make_unique<TouchpadDevice>(*this, getNextDeviceId(), true)}
#else
    InputSystem::InputSystem()
#endif
    {
#if OUZEL_SUPPORTS_X11
        const auto engineLinux = static_cast<core::linux::Engine*>(engine);
        auto display = engineLinux->getDisplay();

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

        using CloseDirFunction = int(*)(DIR*);
        std::unique_ptr<DIR, CloseDirFunction> dir(opendir("/dev/input"), &closedir);
        if (!dir)
            throw std::system_error(errno, std::system_category(), "Failed to open directory");

        while (const dirent* ent = readdir(dir.get()))
        {
            if (std::strncmp("event", ent->d_name, 5) == 0)
            {
                try
                {
                    std::string filename = std::string("/dev/input/") + ent->d_name;
                    auto eventDevice = std::make_unique<EventDevice>(*this, filename);
                    eventDevices.insert(std::pair(eventDevice->getFd(), std::move(eventDevice)));
                }
                catch (const std::exception&)
                {
                }
            }
        }
    }

    InputSystem::~InputSystem()
    {
#if OUZEL_SUPPORTS_X11
        const auto engineLinux = static_cast<core::linux::Engine*>(engine);
        if (emptyCursor != None) XFreeCursor(engineLinux->getDisplay(), emptyCursor);
#endif
    }

    void InputSystem::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::startDeviceDiscovery:
                discovering = true;
                break;
            case Command::Type::stopDeviceDiscovery:
                discovering = false;
                break;
            case Command::Type::setPlayerIndex:
            {
                break;
            }
            case Command::Type::setVibration:
            {
                break;
            }
            case Command::Type::setPosition:
            {
                if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                {
                    if (inputDevice == mouseDevice.get())
                        mouseDevice->setPosition(command.position);
                }
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
#if OUZEL_SUPPORTS_X11
                Cursor* cursor = cursors[command.cursorResource - 1].get();

                if (mouseDevice->getCursor() == cursor)
                {
                    mouseDevice->setCursor(nullptr);
                    updateCursor();
                }
#endif

                cursors[command.cursorResource - 1].reset();
                break;
            }
            case Command::Type::setCursor:
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
            case Command::Type::setCursorVisible:
            {
                if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                {
                    if (inputDevice == mouseDevice.get())
                        mouseDevice->setCursorVisible(command.visible);
                }
                break;
            }
            case Command::Type::setCursorLocked:
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

    void InputSystem::update()
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

        int retval;
        if ((retval = select(maxFd + 1, &rfds, nullptr, nullptr, &tv)) == -1)
            throw std::system_error(errno, std::system_category(), "Select failed");

        if (retval > 0)
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
            using CloseDirFunction = int(*)(DIR*);
            std::unique_ptr<DIR, CloseDirFunction> dir(opendir("/dev/input"), &closedir);

            if (!dir)
                throw std::system_error(errno, std::system_category(), "Failed to open directory");

            while (const dirent* ent = readdir(dir.get()))
            {
                if (std::strncmp("event", ent->d_name, 5) == 0)
                {
                    try
                    {
                        std::string filename = std::string("/dev/input/") + ent->d_name;
                        auto eventDevice = std::make_unique<EventDevice>(*this, filename);
                        eventDevices.insert(std::pair(eventDevice->getFd(), std::move(eventDevice)));
                    }
                    catch (const std::exception&)
                    {
                    }
                }
            }
        }
    }

#if OUZEL_SUPPORTS_X11
    void InputSystem::updateCursor() const
    {
        const auto engineLinux = static_cast<core::linux::Engine*>(engine);
        const auto windowLinux = static_cast<core::linux::NativeWindow*>(engine->getWindow().getNativeWindow());
        auto display = engineLinux->getDisplay();
        auto window = windowLinux->getNativeWindow();

        if (mouseDevice->isCursorVisible())
        {
            if (mouseDevice->getCursor())
                XDefineCursor(display, window, mouseDevice->getCursor()->getCursor());
            else
                XUndefineCursor(display, window);
        }
        else
            XDefineCursor(display, window, emptyCursor);
    }
#endif
}
