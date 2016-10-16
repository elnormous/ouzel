// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <GL/gl.h>
#include <GL/glext.h>
#include "ApplicationLinux.h"
#include "core/Engine.h"
#include "events/Event.h"
#include "input/Input.h"
#include "input/linux/InputLinux.h"
#include "WindowLinux.h"

namespace ouzel
{
    ApplicationLinux::ApplicationLinux(int pArgc, char* pArgv[]):
        Application(pArgc, pArgv)
    {
    }

    int ApplicationLinux::run()
    {
        ouzelMain(args);

        if (!sharedEngine)
        {
            return EXIT_FAILURE;
        }

        sharedEngine->begin();

        if (sharedEngine->getRenderer()->getDriver() == graphics::Renderer::Driver::EMPTY)
        {
            while (sharedEngine->isActive())
            {
                executeAll();

                if (!sharedEngine->draw())
                {
                    sharedEngine->exit();
                }
            }
        }
        else
        {
            XEvent event;

            WindowLinux* windowLinux = static_cast<WindowLinux*>(sharedEngine->getWindow());

            while (sharedEngine->isActive())
            {
                executeAll();

                if (!sharedEngine->draw())
                {
                    sharedEngine->exit();
                }

                while (sharedEngine->isActive())
                {
                    if ((sharedEngine->isRunning() && !XPending(windowLinux->getDisplay())) ||
                        !sharedEngine->isActive())
                    {
                        break;
                    }

                    XNextEvent(windowLinux->getDisplay(), &event);

                    switch (event.type)
                    {
                        case ClientMessage:
                        {
                            if (static_cast<Atom>(event.xclient.data.l[0]) == windowLinux->getDeleteMessage())
                            {
                                sharedEngine->exit();
                            }
                            break;
                        }
                        case FocusIn:
                            sharedEngine->resume();
                            break;
                        case FocusOut:
                            sharedEngine->pause();
                            break;
                        case KeyPress: // keyboard
                        case KeyRelease:
                        {
                            KeySym keySym = XkbKeycodeToKeysym(windowLinux->getDisplay(), event.xkey.keycode, 0, event.xkey.state & ShiftMask ? 1 : 0);

                            if (event.type == KeyPress)
                            {
                                sharedEngine->getInput()->keyDown(input::InputLinux::convertKeyCode(keySym),
                                                                  input::InputLinux::getModifiers(event.xkey.state));
                            }
                            else
                            {
                                sharedEngine->getInput()->keyUp(input::InputLinux::convertKeyCode(keySym),
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
                                sharedEngine->getInput()->mouseDown(button,
                                                                    sharedEngine->getWindow()->convertWindowToNormalizedLocation(pos),
                                                                    input::InputLinux::getModifiers(event.xbutton.state));
                            }
                            else
                            {
                                sharedEngine->getInput()->mouseUp(button,
                                                                  sharedEngine->getWindow()->convertWindowToNormalizedLocation(pos),
                                                                  input::InputLinux::getModifiers(event.xbutton.state));
                            }
                            break;
                        }
                        case MotionNotify:
                        {
                            Vector2 pos(static_cast<float>(event.xmotion.x),
                                        static_cast<float>(event.xmotion.y));

                            sharedEngine->getInput()->mouseMove(sharedEngine->getWindow()->convertWindowToNormalizedLocation(pos),
                                                                input::InputLinux::getModifiers(event.xmotion.state));

                            break;
                        }
                        case ConfigureNotify:
                        {
                            windowLinux->handleResize(event.xconfigure.width, event.xconfigure.height);
                            break;
                        }
                        case Expose:
                        {
                            // need redraw
                            break;
                        }
                    }
                }
            }
        }

        sharedEngine->end();

        return EXIT_SUCCESS;
    }
}
