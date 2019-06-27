// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif
#include <stdexcept>
#include "Window.hpp"
#include "Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "graphics/Renderer.hpp"

#if TARGET_OS_IOS
#  include "ios/NativeWindowIOS.hpp"
#elif TARGET_OS_TV
#  include "tvos/NativeWindowTVOS.hpp"
#elif TARGET_OS_MAC
#  include "macos/NativeWindowMacOS.hpp"
#elif defined(__ANDROID__)
#  include "android/NativeWindowAndroid.hpp"
#elif defined(__linux__)
#  include "linux/NativeWindowLinux.hpp"
#elif defined(_WIN32)
#  include "windows/NativeWindowWin.hpp"
#elif defined(__EMSCRIPTEN__)
#  include "emscripten/NativeWindowEm.hpp"
#endif

namespace ouzel
{
    Window::Window(Engine& initEngine,
                   const Size2U& newSize,
                   uint32_t flags,
                   const std::string& newTitle,
                   graphics::Driver graphicsDriver):
        engine(initEngine),
#if TARGET_OS_IOS
        nativeWindow(new NativeWindowIOS(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                         newTitle,
                                         graphicsDriver,
                                         flags & HIGH_DPI)),
#elif TARGET_OS_TV
        nativeWindow(new NativeWindowTVOS(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                          newTitle,
                                          graphicsDriver,
                                          flags & HIGH_DPI)),
#elif TARGET_OS_MAC
        nativeWindow(new NativeWindowMacOS(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                           newSize,
                                           flags & RESIZABLE,
                                           flags & FULLSCREEN,
                                           flags & EXCLUSIVE_FULLSCREEN,
                                           newTitle,
                                           graphicsDriver,
                                           flags & HIGH_DPI)),
#elif defined(__ANDROID__)
        nativeWindow(new NativeWindowAndroid(std::bind(&Window::eventCallback, this, std::placeholders::_1), newTitle)),
#elif defined(__linux__)
        nativeWindow(new NativeWindowLinux(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                           newSize,
                                           flags & RESIZABLE,
                                           flags & FULLSCREEN,
                                           flags & EXCLUSIVE_FULLSCREEN,
                                           newTitle,
                                           graphicsDriver,
                                           flags & DEPTH)),
#elif defined(_WIN32)
        nativeWindow(new NativeWindowWin(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                         newSize,
                                         flags & RESIZABLE,
                                         flags & FULLSCREEN,
                                         flags & EXCLUSIVE_FULLSCREEN,
                                         newTitle,
                                         flags & HIGH_DPI)),
#elif defined(__EMSCRIPTEN__)
        nativeWindow(new NativeWindowEm(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                        newSize,
                                        flags & FULLSCREEN,
                                        newTitle,
                                        flags & HIGH_DPI)),
#else
        nativeWindow(new NativeWindow(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                      newSize,
                                      flags & RESIZABLE,
                                      flags & FULLSCREEN,
                                      flags & EXCLUSIVE_FULLSCREEN,
                                      newTitle,
                                      flags & HIGH_DPI)),
#endif
        size(nativeWindow->getSize()),
        resolution(nativeWindow->getResolution()),
        resizable(flags & RESIZABLE),
        fullscreen(flags & FULLSCREEN),
        exclusiveFullscreen(flags & EXCLUSIVE_FULLSCREEN),
        highDpi(flags & HIGH_DPI),
        title(newTitle)
    {
        (void)newSize;
        (void)graphicsDriver;
    }

    void Window::update()
    {
        NativeWindow::Event event;

        for (;;)
        {
            std::unique_lock<std::mutex> lock(eventQueueMutex);
            if (eventQueue.empty()) break;
            event = std::move(eventQueue.front());
            eventQueue.pop();
            lock.unlock();

            handleEvent(event);
        }
    }

    void Window::eventCallback(const NativeWindow::Event& event)
    {
        if (event.type == NativeWindow::Event::Type::FOCUS_CHANGE)
        {
            if (event.focus)
                engine.resume();
            else
                engine.pause();
        }
        else
        {
            std::lock_guard<std::mutex> lock(eventQueueMutex);
            eventQueue.push(event);
        }
    }

