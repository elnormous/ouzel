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
    ApplicationLinux::ApplicationLinux(int aArgc, char* aArgv[]):
        Application(aArgc, aArgv)
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
            for (;;)
            {
                executeAll();

                if (!sharedEngine->draw())
                {
                    sharedEngine->exit();
                    break;
                }
            }
        }
        else
        {
            XEvent event;

            WindowLinux* windowLinux = static_cast<WindowLinux*>(sharedEngine->getWindow());

            while (active)
            {
                executeAll();

                if (sharedEngine->draw())
                {
                    while (sharedEngine->isActive())
                    {
                        // XNextEvent will block if there is no event pending, so don't call it if engine is running
                        if (sharedEngine->isRunning() && !XPending(windowLinux->getDisplay()))
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
                                windowLinux->handleResize(Size2(static_cast<float>(event.xconfigure.width),
                                                                static_cast<float>(event.xconfigure.height)));
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
                else
                {
                    break;
                }
            }
        }

        sharedEngine->end();

        return EXIT_SUCCESS;
    }
}
