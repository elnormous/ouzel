// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Window.hpp"
#include "Engine.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    Window::Window():
        eventHandler(EventHandler::PRIORITY_MAX + 1)
    {
        eventHandler.windowHandler = std::bind(&Window::handleWindowChange, this, std::placeholders::_1, std::placeholders::_2);
        sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);
    }

    Window::~Window()
    {
    }

    bool Window::init(const Size2& newSize,
                      bool newResizable,
                      bool newFullscreen,
                      bool newExclusiveFullscreen,
                      const std::string& newTitle,
                      bool newHighDpi,
                      bool)
    {
        size = newSize;
        resizable = newResizable;
        fullscreen = newFullscreen;
        exclusiveFullscreen = newExclusiveFullscreen;
        highDpi = newHighDpi;
        title = newTitle;

        return true;
    }

    void Window::close()
    {
    }

    void Window::setSize(const Size2& newSize)
    {
        if (size != newSize)
        {
            size = newSize;

            Event event;
            event.type = Event::Type::WINDOW_SIZE_CHANGE;

            event.windowEvent.window = this;
            event.windowEvent.size = size;
            event.windowEvent.title = title;
            event.windowEvent.fullscreen = fullscreen;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }
    }

    void Window::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen)
        {
            fullscreen = newFullscreen;

            Event event;
            event.type = Event::Type::FULLSCREEN_CHANGE;

            event.windowEvent.window = this;
            event.windowEvent.size = size;
            event.windowEvent.title = title;
            event.windowEvent.fullscreen = fullscreen;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }
    }

    void Window::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            title = newTitle;

            Event event;
            event.type = Event::Type::WINDOW_TITLE_CHANGE;

            event.windowEvent.window = this;
            event.windowEvent.size = size;
            event.windowEvent.title = title;
            event.windowEvent.fullscreen = fullscreen;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }
    }

    bool Window::handleWindowChange(Event::Type type, const WindowEvent& event)
    {
        switch (type)
        {
            case Event::Type::WINDOW_SIZE_CHANGE:
                size = event.size;
                break;
            case Event::Type::FULLSCREEN_CHANGE:
                fullscreen = event.fullscreen;
                break;
            case Event::Type::RESOLUTION_CHANGE:
                resolution = event.size;
                sharedEngine->getRenderer()->setSize(resolution);
                break;
            default:
                break;
        }

        return true;
    }
}
