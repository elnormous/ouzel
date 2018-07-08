// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Window.hpp"
#include "Setup.h"
#include "NativeWindow.hpp"
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
                   bool depth)
    {
#if OUZEL_PLATFORM_MACOS
        OUZEL_UNUSED(depth);
        nativeWindow = new NativeWindowMacOS(newSize,
                                             newResizable,
                                             newFullscreen,
                                             newExclusiveFullscreen,
                                             newTitle,
                                             graphicsDriver,
                                             newHighDpi);
#elif OUZEL_PLATFORM_IOS
        OUZEL_UNUSED(newSize);
        OUZEL_UNUSED(newResizable);
        OUZEL_UNUSED(newFullscreen);
        OUZEL_UNUSED(newExclusiveFullscreen);
        OUZEL_UNUSED(depth);
        nativeWindow = new NativeWindowIOS(newTitle,
                                           graphicsDriver,
                                           newHighDpi);
#elif OUZEL_PLATFORM_TVOS
        OUZEL_UNUSED(newSize);
        OUZEL_UNUSED(newResizable);
        OUZEL_UNUSED(newFullscreen);
        OUZEL_UNUSED(newExclusiveFullscreen);
        OUZEL_UNUSED(depth);
        nativeWindow = new NativeWindowTVOS(newTitle,
                                            graphicsDriver,
                                            newHighDpi);
#elif OUZEL_PLATFORM_ANDROID
        OUZEL_UNUSED(newSize);
        OUZEL_UNUSED(newResizable);
        OUZEL_UNUSED(newFullscreen);
        OUZEL_UNUSED(newExclusiveFullscreen);
        OUZEL_UNUSED(graphicsDriver);
        OUZEL_UNUSED(newHighDpi);
        OUZEL_UNUSED(depth);
        nativeWindow = new NativeWindowAndroid(newTitle);
#elif OUZEL_PLATFORM_LINUX
        OUZEL_UNUSED(newHighDpi);
        nativeWindow = new NativeWindowLinux(newSize,
                                             newResizable,
                                             newFullscreen,
                                             newExclusiveFullscreen,
                                             newTitle,
                                             graphicsDriver,
                                             depth);
#elif OUZEL_PLATFORM_WINDOWS
        OUZEL_UNUSED(graphicsDriver);
        OUZEL_UNUSED(depth);
        nativeWindow = new NativeWindowWin(newSize,
                                           newResizable,
                                           newFullscreen,
                                           newExclusiveFullscreen,
                                           newTitle,
                                           newHighDpi);
#elif OUZEL_PLATFORM_EMSCRIPTEN
        OUZEL_UNUSED(newResizable);
        OUZEL_UNUSED(newExclusiveFullscreen);
        OUZEL_UNUSED(graphicsDriver);
        OUZEL_UNUSED(newHighDpi);
        OUZEL_UNUSED(depth);
        nativeWindow = new NativeWindowEm(newSize,
                                          newFullscreen,
                                          newTitle);
#else
        OUZEL_UNUSED(graphicsDriver);
        OUZEL_UNUSED(depth);
        resource = new NativeWindow(newSize,
                                    newResizable,
                                    newFullscreen,
                                    newExclusiveFullscreen,
                                    newTitle,
                                    newHighDpi);
#endif

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
