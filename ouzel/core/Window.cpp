// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Window.h"
#include "Engine.h"
#include "events/EventDispatcher.h"

namespace ouzel
{
    Window::Window(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle):
        size(aSize),
        resizable(aResizable),
        fullscreen(aFullscreen),
        title(aTitle),
        eventHandler(EventHandler::PRIORITY_MAX + 1)
    {
        eventHandler.windowHandler = std::bind(&Window::handleWindowChange, this, std::placeholders::_1, std::placeholders::_2);
        sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);
    }

    Window::~Window()
    {
    }

    bool Window::init()
    {
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

            sharedEngine->getRenderer()->setSize(size * getContentScale());
        }
    }

    void Window::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen)
        {
            fullscreen = newFullscreen;

            Event event;
            event.type = Event::Type::WINDOW_FULLSCREEN_CHANGE;

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
                sharedEngine->getRenderer()->setSize(size * contentScale);
                break;
            case Event::Type::WINDOW_FULLSCREEN_CHANGE:
                fullscreen = event.fullscreen;
                break;
            case Event::Type::WINDOW_CONTENT_SCALE_CHANGE:
                contentScale = event.contentScale;
                sharedEngine->getRenderer()->setSize(size * contentScale);
                break;
            default:
                break;
        }

        return true;
    }
}
