// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <GL/glx.h>
#include <GL/gl.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include "Engine.h"

static uint32_t getModifiers(unsigned int state)
{
    uint32_t modifiers = 0;
    
    if (state & ShiftMask) modifiers |= Event::SHIFT_DOWN;
    if (state & ControlMask) modifiers |= Event::CONTROL_DOWN;
    if (state & Button1) modifiers |= Event::LEFT_MOUSE_DOWN;
    if (state & Button2) modifiers |= Event::RIGHT_MOUSE_DOWN;
    if (state & Button3) modifiers |= Event::MIDDLE_MOUSE_DOWN;

    return modifiers;
}

int main(int argc, char **argv)
{
    std::vector<std::string> args;
    
    for (int32_t i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }
    
    ouzel::Engine::getInstance()->setArgs(args);
    
    // open a connection to the X server
    Display* display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        ouzel::log("Failed to open display");
        return 1;
    }

    // make sure OpenGL's GLX extension supported
    if (!glXQueryExtension(display, &dummy, &dummy))
    {
        ouzel::log("X server has no OpenGL GLX extension");
        return 1;
    }

    // find an OpenGL-capable RGB visual with depth buffer
    static int doubleBuffer[]  = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
    
    XVisualInfo* vi = glXChooseVisual(display, DefaultScreen(display), doubleBuffer);
    if (vi == NULL)
    {
        ouzel::log("No RGB visual with depth buffer");
        return 1;
    }
    if (vi->class != TrueColor)
    {
        ouzel::log("TrueColor visual required for this program");
        return 1;
    }

    // create an OpenGL rendering context
    GLXContext context = glXCreateContext(display, vi, /* no shared dlists */ None,
                                          /* direct rendering if possible */ GL_TRUE);
    if (context == NULL)
    {
        ouzel::log("Failed to create rendering context");
        return 1;
    }
    
    // create an X colormap since probably not using default visual
    Colormap cmap = XCreateColormap(display, RootWindow(display, vi->screen), vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.border_pixel = 0;
    swa.event_mask = KeyPressMask | KeyRelease | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;
    Window window = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0,
                                  300, 300, 0, vi->depth, InputOutput, vi->visual,
                                  CWBorderPixel | CWColormap | CWEventMask, &swa);
    XSetStandardProperties(display, window, "ouzel", "ouzel", None, argv, argc, NULL);

    // bind the rendering context to the window
    glXMakeCurrent(display, window, context);

    // request the X window to be displayed on the screen
    XMapWindow(display, window);
    
    ouzel::Engine::getInstance()->init();
    ouzel::Engine::getInstance()->begin();

    MSG msg;

    XEvent event;
    
    while (true)
    {
        do
        {
            XNextEvent(display, &event);
            switch (event.type)
            {
                case KeyPress: // keyboard
                case KeyRelease:
                {
                    KeySym     keysym;
                    char       buffer[1];

                    XKeyEvent* keyEvent = static_cast<XKeyEvent*>(&event);
                    //if ((XLookupString(keyEvent, buffer, 1, &keysym, NULL) == 1) && (keysym == (KeySym)XK_Escape))
                    //    running = false;
                    break;
                }
                case ButtonPress: // mouse button
                case ButtonRelease:
                {
                    XButtonEvent* buttonEvent = static_cast<XKeyEvent*>(&event);
                    
                    Vector2 pos(static_cast<float>(buttonEvent->x),
                                static_cast<float>(buttonEvent->y));
                
                    MouseButton button;
                    
                    switch (event.xbutton.button)
                    {
                    case 1:
                        button = MouseButton::LEFT;
                        break;
                    case 2:
                        button = MouseButton::RIGHT;
                        break;
                    case 3:
                        button = MouseButton::MIDDLE;
                        break;
                    }
                    
                    if (event.type == ButtonPress)
                    {
                        Engine::getInstance()->getInput()->mouseDown(button,
                                                                     Engine::getInstance()->getRenderer()->viewToScreenLocation(pos),
                                                                     getModifiers(buttonEvent->state));
                    }
                    else
                    {
                        Engine::getInstance()->getInput()->mouseUp(button,
                                                                   Engine::getInstance()->getRenderer()->viewToScreenLocation(pos),
                                                                   getModifiers(buttonEvent->state));
                    }
                    break;
                }
                case MotionNotify:
                {
                    XMotionEvent* motionEvent = static_cast<XMotionEvent*>(&event);
                    
                    Vector2 pos(static_cast<float>(motionEvent->x),
                                static_cast<float>(motionEvent->y));
                                
                    Engine::getInstance()->getInput()->mouseMove(Engine::getInstance()->getRenderer()->viewToScreenLocation(pos),
                                                                 getModifiers(motionEvent->state));
                    
                    break;
                }
                case ConfigureNotify:
                {
                    Engine::getInstance->getRenderer()->resize(ouzel::Size2(event.xconfigure.width, event.xconfigure.height));
                    //needRedraw = true;
                    break;
                }
                case Expose:
                {
                    // needRedraw = true;
                    break;
                }
          }
        }
        while (XPending(display)); /* loop to compress events */

        if (!ouzel::Engine::getInstance()->run())
        {
            break;
        }
        glXSwapBuffers(display, window);
    }

    if (display)
    {
        if (context)
        {
            glXDestroyContext(display, context);
        }
    
        if (window)
        {
            XDestroyWindow(display, window);
        }
        
	    XCloseDisplay(display);
	}

    ouzel::Engine::getInstance()->end();
    
    return 0;
}
