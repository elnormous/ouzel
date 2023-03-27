// Ouzel by Elviss Strazdins

#include <stdexcept>
#include "Window.hpp"

namespace ouzel::core
{
    Window::Window([[maybe_unused]] const math::Size<std::uint32_t, 2>& newSize,
                   Flags flags,
                   const std::string& newTitle,
                   [[maybe_unused]] graphics::Driver graphicsDriver):
#if TARGET_OS_IOS
        nativeWindow{newTitle, graphicsDriver, (flags & Flags::highDpi) == Flags::highDpi},
#elif TARGET_OS_TV
        nativeWindow{newTitle, graphicsDriver, (flags & Flags::highDpi) == Flags::highDpi},
#elif TARGET_OS_MAC
        nativeWindow{
            newSize,
            (flags & Flags::resizable) == Flags::resizable,
            (flags & Flags::fullscreen) == Flags::fullscreen,
            (flags & Flags::exclusiveFullscreen) == Flags::exclusiveFullscreen,
            newTitle,
            graphicsDriver,
            (flags & Flags::highDpi) == Flags::highDpi
        },
#elif defined(__ANDROID__)
        nativeWindow{newTitle},
#elif defined(__linux__)
        nativeWindow{
            newSize,
            (flags & Flags::resizable) == Flags::resizable,
            (flags & Flags::fullscreen) == Flags::fullscreen,
            (flags & Flags::exclusiveFullscreen) == Flags::exclusiveFullscreen,
            newTitle
        },
#elif defined(_WIN32)
        nativeWindow{
            newSize,
            (flags & Flags::resizable) == Flags::resizable,
            (flags & Flags::fullscreen) == Flags::fullscreen,
            (flags & Flags::exclusiveFullscreen) == Flags::exclusiveFullscreen,
            newTitle,
            (flags & Flags::highDpi) == Flags::highDpi
        },
#elif defined(__EMSCRIPTEN__)
        nativeWindow{
            newSize,
            (flags & Flags::fullscreen) == Flags::fullscreen,
            newTitle,
            (flags & Flags::highDpi) == Flags::highDpi
        },
#else
        nativeWindow{
            newSize,
            (flags & Flags::resizable) == Flags::resizable,
            (flags & Flags::fullscreen) == Flags::fullscreen,
            (flags & Flags::exclusiveFullscreen) == Flags::exclusiveFullscreen,
            newTitle,
            (flags & Flags::highDpi) == Flags::highDpi
        },
#endif
        size{nativeWindow.getSize()},
        resolution{nativeWindow.getResolution()},
        resizable{(flags & Flags::resizable) == Flags::resizable},
        fullscreen{(flags & Flags::fullscreen) == Flags::fullscreen},
        exclusiveFullscreen{(flags & Flags::exclusiveFullscreen) == Flags::exclusiveFullscreen},
        highDpi{(flags & Flags::highDpi) == Flags::highDpi},
        title{newTitle}
    {
    }

    std::queue<std::unique_ptr<Event>> Window::getEvents(bool waitForEvents)
    {
        auto events = nativeWindow.getEvents(waitForEvents);

        std::queue<std::unique_ptr<Event>> result;

        while (!events.empty())
        {
            const auto event = std::move(events.front());
            events.pop();

            switch (event.type)
            {
                case NativeWindow::Event::Type::sizeChange:
                {
                    size = event.size;

                    auto sizeChangeEvent = std::make_unique<WindowEvent>();
                    sizeChangeEvent->type = Event::Type::windowSizeChange;
                    sizeChangeEvent->window = this;
                    sizeChangeEvent->size = event.size;
                    result.push(std::move(sizeChangeEvent));
                    break;
                }
                case NativeWindow::Event::Type::resolutionChange:
                {
                    resolution = event.size;

                    auto resolutionChangeEvent = std::make_unique<WindowEvent>();
                    resolutionChangeEvent->type = Event::Type::resolutionChange;
                    resolutionChangeEvent->window = this;
                    resolutionChangeEvent->size = event.size;
                    result.push(std::move(resolutionChangeEvent));
                    break;
                }
                case NativeWindow::Event::Type::fullscreenChange:
                {
                    fullscreen = event.fullscreen;

                    auto fullscreenChangeEvent = std::make_unique<WindowEvent>();
                    fullscreenChangeEvent->type = Event::Type::fullscreenChange;
                    fullscreenChangeEvent->window = this;
                    fullscreenChangeEvent->fullscreen = event.fullscreen;
                    result.push(std::move(fullscreenChangeEvent));
                    break;
                }
                case NativeWindow::Event::Type::screenChange:
                {
                    displayId = event.displayId;

                    auto screenChangeEvent = std::make_unique<WindowEvent>();
                    screenChangeEvent->type = Event::Type::screenChange;
                    screenChangeEvent->window = this;
                    screenChangeEvent->screenId = event.displayId;
                    result.push(std::move(screenChangeEvent));
                    break;
                }
                case NativeWindow::Event::Type::focusChange:
                    // handled by applicationDidBecomeActive: and applicationDidResignActive:
                    break;
                case NativeWindow::Event::Type::close:
                    // handled by applicationShouldTerminateAfterLastWindowClosed:
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
                    throw std::runtime_error{"Unhandled event"};
            }
        }

        return result;
    }

    void Window::close()
    {
        NativeWindow::Command command{NativeWindow::Command::Type::close};
        nativeWindow.addCommand(command);
    }

    void Window::setSize(const math::Size<std::uint32_t, 2>& newSize)
    {
        if (size != newSize)
        {
            size = newSize;

            NativeWindow::Command command{NativeWindow::Command::Type::changeSize};
            command.size = newSize;
            nativeWindow.addCommand(command);
        }
    }

    void Window::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen)
        {
            fullscreen = newFullscreen;

            NativeWindow::Command command{NativeWindow::Command::Type::changeFullscreen};
            command.fullscreen = newFullscreen;
            nativeWindow.addCommand(command);
        }
    }

    void Window::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            title = newTitle;

            NativeWindow::Command command{NativeWindow::Command::Type::setTitle};
            command.title = newTitle;
            nativeWindow.addCommand(command);
        }
    }

    void Window::bringToFront()
    {
        NativeWindow::Command command{NativeWindow::Command::Type::bringToFront};
        nativeWindow.addCommand(command);
    }

    void Window::show()
    {
        NativeWindow::Command command{NativeWindow::Command::Type::show};
        nativeWindow.addCommand(command);
    }

    void Window::hide()
    {
        NativeWindow::Command command{NativeWindow::Command::Type::hide};
        nativeWindow.addCommand(command);
    }

    void Window::minimize()
    {
        NativeWindow::Command command{NativeWindow::Command::Type::minimize};
        nativeWindow.addCommand(command);
    }

    void Window::maximize()
    {
        NativeWindow::Command command{NativeWindow::Command::Type::maximize};
        nativeWindow.addCommand(command);
    }

    void Window::restore()
    {
        NativeWindow::Command command{NativeWindow::Command::Type::restore};
        nativeWindow.addCommand(command);
    }
}
