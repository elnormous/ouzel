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
        nativeWindow(std::make_unique<NativeWindowIOS>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                       newTitle,
                                                       graphicsDriver,
                                                       flags & HIGH_DPI)),
#elif TARGET_OS_TV
        nativeWindow(std::make_unique<NativeWindowTVOS>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                        newTitle,
                                                        graphicsDriver,
                                                        flags & HIGH_DPI)),
#elif TARGET_OS_MAC
        nativeWindow(std::make_unique<NativeWindowMacOS>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                         newSize,
                                                         flags & RESIZABLE,
                                                         flags & FULLSCREEN,
                                                         flags & EXCLUSIVE_FULLSCREEN,
                                                         newTitle,
                                                         graphicsDriver,
                                                         flags & HIGH_DPI)),
#elif defined(__ANDROID__)
        nativeWindow(std::make_unique<NativeWindowAndroid>(std::bind(&Window::eventCallback, this, std::placeholders::_1), newTitle)),
#elif defined(__linux__)
        nativeWindow(std::make_unique<NativeWindowLinux>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                         newSize,
                                                         flags & RESIZABLE,
                                                         flags & FULLSCREEN,
                                                         flags & EXCLUSIVE_FULLSCREEN,
                                                         newTitle,
                                                         graphicsDriver,
                                                         flags & DEPTH)),
#elif defined(_WIN32)
        nativeWindow(std::make_unique<NativeWindowWin>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                       newSize,
                                                       flags & RESIZABLE,
                                                       flags & FULLSCREEN,
                                                       flags & EXCLUSIVE_FULLSCREEN,
                                                       newTitle,
                                                       flags & HIGH_DPI)),
#elif defined(__EMSCRIPTEN__)
        nativeWindow(std::make_unique<NativeWindowEm>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                      newSize,
                                                      flags & FULLSCREEN,
                                                      newTitle,
                                                      flags & HIGH_DPI)),
#else
        nativeWindow(std::make_unique<NativeWindow>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
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
        if (event.type == NativeWindow::Event::Type::FocusChange)
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
            case NativeWindow::Event::Type::SizeChange:
            {
                size = event.size;

                auto sizeChangeEvent = std::make_unique<WindowEvent>();
                sizeChangeEvent->type = Event::Type::WindowSizeChange;
                sizeChangeEvent->window = this;
                sizeChangeEvent->size = event.size;
                engine.getEventDispatcher().dispatchEvent(std::move(sizeChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::ResolutionChange:
            {
                resolution = event.size;

                engine.getRenderer()->setSize(resolution);

                auto resolutionChangeEvent = std::make_unique<WindowEvent>();
                resolutionChangeEvent->type = Event::Type::ResolutionChange;
                resolutionChangeEvent->window = this;
                resolutionChangeEvent->size = event.size;
                engine.getEventDispatcher().dispatchEvent(std::move(resolutionChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::FullscreenChange:
            {
                fullscreen = event.fullscreen;

                auto fullscreenChangeEvent = std::make_unique<WindowEvent>();
                fullscreenChangeEvent->type = Event::Type::FullscreenChange;
                fullscreenChangeEvent->window = this;
                fullscreenChangeEvent->fullscreen = event.fullscreen;
                engine.getEventDispatcher().dispatchEvent(std::move(fullscreenChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::ScreenChange:
            {
                displayId = event.displayId;

                auto screenChangeEvent = std::make_unique<WindowEvent>();
                screenChangeEvent->type = Event::Type::ScreenChange;
                screenChangeEvent->window = this;
                screenChangeEvent->screenId = event.displayId;
                engine.getEventDispatcher().dispatchEvent(std::move(screenChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::Close:
                engine.exit();
                break;

            case NativeWindow::Event::Type::Show:
                visible = true;
                break;
            case NativeWindow::Event::Type::Hide:
                visible = false;
                break;
            case NativeWindow::Event::Type::Minimize:
                minimized = true;
                break;
            case NativeWindow::Event::Type::Maximize:
                break;
            case NativeWindow::Event::Type::Restore:
                minimized = false;
                break;
            default:
                throw std::runtime_error("Unhandled event");
        }
    }

    void Window::close()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::Close);
        nativeWindow->addCommand(command);
    }

    void Window::setSize(const Size2U& newSize)
    {
        if (size != newSize)
        {
            size = newSize;

            NativeWindow::Command command(NativeWindow::Command::Type::ChangeSize);
            command.size = newSize;
            nativeWindow->addCommand(command);

            auto event = std::make_unique<WindowEvent>();
            event->type = Event::Type::WindowSizeChange;
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

            NativeWindow::Command command(NativeWindow::Command::Type::ChangeFullscreen);
            command.fullscreen = newFullscreen;
            nativeWindow->addCommand(command);

            auto event = std::make_unique<WindowEvent>();
            event->type = Event::Type::FullscreenChange;
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

            NativeWindow::Command command(NativeWindow::Command::Type::SetTitle);
            command.title = newTitle;
            nativeWindow->addCommand(command);

            auto event = std::make_unique<WindowEvent>();
            event->type = Event::Type::WindowTitleChange;
            event->window = this;
            event->size = size;
            event->title = title;
            event->fullscreen = fullscreen;
            engine.getEventDispatcher().dispatchEvent(std::move(event));
        }
    }

    void Window::bringToFront()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::BringToFront);
        nativeWindow->addCommand(command);
    }

    void Window::show()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::Show);
        nativeWindow->addCommand(command);
    }

    void Window::hide()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::Hide);
        nativeWindow->addCommand(command);
    }

    void Window::minimize()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::Minimize);
        nativeWindow->addCommand(command);
    }

    void Window::maximize()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::Maximize);
        nativeWindow->addCommand(command);
    }

    void Window::restore()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::Restore);
        nativeWindow->addCommand(command);
    }
}
