// Ouzel by Elviss Strazdins

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif
#include <stdexcept>
#include "Window.hpp"
#include "Engine.hpp"
#include "../events/EventDispatcher.hpp"
#include "../graphics/Graphics.hpp"

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

namespace ouzel::core
{
    Window::Window(Engine& initEngine,
                   const Size<std::uint32_t, 2>& newSize,
                   Flags flags,
                   const std::string& newTitle,
                   graphics::Driver graphicsDriver):
        engine{initEngine},
#if TARGET_OS_IOS
        nativeWindow{std::make_unique<ios::NativeWindow>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                         newTitle,
                                                         graphicsDriver,
                                                         (flags & Flags::highDpi) == Flags::highDpi)},
#elif TARGET_OS_TV
        nativeWindow{std::make_unique<tvos::NativeWindow>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                          newTitle,
                                                          graphicsDriver,
                                                          (flags & Flags::highDpi) == Flags::highDpi)},
#elif TARGET_OS_MAC
        nativeWindow{std::make_unique<macos::NativeWindow>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                           newSize,
                                                           (flags & Flags::resizable) == Flags::resizable,
                                                           (flags & Flags::fullscreen) == Flags::fullscreen,
                                                           (flags & Flags::exclusiveFullscreen) == Flags::exclusiveFullscreen,
                                                           newTitle,
                                                           graphicsDriver,
                                                           (flags & Flags::highDpi) == Flags::highDpi)},
#elif defined(__ANDROID__)
        nativeWindow{std::make_unique<android::NativeWindow>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                             newTitle)},
#elif defined(__linux__)
        nativeWindow{std::make_unique<linux::NativeWindow>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                           newSize,
                                                           (flags & Flags::resizable) == Flags::resizable,
                                                           (flags & Flags::fullscreen) == Flags::fullscreen,
                                                           (flags & Flags::exclusiveFullscreen) == Flags::exclusiveFullscreen,
                                                           newTitle)},
#elif defined(_WIN32)
        nativeWindow{std::make_unique<windows::NativeWindow>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                             newSize,
                                                             (flags & Flags::resizable) == Flags::resizable,
                                                             (flags & Flags::fullscreen) == Flags::fullscreen,
                                                             (flags & Flags::exclusiveFullscreen) == Flags::exclusiveFullscreen,
                                                             newTitle,
                                                             (flags & Flags::highDpi) == Flags::highDpi)},
#elif defined(__EMSCRIPTEN__)
        nativeWindow{std::make_unique<emscripten::NativeWindow>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                                newSize,
                                                                (flags & Flags::fullscreen) == Flags::fullscreen,
                                                                newTitle,
                                                                (flags & Flags::highDpi) == Flags::highDpi)},
#else
        nativeWindow{std::make_unique<NativeWindow>(std::bind(&Window::eventCallback, this, std::placeholders::_1),
                                                    newSize,
                                                    (flags & Flags::resizable) == Flags::resizable,
                                                    (flags & Flags::fullscreen) == Flags::fullscreen,
                                                    (flags & Flags::exclusiveFullscreen) == Flags::exclusiveFullscreen,
                                                    newTitle,
                                                    (flags & Flags::highDpi) == Flags::highDpi)},
