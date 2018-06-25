// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Window.hpp"
#include "Setup.h"
#include "WindowResource.hpp"
#include "Engine.hpp"
#include "events/EventDispatcher.hpp"

#if OUZEL_PLATFORM_MACOS
#include "macos/WindowResourceMacOS.hpp"
#elif OUZEL_PLATFORM_IOS
#include "ios/WindowResourceIOS.hpp"
#elif OUZEL_PLATFORM_TVOS
#include "tvos/WindowResourceTVOS.hpp"
#elif OUZEL_PLATFORM_ANDROID
#include "android/WindowResourceAndroid.hpp"
#elif OUZEL_PLATFORM_LINUX
#include "linux/WindowResourceLinux.hpp"
#elif OUZEL_PLATFORM_WINDOWS
#include "windows/WindowResourceWin.hpp"
#elif OUZEL_PLATFORM_RASPBIAN
#include "raspbian/WindowResourceRasp.hpp"
#elif OUZEL_PLATFORM_EMSCRIPTEN
#include "emscripten/WindowResourceEm.hpp"
#endif

namespace ouzel
{
    Window::Window()
    {
#if OUZEL_PLATFORM_MACOS
        resource = new WindowResourceMacOS();
#elif OUZEL_PLATFORM_IOS
        resource = new WindowResourceIOS();
#elif OUZEL_PLATFORM_TVOS
        resource = new WindowResourceTVOS();
#elif OUZEL_PLATFORM_ANDROID
        resource = new WindowResourceAndroid();
#elif OUZEL_PLATFORM_LINUX
        resource = new WindowResourceLinux();
#elif OUZEL_PLATFORM_WINDOWS
        resource = new WindowResourceWin();
#elif OUZEL_PLATFORM_RASPBIAN
        resource = new WindowResourceRasp();
#elif OUZEL_PLATFORM_EMSCRIPTEN
        resource = new WindowResourceEm();
#else
        resource = new WindowResource();
#endif

        resource->setListener(this);
    }

    Window::~Window()
    {
        if (resource)
        {
            resource->setListener(nullptr);
            delete resource;
        }
    }

    void Window::init(const Size2& newSize,
                      bool newResizable,
                      bool newFullscreen,
                      bool newExclusiveFullscreen,
                      const std::string& newTitle,
                      bool newHighDpi,
                      bool depth)
    {
        resource->init(newSize,
                       newResizable,
                       newFullscreen,
                       newExclusiveFullscreen,
                       newTitle,
                       newHighDpi,
                       depth);

        size = resource->getSize();
        resolution = resource->getResolution();
        resizable = newResizable;
        fullscreen = newFullscreen;
        exclusiveFullscreen = newExclusiveFullscreen;
        highDpi = newHighDpi;
        title = newTitle;
    }

    void Window::close()
    {
        engine->executeOnMainThread(std::bind(&WindowResource::close, resource));
    }

    void Window::setSize(const Size2& newSize)
    {
        if (size != newSize)
        {
            size = newSize;

            engine->executeOnMainThread(std::bind(&WindowResource::setSize, resource, newSize));

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

            engine->executeOnMainThread(std::bind(&WindowResource::setFullscreen, resource, newFullscreen));

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

            engine->executeOnMainThread(std::bind(&WindowResource::setTitle, resource, newTitle));

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
