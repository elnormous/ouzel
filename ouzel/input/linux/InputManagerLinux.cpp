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
#else
#  include <linux/input.h>
#endif
#include "InputManagerLinux.hpp"
#include "InputSystemLinux.hpp"
#include "NativeCursorLinux.hpp"
#include "events/Event.hpp"
#include "core/linux/EngineLinux.hpp"
#include "core/linux/NativeWindowLinux.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace input
    {
        InputManagerLinux::InputManagerLinux()
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
        }

        InputManagerLinux::~InputManagerLinux()
        {
#if OUZEL_SUPPORTS_X11
            if (engine)
            {
                EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
                if (emptyCursor != None) XFreeCursor(engineLinux->getDisplay(), emptyCursor);
            }
#endif
        }

        void InputManagerLinux::activateNativeCursor(NativeCursor* resource)
        {
            InputManager::activateNativeCursor(resource);

#if OUZEL_SUPPORTS_X11
            if (engine)
            {
                EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
                NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(engine->getWindow()->getNativeWindow());
                Display* display = engineLinux->getDisplay();
                ::Window window = windowLinux->getNativeWindow();

                NativeCursorLinux* cursorLinux = static_cast<NativeCursorLinux*>(resource);

                if (cursorLinux)
                {
                    currentCursor = cursorLinux->getNativeCursor();
                    if (!currentCursor) currentCursor = emptyCursor;
                }
                else
                    currentCursor = None;

                if (cursorVisible)
                {
                    if (currentCursor)
                        XDefineCursor(display, window, currentCursor);
                    else
                        XUndefineCursor(display, window);
                }
                else
                    XDefineCursor(display, window, emptyCursor);
            }
#endif
        }

        NativeCursor* InputManagerLinux::createNativeCursor()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            std::unique_ptr<NativeCursorLinux> cursorResource(new NativeCursorLinux(*this));
            NativeCursor* result = cursorResource.get();

            resources.push_back(std::move(cursorResource));

            return result;
        }

        void InputManagerLinux::setCursorVisible(bool visible)
        {
            if (visible != cursorVisible)
            {
                cursorVisible = visible;

#if OUZEL_SUPPORTS_X11
                engine->executeOnMainThread([visible, this] {
                    EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
                    NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(engine->getWindow()->getNativeWindow());
                    Display* display = engineLinux->getDisplay();
                    ::Window window = windowLinux->getNativeWindow();

                    if (visible)
                    {
                        if (currentCursor)
                            XDefineCursor(display, window, currentCursor);
                        else
                            XUndefineCursor(display, window);
                    }
                    else
                        XDefineCursor(display, window, emptyCursor);

                    XFlush(display);
                });
#endif
            }
        }

        bool InputManagerLinux::isCursorVisible() const
        {
            return cursorVisible;
        }

        void InputManagerLinux::setCursorLocked(bool locked)
        {
#if OUZEL_SUPPORTS_X11
            engine->executeOnMainThread([locked] {
                EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
                NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(engine->getWindow()->getNativeWindow());
                Display* display = engineLinux->getDisplay();
                ::Window window = windowLinux->getNativeWindow();

                if (locked)
                {
                    if (XGrabPointer(display, window, False,
                                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask,
                                     GrabModeAsync, GrabModeAsync,
                                     None, None, CurrentTime) != GrabSuccess)
                        throw SystemError("Failed to grab pointer");
                }
                else
                    XUngrabPointer(display, CurrentTime);

                XSync(display, False);
            });
#endif
            cursorLocked = locked;
        }

        bool InputManagerLinux::isCursorLocked() const
        {
            return cursorLocked;
        }

       /*void InputManagerLinux::setCursorPosition(const Vector2& position)
        {
            InputManager::setCursorPosition(position);

#if OUZEL_SUPPORTS_X11
            engine->executeOnMainThread([position] {
                EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
                NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(engine->getWindow()->getNativeWindow());
                Display* display = engineLinux->getDisplay();
                ::Window window = windowLinux->getNativeWindow();

                XWindowAttributes attributes;
                XGetWindowAttributes(display, window, &attributes);

                Vector2 windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);

                XWarpPointer(display, None, window, 0, 0, 0, 0,
                             attributes.x + static_cast<int>(windowLocation.x),
                             attributes.y + static_cast<int>(windowLocation.y));
                XSync(display, False);
            });
#endif
        }

        void InputManagerLinux::startDeviceDiscovery()
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

                        bool found = false;
                        for (const EventDevice& inputDevice : inputDevices)
                        {
                            if (inputDevice.getFilename() == filename)
                            {
                                found = true;
                                break;
                            }
                        }

                        if (!found)
                        {
                            EventDevice inputDevice(filename);

                            if (inputDevice.getDeviceClass() != EventDevice::CLASS_NONE)
                            {
                                if (inputDevice.getFd() > maxFd) maxFd = inputDevice.getFd();

                                inputDevices.push_back(std::move(inputDevice));
                            }
                        }
                    }
                    catch (...)
                    {
                    }
                }
            }

            closedir(dir);
        }*/
    } // namespace input
} // namespace ouzel
