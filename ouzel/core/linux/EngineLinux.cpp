// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"
#include <cstdlib>
#include <unistd.h>
#if OUZEL_SUPPORTS_X11
#  include "GL/gl.h"
#  include "GL/glext.h"
#  include <X11/XKBlib.h>
#  include <X11/extensions/scrnsaver.h>
#  include <X11/extensions/XInput2.h>
#endif
#include "EngineLinux.hpp"
#include "NativeWindowLinux.hpp"
#include "events/Event.hpp"
#include "graphics/RenderDevice.hpp"
#include "input/linux/InputManagerLinux.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    EngineLinux::EngineLinux(int initArgc, char* initArgv[])
    {
        for (int i = 0; i < initArgc; ++i)
            args.push_back(initArgv[i]);
    }

    EngineLinux::~EngineLinux()
    {
#if OUZEL_SUPPORTS_X11
        if (display) XCloseDisplay(display);
#else
        if (display != DISPMANX_NO_HANDLE)
            vc_dispmanx_display_close(display);

        bcm_host_deinit();
#endif
    }

    void EngineLinux::run()
    {
#if OUZEL_SUPPORTS_X11
        if (!XInitThreads())
            throw SystemError("Failed to initialize thread support");

        // open a connection to the X server
        display = XOpenDisplay(nullptr);

        if (!display)
            throw SystemError("Failed to open display");
#else
        bcm_host_init();
#endif

        init();
        start();

        input::InputManagerLinux* inputLinux = static_cast<input::InputManagerLinux*>(inputManager.get());

#if OUZEL_SUPPORTS_X11
        NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(window->getNativeWindow());

        int xInputOpCode = 0;
        int eventCode;
        int err;
        if (XQueryExtension(display, "XInputExtension", &xInputOpCode, &eventCode, &err))
        {
            int majorVersion = 2;
            int minorVersion = 0;

            XIQueryVersion(display, &majorVersion, &minorVersion);

            if (majorVersion >= 2)
            {
                unsigned char mask[] = {0, 0 ,0};

                XIEventMask eventMask;
                eventMask.deviceid = XIAllMasterDevices;
                eventMask.mask_len = sizeof(mask);
                eventMask.mask = mask;

                XISetMask(mask, XI_TouchBegin);
                XISetMask(mask, XI_TouchEnd);
                XISetMask(mask, XI_TouchUpdate);

                XISelectEvents(display, windowLinux->getNativeWindow(), &eventMask, 1);
            }
            else
                Log(Log::Level::WARN) << "XInput2 not supported";
        }
        else
            Log(Log::Level::WARN) << "XInput not supported";

        XEvent event;

        while (active)
        {
            // XNextEvent will block if there is no event pending, so don't call it if engine is not paused
            if (paused || XPending(display))
            {
                XNextEvent(display, &event);

                switch (event.type)
                {
                    case ClientMessage:
                    {
                        if (event.xclient.message_type == windowLinux->getProtocolsAtom() && static_cast<Atom>(event.xclient.data.l[0]) == windowLinux->getDeleteAtom())
                            exit();
                        else if (event.xclient.message_type == windowLinux->getExecuteAtom())
                            executeAll();
                        break;
                    }
                    case FocusIn:
                        resume();
                        break;
                    case FocusOut:
                        pause();
                        break;
                    case KeyPress: // keyboard
                    case KeyRelease:
                    {
                        KeySym keySym = XkbKeycodeToKeysym(display,
                                                           event.xkey.keycode, 0,
                                                           event.xkey.state & ShiftMask ? 1 : 0);

                        if (event.type == KeyPress)
                        {
                            inputManager->keyPress(input::InputManagerLinux::convertKeyCode(keySym),
                                                   input::InputManagerLinux::getModifiers(event.xkey.state));
                        }
                        else
                        {
                            inputManager->keyRelease(input::InputManagerLinux::convertKeyCode(keySym),
                                                     input::InputManagerLinux::getModifiers(event.xkey.state));
                        }
                        break;
                    }
                    case ButtonPress: // mouse button
                    case ButtonRelease:
                    {
                        Vector2 pos(static_cast<float>(event.xbutton.x),
                                    static_cast<float>(event.xbutton.y));

                        input::MouseButton button;

                        switch (event.xbutton.button)
                        {
                        case 1:
                            button = input::MouseButton::LEFT;
                            break;
                        case 2:
                            button = input::MouseButton::RIGHT;
                            break;
                        case 3:
                            button = input::MouseButton::MIDDLE;
                            break;
                        default:
                            button = input::MouseButton::NONE;
                            break;
                        }

                        if (event.type == ButtonPress)
                        {
                            inputManager->mouseButtonPress(button,
                                                           window->convertWindowToNormalizedLocation(pos),
                                                           input::InputManagerLinux::getModifiers(event.xbutton.state));
                        }
                        else
                        {
                            inputManager->mouseButtonRelease(button,
                                                             window->convertWindowToNormalizedLocation(pos),
                                                             input::InputManagerLinux::getModifiers(event.xbutton.state));
                        }
                        break;
                    }
                    case MotionNotify:
                    {
                        Vector2 pos(static_cast<float>(event.xmotion.x),
                                    static_cast<float>(event.xmotion.y));

                        inputManager->mouseMove(window->convertWindowToNormalizedLocation(pos),
                                                input::InputManagerLinux::getModifiers(event.xmotion.state));

                        break;
                    }
                    case ConfigureNotify:
                    {
                        windowLinux->handleResize(Size2(static_cast<float>(event.xconfigure.width),
                                                        static_cast<float>(event.xconfigure.height)));
                        break;
                    }
                    case Expose:
                    {
                        // need to redraw
                        break;
                    }
                    case GenericEvent:
                    {
                        XGenericEventCookie* cookie = &event.xcookie;
                        if (cookie->extension == xInputOpCode)
                        {
                            switch (cookie->evtype)
                            {
                                case XI_TouchBegin:
                                {
                                    XIDeviceEvent* xievent = reinterpret_cast<XIDeviceEvent*>(cookie->data);
                                    inputManager->touchBegin(xievent->detail,
                                                             window->convertWindowToNormalizedLocation(Vector2(static_cast<float>(xievent->event_x),
                                                                                                                            static_cast<float>(xievent->event_y))));
                                    break;
                                }
                                case XI_TouchEnd:
                                {
                                    XIDeviceEvent* xievent = reinterpret_cast<XIDeviceEvent*>(cookie->data);
                                    inputManager->touchEnd(xievent->detail,
                                                           window->convertWindowToNormalizedLocation(Vector2(static_cast<float>(xievent->event_x),
                                                                                                                          static_cast<float>(xievent->event_y))));
                                    break;
                                }
                                case XI_TouchUpdate:
                                {
                                    XIDeviceEvent* xievent = reinterpret_cast<XIDeviceEvent*>(cookie->data);
                                    inputManager->touchMove(xievent->detail,
                                                            window->convertWindowToNormalizedLocation(Vector2(static_cast<float>(xievent->event_x),
                                                                                                                           static_cast<float>(xievent->event_y))));
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }

                inputLinux->update();
            }
        }
#else
        while (active)
        {
            executeAll();

            inputLinux->update();
        }
#endif

        exit();
    }

    void EngineLinux::executeOnMainThread(const std::function<void(void)>& func)
    {
#if OUZEL_SUPPORTS_X11
        NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(window->getNativeWindow());

        XEvent event;
        event.type = ClientMessage;
        event.xclient.window = windowLinux->getNativeWindow();
        event.xclient.message_type = windowLinux->getExecuteAtom();
        event.xclient.format = 32; // data is set as 32-bit integers
        event.xclient.data.l[0] = 0;
        event.xclient.data.l[1] = CurrentTime;
        event.xclient.data.l[2] = 0; // unused
        event.xclient.data.l[3] = 0; // unused
        event.xclient.data.l[4] = 0; // unused

        std::unique_lock<std::mutex> lock(executeMutex);

        executeQueue.push(func);

        if (!XSendEvent(display, windowLinux->getNativeWindow(), False, NoEventMask, &event))
            throw SystemError("Failed to send X11 delete message");

        XFlush(display);
#else
        std::unique_lock<std::mutex> lock(executeMutex);

        executeQueue.push(func);
#endif
    }

    void EngineLinux::openURL(const std::string& url)
    {
#if OUZEL_SUPPORTS_X11
		::exit(execl("/usr/bin/xdg-open", "xdg-open", url.c_str(), nullptr));
#endif
	}

	void EngineLinux::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        Engine::setScreenSaverEnabled(newScreenSaverEnabled);

#if OUZEL_SUPPORTS_X11
        executeOnMainThread([this, newScreenSaverEnabled]() {
            XScreenSaverSuspend(display, !newScreenSaverEnabled);
        });
#endif
    }

    void EngineLinux::executeAll()
    {
        std::function<void(void)> func;

        for (;;)
        {
            {
                std::unique_lock<std::mutex> lock(executeMutex);

                if (executeQueue.empty())
                    break;

                func = std::move(executeQueue.front());
                executeQueue.pop();
            }

            if (func) func();
        }
    }
}
