// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <X11/XKBlib.h>
#include <X11/extensions/scrnsaver.h>
#include "EngineLinux.hpp"
#include "events/Event.hpp"
#include "graphics/RenderDevice.hpp"
#include "input/Input.hpp"
#include "input/linux/InputLinux.hpp"
#include "WindowResourceLinux.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    EngineLinux::EngineLinux(int aArgc, char* aArgv[])
    {
        argc = aArgc;
        argv = aArgv;

        for (int i = 0; i < aArgc; ++i)
        {
            args.push_back(aArgv[i]);
        }
    }

    int EngineLinux::run()
    {
        if (!XInitThreads())
        {
            Log(Log::Level::ERR) << "Failed to initialize thread support";
            return false;
        }

        if (!init())
        {
            return EXIT_FAILURE;
        }

        start();

        /*for (;;)
        {
            executeAll();
        }*/

        XEvent event;

        WindowResourceLinux* windowLinux = static_cast<WindowResourceLinux*>(window.getResource());
        input::InputLinux* inputLinux = static_cast<input::InputLinux*>(input.get());

        while (active)
        {
            executeAll();

            // XNextEvent will block if there is no event pending, so don't call it if engine is not paused
            if (paused || XPending(windowLinux->getDisplay()))
            {
                XNextEvent(windowLinux->getDisplay(), &event);

                switch (event.type)
                {
                    case ClientMessage:
                    {
                        if (static_cast<Atom>(event.xclient.data.l[0]) == windowLinux->getDeleteMessage())
                        {
                            exit();
                        }
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
                        KeySym keySym = XkbKeycodeToKeysym(windowLinux->getDisplay(),
                                                           event.xkey.keycode, 0,
                                                           event.xkey.state & ShiftMask ? 1 : 0);

                        if (event.type == KeyPress)
                        {
                            input->keyPress(input::InputLinux::convertKeyCode(keySym),
                                            input::InputLinux::getModifiers(event.xkey.state));
                        }
                        else
                        {
                            input->keyRelease(input::InputLinux::convertKeyCode(keySym),
                                              input::InputLinux::getModifiers(event.xkey.state));
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
                            input->mouseButtonPress(button,
                                                    window.convertWindowToNormalizedLocation(pos),
                                                    input::InputLinux::getModifiers(event.xbutton.state));
                        }
                        else
                        {
                            input->mouseButtonRelease(button,
                                                      window.convertWindowToNormalizedLocation(pos),
                                                      input::InputLinux::getModifiers(event.xbutton.state));
                        }
                        break;
                    }
                    case MotionNotify:
                    {
                        Vector2 pos(static_cast<float>(event.xmotion.x),
                                    static_cast<float>(event.xmotion.y));

                        input->mouseMove(window.convertWindowToNormalizedLocation(pos),
                                         input::InputLinux::getModifiers(event.xmotion.state));

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
                        inputLinux->handleXInput2Event(cookie);
                        break;
                    }
                }
            }
        }

        exit();

        return EXIT_SUCCESS;
    }

    void EngineLinux::executeOnMainThread(const std::function<void(void)>& func)
    {
        std::lock_guard<std::mutex> lock(executeMutex);

        executeQueue.push(func);
    }

    bool EngineLinux::openURL(const std::string& url)
    {
		::exit(execl("/usr/bin/xdg-open", "xdg-open", url.c_str(), nullptr));

		return true;
	}

	void EngineLinux::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        Engine::setScreenSaverEnabled(newScreenSaverEnabled);

        executeOnMainThread([this, newScreenSaverEnabled]() {
            WindowResourceLinux* windowLinux = static_cast<WindowResourceLinux*>(window.getResource());

            XScreenSaverSuspend(windowLinux->getDisplay(), !newScreenSaverEnabled);
        });
    }

    bool EngineLinux::setCurrentThreadName(const std::string& name)
    {
        return pthread_setname_np(pthread_self(), name.c_str()) == 0;
    }

    void EngineLinux::executeAll()
    {
        std::function<void(void)> func;

        for (;;)
        {
            {
                std::lock_guard<std::mutex> lock(executeMutex);

                if (executeQueue.empty())
                {
                    break;
                }

                func = std::move(executeQueue.front());
                executeQueue.pop();
            }

            if (func)
            {
                func();
            }
        }
    }
}
