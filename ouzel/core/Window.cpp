// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Window.hpp"
#include "Setup.h"
#include "NativeWindow.hpp"
#include "Engine.hpp"
#include "events/EventDispatcher.hpp"

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
                   bool newHighDpi,
                   bool depth)
    {
#if OUZEL_PLATFORM_MACOS
        nativeWindow = new NativeWindowMacOS();
#elif OUZEL_PLATFORM_IOS
        nativeWindow = new NativeWindowIOS();
#elif OUZEL_PLATFORM_TVOS
        nativeWindow = new NativeWindowTVOS();
#elif OUZEL_PLATFORM_ANDROID
        nativeWindow = new NativeWindowAndroid();
#elif OUZEL_PLATFORM_LINUX
        nativeWindow = new NativeWindowLinux();
#elif OUZEL_PLATFORM_WINDOWS
        nativeWindow = new NativeWindowWin();
#elif OUZEL_PLATFORM_EMSCRIPTEN
        nativeWindow = new NativeWindowEm();
#else
        resource = new NativeWindow();
#endif

        nativeWindow->init(newSize,
                           newResizable,
                           newFullscreen,
                           newExclusiveFullscreen,
                           newTitle,
                           newHighDpi,
                           depth);

        nativeWindow->setListener(this);

        size = nativeWindow->getSize();
        resolution = nativeWindow->getResolution();
        resizable = newResizable;
        fullscreen = newFullscreen;
        exclusiveFullscreen = newExclusiveFullscreen;
        highDpi = newHighDpi;
        title = newTitle;
    }

    Window::~Window()
    {
        if (nativeWindow)
        {
            nativeWindow->setListener(nullptr);
            delete nativeWindow;
        }
    }

    void Window::close()
    {
        engine->executeOnMainThread(std::bind(&NativeWindow::close, nativeWindow));
    }

    void Window::setSize(const Size2& newSize)
    {
        if (size != newSize)
        {
            size = newSize;

            engine->executeOnMainThread(std::bind(&NativeWindow::setSize, nativeWindow, newSize));

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

            engine->executeOnMainThread(std::bind(&NativeWindow::setFullscreen, nativeWindow, newFullscreen));

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

            engine->executeOnMainThread(std::bind(&NativeWindow::setTitle, nativeWindow, newTitle));

            Event event;
            event.type = Event::Type::WINDOW_TITLE_CHANGE;

            event.windowEvent.window = this;
            event.windowEvent.size = size;
            event.windowEvent.title = title;
            event.windowEvent.fullscreen = fullscreen;

            engine->getEventDispatcher()->postEvent(event);
        }
    }

    void Window::onSizeChange(const Size2& newSize)
    {
        engine->getSceneManager()->executeOnUpdateThread([this, newSize]() {
            size = newSize;

            Event sizeChangeEvent;
            sizeChangeEvent.type = Event::Type::WINDOW_SIZE_CHANGE;
            sizeChangeEvent.windowEvent.window = this;
            sizeChangeEvent.windowEvent.size = newSize;
            engine->getEventDispatcher()->postEvent(sizeChangeEvent);
        });
    }

    void Window::onResolutionChange(const Size2& newResolution)
    {
        engine->getSceneManager()->executeOnUpdateThread([this, newResolution]() {
            resolution = newResolution;

            Event resolutionChangeEvent;
            resolutionChangeEvent.type = Event::Type::RESOLUTION_CHANGE;
            resolutionChangeEvent.windowEvent.window = this;
            resolutionChangeEvent.windowEvent.size = newResolution;
            engine->getEventDispatcher()->postEvent(resolutionChangeEvent);

            engine->getRenderer()->setSize(newResolution);
        });
    }

    void Window::onFullscreenChange(bool newFullscreen)
    {
        engine->getSceneManager()->executeOnUpdateThread([this, newFullscreen]() {
            fullscreen = newFullscreen;

            Event event;
            event.type = Event::Type::FULLSCREEN_CHANGE;

            event.windowEvent.window = this;
            event.windowEvent.fullscreen = newFullscreen;

            engine->getEventDispatcher()->postEvent(event);
        });
    }

    void Window::onScreenChange(uint32_t newDisplayId)
    {
        engine->getSceneManager()->executeOnUpdateThread([this, newDisplayId]() {
            displayId = newDisplayId;

            Event event;
            event.type = Event::Type::SCREEN_CHANGE;

            event.windowEvent.window = this;
            event.windowEvent.screenId = newDisplayId;

            engine->getEventDispatcher()->postEvent(event);
        });
    }

    void Window::onClose()
    {
        engine->exit();
    }
}
