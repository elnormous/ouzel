// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Window.hpp"
#include "Setup.h"
#include "Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "utils/Utils.hpp"

#if OUZEL_PLATFORM_MACOS
#include "macos/NativeWindowMacOS.hpp"
#elif OUZEL_PLATFORM_IOS
#include "ios/NativeWindowIOS.hpp"
#elif OUZEL_PLATFORM_TVOS
#include "tvos/NativeWindowTVOS.hpp"
#elif OUZEL_PLATFORM_ANDROID
#include "android/NativeWindowAndroid.hpp"
#elif OUZEL_PLATFORM_LINUX
#include "linux/NativeWindowLinux.hpp"
#elif OUZEL_PLATFORM_WINDOWS
#include "windows/NativeWindowWin.hpp"
#elif OUZEL_PLATFORM_EMSCRIPTEN
#include "emscripten/NativeWindowEm.hpp"
#endif

namespace ouzel
{
    Window::Window(const Size2& newSize,
                   bool newResizable,
                   bool newFullscreen,
                   bool newExclusiveFullscreen,
                   const std::string& newTitle,
                   graphics::Renderer::Driver graphicsDriver,
                   bool newHighDpi,
                   bool depth):
#if OUZEL_PLATFORM_MACOS
        nativeWindow(new NativeWindowMacOS(newSize,
                                           newResizable,
                                           newFullscreen,
                                           newExclusiveFullscreen,
                                           newTitle,
                                           graphicsDriver,
                                           newHighDpi))
#elif OUZEL_PLATFORM_IOS
        nativeWindow(new NativeWindowIOS(newTitle,
                                         graphicsDriver,
                                         newHighDpi))
#elif OUZEL_PLATFORM_TVOS
        nativeWindow(new NativeWindowTVOS(newTitle,
                                          graphicsDriver,
                                          newHighDpi))
#elif OUZEL_PLATFORM_ANDROID
        nativeWindow(new NativeWindowAndroid(newTitle))
#elif OUZEL_PLATFORM_LINUX
        nativeWindow(new NativeWindowLinux(newSize,
                                           newResizable,
                                           newFullscreen,
                                           newExclusiveFullscreen,
                                           newTitle,
                                           graphicsDriver,
                                           depth))
#elif OUZEL_PLATFORM_WINDOWS
        nativeWindow(new NativeWindowWin(newSize,
                                         newResizable,
                                         newFullscreen,
                                         newExclusiveFullscreen,
                                         newTitle,
                                         newHighDpi))
#elif OUZEL_PLATFORM_EMSCRIPTEN
        nativeWindow(new NativeWindowEm(newSize,
                                        newFullscreen,
                                        newTitle,
                                        newHighDpi))
#else
        resource(new NativeWindow(newSize,
                                  newResizable,
                                  newFullscreen,
                                  newExclusiveFullscreen,
                                  newTitle,
                                  newHighDpi))
#endif
    {
        OUZEL_UNUSED(newSize);
        OUZEL_UNUSED(newResizable);
        OUZEL_UNUSED(newFullscreen);
        OUZEL_UNUSED(newExclusiveFullscreen);
        OUZEL_UNUSED(graphicsDriver);
        OUZEL_UNUSED(newHighDpi);
        OUZEL_UNUSED(depth);

        size = nativeWindow->getSize();
        resolution = nativeWindow->getResolution();
        resizable = newResizable;
        fullscreen = newFullscreen;
        exclusiveFullscreen = newExclusiveFullscreen;
        highDpi = newHighDpi;
        title = newTitle;
    }

    void Window::update()
    {
        for (const NativeWindow::Event& event : nativeWindow->getEvents())
        {
            switch (event.type)
            {
                case NativeWindow::Event::Type::SIZE_CHANGE:
                {
                    size = event.size;

                    Event sizeChangeEvent;
                    sizeChangeEvent.type = Event::Type::WINDOW_SIZE_CHANGE;
                    sizeChangeEvent.windowEvent.window = this;
                    sizeChangeEvent.windowEvent.size = event.size;
                    engine->getEventDispatcher()->postEvent(sizeChangeEvent);
                    break;
                }
                case NativeWindow::Event::Type::RESOLUTION_CHANGE:
                {
                    resolution = event.size;

                    Event resolutionChangeEvent;
                    resolutionChangeEvent.type = Event::Type::RESOLUTION_CHANGE;
                    resolutionChangeEvent.windowEvent.window = this;
                    resolutionChangeEvent.windowEvent.size = event.size;
                    engine->getEventDispatcher()->postEvent(resolutionChangeEvent);

                    engine->getRenderer()->setSize(event.size);
                    break;
                }
                case NativeWindow::Event::Type::FULLSCREEN_CHANGE:
                {
                    fullscreen = event.fullscreen;

                    Event fullscreenChangeEvent;
                    fullscreenChangeEvent.type = Event::Type::FULLSCREEN_CHANGE;

                    fullscreenChangeEvent.windowEvent.window = this;
                    fullscreenChangeEvent.windowEvent.fullscreen = event.fullscreen;

                    engine->getEventDispatcher()->postEvent(fullscreenChangeEvent);
                    break;
                }
                case NativeWindow::Event::Type::SCREEN_CHANGE:
                {
                    displayId = event.displayId;

                    Event screenChangeEvent;
                    screenChangeEvent.type = Event::Type::SCREEN_CHANGE;

                    screenChangeEvent.windowEvent.window = this;
                    screenChangeEvent.windowEvent.screenId = event.displayId;

                    engine->getEventDispatcher()->postEvent(screenChangeEvent);
                    break;
                }
                case NativeWindow::Event::Type::CLOSE:
                    engine->exit();
                    break;
            }
        }
    }

    void Window::close()
    {
        engine->executeOnMainThread(std::bind(&NativeWindow::close, nativeWindow.get()));
    }

    void Window::setSize(const Size2& newSize)
    {
        if (size != newSize)
        {
            size = newSize;

            engine->executeOnMainThread(std::bind(&NativeWindow::setSize, nativeWindow.get(), newSize));

            Event event;
            event.type = Event::Type::WINDOW_SIZE_CHANGE;

            event.windowEvent.window = this;
            event.windowEvent.size = size;
            event.windowEvent.title = title;
            event.windowEvent.fullscreen = fullscreen;

            engine->getEventDispatcher()->postEvent(event);
        }
    }

    void Window::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen)
        {
            fullscreen = newFullscreen;

            engine->executeOnMainThread(std::bind(&NativeWindow::setFullscreen, nativeWindow.get(), newFullscreen));

            Event event;
            event.type = Event::Type::FULLSCREEN_CHANGE;

            event.windowEvent.window = this;
            event.windowEvent.size = size;
            event.windowEvent.title = title;
            event.windowEvent.fullscreen = fullscreen;

            engine->getEventDispatcher()->postEvent(event);
        }
    }

    void Window::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            title = newTitle;

            engine->executeOnMainThread(std::bind(&NativeWindow::setTitle, nativeWindow.get(), newTitle));

            Event event;
            event.type = Event::Type::WINDOW_TITLE_CHANGE;

            event.windowEvent.window = this;
            event.windowEvent.size = size;
            event.windowEvent.title = title;
            event.windowEvent.fullscreen = fullscreen;

            engine->getEventDispatcher()->postEvent(event);
        }
    }
}