#endif
        size{nativeWindow->getSize()},
        resolution{nativeWindow->getResolution()},
        resizable{(flags & Flags::resizable) == Flags::resizable},
        fullscreen{(flags & Flags::fullscreen) == Flags::fullscreen},
        exclusiveFullscreen{(flags & Flags::exclusiveFullscreen) == Flags::exclusiveFullscreen},
        highDpi{(flags & Flags::highDpi) == Flags::highDpi},
        title{newTitle}
    {
        static_cast<void>(newSize);
        static_cast<void>(graphicsDriver);
    }

    void Window::update()
    {
        NativeWindow::Event event;

        for (;;)
        {
            std::unique_lock lock(eventQueueMutex);
            if (eventQueue.empty()) break;
            event = std::move(eventQueue.front());
            eventQueue.pop();
            lock.unlock();

            handleEvent(event);
        }
    }

    void Window::eventCallback(const NativeWindow::Event& event)
    {
        if (event.type == NativeWindow::Event::Type::focusChange)
        {
            if (event.focus)
                engine.resume();
            else
                engine.pause();
        }
        else
        {
            std::lock_guard lock(eventQueueMutex);
            eventQueue.push(event);
        }
    }

    void Window::handleEvent(const NativeWindow::Event& event)
    {
        switch (event.type)
        {
            case NativeWindow::Event::Type::sizeChange:
            {
                size = event.size;

                auto sizeChangeEvent = std::make_unique<WindowEvent>();
                sizeChangeEvent->type = Event::Type::windowSizeChange;
                sizeChangeEvent->window = this;
                sizeChangeEvent->size = event.size;
                engine.getEventDispatcher().dispatchEvent(std::move(sizeChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::resolutionChange:
            {
                resolution = event.size;

                engine.getGraphics()->setSize(resolution);

                auto resolutionChangeEvent = std::make_unique<WindowEvent>();
                resolutionChangeEvent->type = Event::Type::resolutionChange;
                resolutionChangeEvent->window = this;
                resolutionChangeEvent->size = event.size;
                engine.getEventDispatcher().dispatchEvent(std::move(resolutionChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::fullscreenChange:
            {
                fullscreen = event.fullscreen;

                auto fullscreenChangeEvent = std::make_unique<WindowEvent>();
                fullscreenChangeEvent->type = Event::Type::fullscreenChange;
                fullscreenChangeEvent->window = this;
                fullscreenChangeEvent->fullscreen = event.fullscreen;
                engine.getEventDispatcher().dispatchEvent(std::move(fullscreenChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::screenChange:
            {
                displayId = event.displayId;

                auto screenChangeEvent = std::make_unique<WindowEvent>();
                screenChangeEvent->type = Event::Type::screenChange;
                screenChangeEvent->window = this;
                screenChangeEvent->screenId = event.displayId;
                engine.getEventDispatcher().dispatchEvent(std::move(screenChangeEvent));
                break;
            }
            case NativeWindow::Event::Type::close:
                engine.exit();
                break;

            case NativeWindow::Event::Type::show:
                visible = true;
                break;
            case NativeWindow::Event::Type::hide:
                visible = false;
                break;
            case NativeWindow::Event::Type::minimize:
                minimized = true;
                break;
            case NativeWindow::Event::Type::maximize:
                break;
            case NativeWindow::Event::Type::restore:
                minimized = false;
                break;
            default:
                throw std::runtime_error("Unhandled event");
        }
    }

    void Window::close()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::close);
        nativeWindow->addCommand(command);
    }

    void Window::setSize(const Size<std::uint32_t, 2>& newSize)
    {
        if (size != newSize)
        {
            size = newSize;

            NativeWindow::Command command(NativeWindow::Command::Type::changeSize);
            command.size = newSize;
            nativeWindow->addCommand(command);

            auto event = std::make_unique<WindowEvent>();
            event->type = Event::Type::windowSizeChange;
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

            NativeWindow::Command command(NativeWindow::Command::Type::changeFullscreen);
            command.fullscreen = newFullscreen;
            nativeWindow->addCommand(command);

            auto event = std::make_unique<WindowEvent>();
            event->type = Event::Type::fullscreenChange;
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

            NativeWindow::Command command(NativeWindow::Command::Type::setTitle);
            command.title = newTitle;
            nativeWindow->addCommand(command);

            auto event = std::make_unique<WindowEvent>();
            event->type = Event::Type::windowTitleChange;
            event->window = this;
            event->size = size;
            event->title = title;
            event->fullscreen = fullscreen;
            engine.getEventDispatcher().dispatchEvent(std::move(event));
        }
    }

    void Window::bringToFront()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::bringToFront);
        nativeWindow->addCommand(command);
    }

    void Window::show()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::show);
        nativeWindow->addCommand(command);
    }

    void Window::hide()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::hide);
        nativeWindow->addCommand(command);
    }

    void Window::minimize()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::minimize);
        nativeWindow->addCommand(command);
    }

    void Window::maximize()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::maximize);
        nativeWindow->addCommand(command);
    }

    void Window::restore()
    {
        NativeWindow::Command command(NativeWindow::Command::Type::restore);
        nativeWindow->addCommand(command);
    }
}