    void Window::handleEvent(const NativeWindow::Event& event)
    {
        switch (event.type)
        {
            case NativeWindow::Event::Type::SIZE_CHANGE:
            {
                size = event.size;

                std::unique_ptr<WindowEvent> sizeChangeEvent(new WindowEvent());
                sizeChangeEvent->type = Event::Type::WINDOW_SIZE_CHANGE;
                sizeChangeEvent->window = this;
                sizeChangeEvent->size = event.size;
                engine.getEventDispatcher().dispatchEvent(std::move(sizeChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::RESOLUTION_CHANGE:
            {
                resolution = event.size;

                engine.getRenderer()->setSize(resolution);

                std::unique_ptr<WindowEvent> resolutionChangeEvent(new WindowEvent());
                resolutionChangeEvent->type = Event::Type::RESOLUTION_CHANGE;
                resolutionChangeEvent->window = this;
                resolutionChangeEvent->size = event.size;
                engine.getEventDispatcher().dispatchEvent(std::move(resolutionChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::FULLSCREEN_CHANGE:
            {
                fullscreen = event.fullscreen;

                std::unique_ptr<WindowEvent> fullscreenChangeEvent(new WindowEvent());
                fullscreenChangeEvent->type = Event::Type::FULLSCREEN_CHANGE;
                fullscreenChangeEvent->window = this;
                fullscreenChangeEvent->fullscreen = event.fullscreen;
                engine.getEventDispatcher().dispatchEvent(std::move(fullscreenChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::SCREEN_CHANGE:
            {
                displayId = event.displayId;

                std::unique_ptr<WindowEvent> screenChangeEvent(new WindowEvent());
                screenChangeEvent->type = Event::Type::SCREEN_CHANGE;
                screenChangeEvent->window = this;
                screenChangeEvent->screenId = event.displayId;
                engine.getEventDispatcher().dispatchEvent(std::move(screenChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::CLOSE:
                engine.exit();
                break;

            case NativeWindow::Event::Type::SHOW:
                visible = true;
                break;
            case NativeWindow::Event::Type::HIDE:
                visible = false;
                break;
            case NativeWindow::Event::Type::MINIMIZE:
                minimized = true;
                break;
            case NativeWindow::Event::Type::MAXIMIZE:
                break;
            case NativeWindow::Event::Type::RESTORE:
                minimized = false;
                break;
            default:
                throw std::runtime_error("Unhandled event");
        }
    }

    void Window::close()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::CLOSE);
        nativeWindow->addCommand(command);
    }

    void Window::setSize(const Size2U& newSize)
    {
        if (size != newSize)
        {
            size = newSize;

            NativeWindow::Command command(NativeWindow::Command::Type::CHANGE_SIZE);
            command.size = newSize;
            nativeWindow->addCommand(command);

            std::unique_ptr<WindowEvent> event(new WindowEvent());
            event->type = Event::Type::WINDOW_SIZE_CHANGE;
            event->window = this;
            event->size = size;
            event->title = title;
            event->fullscreen = fullscreen;
            engine.getEventDispatcher().dispatchEvent(std::move(event));
        }
    }

    void Window::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen)
        {
            fullscreen = newFullscreen;

            NativeWindow::Command command(NativeWindow::Command::Type::CHANGE_FULLSCREEN);
            command.fullscreen = newFullscreen;
            nativeWindow->addCommand(command);

            std::unique_ptr<WindowEvent> event(new WindowEvent());
            event->type = Event::Type::FULLSCREEN_CHANGE;
            event->window = this;
            event->size = size;
            event->title = title;
            event->fullscreen = fullscreen;
            engine.getEventDispatcher().dispatchEvent(std::move(event));
        }
    }

    void Window::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            title = newTitle;

            NativeWindow::Command command(NativeWindow::Command::Type::SET_TITLE);
            command.title = newTitle;
            nativeWindow->addCommand(command);

            std::unique_ptr<WindowEvent> event(new WindowEvent());
            event->type = Event::Type::WINDOW_TITLE_CHANGE;
            event->window = this;
            event->size = size;
            event->title = title;
            event->fullscreen = fullscreen;
            engine.getEventDispatcher().dispatchEvent(std::move(event));
        }
    }

    void Window::bringToFront()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::BRING_TO_FRONT);
        nativeWindow->addCommand(command);
    }

    void Window::show()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::SHOW);
        nativeWindow->addCommand(command);
    }

    void Window::hide()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::HIDE);
        nativeWindow->addCommand(command);
    }

    void Window::minimize()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::MINIMIZE);
        nativeWindow->addCommand(command);
    }

    void Window::maximize()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::MAXIMIZE);
        nativeWindow->addCommand(command);
    }

    void Window::restore()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::RESTORE);
        nativeWindow->addCommand(command);
    }
}
