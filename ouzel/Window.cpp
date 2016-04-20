// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Window.h"
#include "Engine.h"
#include "Renderer.h"
#include "SceneManager.h"

namespace ouzel
{
    Window::Window(const Size2& size, bool resizable, bool fullscreen, const std::string& title, graphics::Renderer::Driver driver)
    {
        _size = size;
        _resizable = resizable;
        _fullscreen = fullscreen;
        _title = title;
        _driver = driver;
    }

    Window::~Window()
    {

    }

    bool Window::init()
    {
        if (!sharedEngine->getRenderer()->init(_size, _fullscreen))
        {
            return false;
        }

        return true;
    }

    void Window::close()
    {

    }

    void Window::setSize(const Size2& size)
    {
        if (_size != size)
        {
            _size = size;
            sharedEngine->getRenderer()->setSize(_size);
            sharedEngine->getSceneManager()->recalculateProjection();

            WindowEventPtr event = std::make_shared<WindowEvent>();
            event->type = Event::Type::WINDOW_SIZE_CHANGE;
            event->size = _size;
            event->title = _title;
            event->fullscreen = _fullscreen;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, sharedEngine->getRenderer());
        }
    }

    void Window::setFullscreen(bool fullscreen)
    {
        if (fullscreen != _fullscreen)
        {
            _fullscreen = fullscreen;

            sharedEngine->getRenderer()->setFullscreen(_fullscreen);

            WindowEventPtr event = std::make_shared<WindowEvent>();
            event->type = Event::Type::WINDOW_FULLSCREEN_CHANGE;
            event->size = _size;
            event->title = _title;
            event->fullscreen = _fullscreen;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, sharedEngine->getRenderer());
        }
    }

    void Window::setTitle(const std::string& title)
    {
        if (_title != title)
        {
            _title = title;

            WindowEventPtr event = std::make_shared<WindowEvent>();
            event->type = Event::Type::WINDOW_TITLE_CHANGE;
            event->size = _size;
            event->title = _title;
            event->fullscreen = _fullscreen;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, sharedEngine->getRenderer());
        }
    }
}
