// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "Window.hpp"
#include "Setup.h"
#include "Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "graphics/Renderer.hpp"

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
    Window::Window(Engine& initEngine,
                   const Size2& newSize,
                   bool newResizable,
                   bool newFullscreen,
                   bool newExclusiveFullscreen,
                   const std::string& newTitle,
                   graphics::Driver graphicsDriver,
                   bool newHighDpi,
                   bool depth):
        engine(initEngine),
#if OUZEL_PLATFORM_MACOS
        nativeWindow(new NativeWindowMacOS(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                           newSize,
                                           newResizable,
                                           newFullscreen,
                                           newExclusiveFullscreen,
                                           newTitle,
                                           graphicsDriver,
                                           newHighDpi)),
#elif OUZEL_PLATFORM_IOS
        nativeWindow(new NativeWindowIOS(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                         newTitle,
                                         graphicsDriver,
                                         newHighDpi)),
#elif OUZEL_PLATFORM_TVOS
        nativeWindow(new NativeWindowTVOS(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                          newTitle,
                                          graphicsDriver,
                                          newHighDpi)),
#elif OUZEL_PLATFORM_ANDROID
        nativeWindow(new NativeWindowAndroid(std::bind(&Window::eventCallback, this, std::placeholders::_1), newTitle)),
#elif OUZEL_PLATFORM_LINUX
        nativeWindow(new NativeWindowLinux(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                           newSize,
                                           newResizable,
                                           newFullscreen,
                                           newExclusiveFullscreen,
                                           newTitle,
                                           graphicsDriver,
                                           depth)),
#elif OUZEL_PLATFORM_WINDOWS
        nativeWindow(new NativeWindowWin(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                         newSize,
                                         newResizable,
                                         newFullscreen,
                                         newExclusiveFullscreen,
                                         newTitle,
                                         newHighDpi)),
#elif OUZEL_PLATFORM_EMSCRIPTEN
        nativeWindow(new NativeWindowEm(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                        newSize,
                                        newFullscreen,
                                        newTitle,
                                        newHighDpi)),
#else
        nativeWindow(new NativeWindow(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                      newSize,
                                      newResizable,
                                      newFullscreen,
                                      newExclusiveFullscreen,
                                      newTitle,
                                      newHighDpi)),
#endif
        size(nativeWindow->getSize()),
        resolution(nativeWindow->getResolution()),
        resizable(newResizable),
        fullscreen(newFullscreen),
        exclusiveFullscreen(newExclusiveFullscreen),
        highDpi(newHighDpi),
        title(newTitle)
    {
        (void)newSize;
        (void)newResizable;
        (void)newFullscreen;
        (void)newExclusiveFullscreen;
        (void)graphicsDriver;
        (void)newHighDpi;
        (void)depth;
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
            std::unique_lock<std::mutex> lock(eventQueueMutex);
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
                resolution = event.resolution;

                engine.getRenderer()->setSize(resolution);

                std::unique_ptr<WindowEvent> resolutionChangeEvent(new WindowEvent());
                resolutionChangeEvent->type = Event::Type::RESOLUTION_CHANGE;
                resolutionChangeEvent->window = this;
                resolutionChangeEvent->size = event.size;
                engine.getEventDispatcher().dispatchEvent(std::move(resolutionChangeEvent));
                engine.getRenderer()->setSize(event.size);
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
            default:
                throw std::runtime_error("Unhandled event");
                break;
        }
    }

    void Window::close()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::CLOSE);
        nativeWindow->addCommand(command);
    }

    void Window::setSize(const Size2& newSize)
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
}
