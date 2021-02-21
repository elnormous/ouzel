// Ouzel by Elviss Strazdins

#include <stdexcept>
#include <emscripten.h>
#include <emscripten/html5.h>
#include "NativeWindowEm.hpp"

namespace ouzel::core::emscripten
{
    namespace
    {
        EM_BOOL emResizeCallback(int eventType, const EmscriptenUiEvent* uiEvent, void* userData)
        {
            if (eventType == EMSCRIPTEN_EVENT_RESIZE)
            {
                auto nativeWindowEm = static_cast<NativeWindow*>(userData);
                nativeWindowEm->handleResize();
                return EM_TRUE;
            }

            return EM_FALSE;
        }

        EM_BOOL emCanvasResizeCallback(int eventType, const void*, void* userData)
        {
            if (eventType == EMSCRIPTEN_EVENT_CANVASRESIZED)
            {
                auto nativeWindowEm = static_cast<NativeWindow*>(userData);
                nativeWindowEm->handleResize();
                return EM_TRUE;
            }

            return EM_FALSE;
        }

        EM_BOOL emFullscreenCallback(int eventType, const EmscriptenFullscreenChangeEvent* fullscreenChangeEvent, void* userData)
        {
            if (eventType == EMSCRIPTEN_EVENT_FULLSCREENCHANGE)
            {
                auto nativeWindowEm = static_cast<NativeWindow*>(userData);
                nativeWindowEm->handleFullscreenChange(fullscreenChangeEvent->isFullscreen == EM_TRUE);
                return EM_TRUE;
            }

            return EM_FALSE;
        }
    }

    NativeWindow::NativeWindow(const std::function<void(const Event&)>& initCallback,
                               const Size2U& newSize,
                               bool newFullscreen,
                               const std::string& newTitle,
                               bool newHighDpi):
        core::NativeWindow(initCallback,
                           newSize,
                           true,
                           newFullscreen,
                           true,
                           newTitle,
                           newHighDpi)
    {
        emscripten_set_resize_callback(nullptr, this, EM_TRUE, emResizeCallback);
        emscripten_set_fullscreenchange_callback(nullptr, this, EM_TRUE, emFullscreenCallback);

        if (size.v[0] == 0 || size.v[1] == 0)
        {
            double width;
            double height;
            emscripten_get_element_css_size("#canvas", &width, &height);

            if (size.v[0] == 0) size.v[0] = static_cast<std::uint32_t>(width);
            if (size.v[1] == 0) size.v[1] = static_cast<std::uint32_t>(height);
        }
        else
            emscripten_set_element_css_size("#canvas",
                                            static_cast<double>(size.v[0]),
                                            static_cast<double>(size.v[1]));

        if (fullscreen)
        {
            EmscriptenFullscreenStrategy s;
            s.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_STRETCH;
            s.canvasResolutionScaleMode = highDpi ? EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF : EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
            s.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
            s.canvasResizedCallback = emCanvasResizeCallback;
            s.canvasResizedCallbackUserData = this;

            emscripten_request_fullscreen_strategy(nullptr, EM_TRUE, &s);
        }

        resolution = size;
    }

    void NativeWindow::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::changeSize:
                setSize(command.size);
                break;
            case Command::Type::changeFullscreen:
                setFullscreen(command.fullscreen);
                break;
            case Command::Type::close:
                break;
            case Command::Type::setTitle:
                break;
            case Command::Type::bringToFront:
                break;
            case Command::Type::show:
                break;
            case Command::Type::hide:
                break;
            case Command::Type::minimize:
                break;
            case Command::Type::maximize:
                break;
            case Command::Type::restore:
                break;
            default:
                throw std::runtime_error("Invalid command");
        }
    }

    void NativeWindow::setSize(const Size2U& newSize)
    {
        size = newSize;

        emscripten_set_element_css_size("#canvas",
                                        static_cast<double>(size.v[0]),
                                        static_cast<double>(size.v[1]));
    }

    void NativeWindow::setFullscreen(bool newFullscreen)
    {
        fullscreen = newFullscreen;

        if (newFullscreen)
        {
            EmscriptenFullscreenStrategy s;
            s.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_STRETCH;
            s.canvasResolutionScaleMode = highDpi ? EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF : EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
            s.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
            s.canvasResizedCallback = emCanvasResizeCallback;
            s.canvasResizedCallbackUserData = this;

            emscripten_request_fullscreen_strategy(nullptr, EM_TRUE, &s);
        }
        else
            emscripten_exit_fullscreen();
    }

    void NativeWindow::handleResize()
    {
        double width;
        double height;
        emscripten_get_element_css_size("#canvas", &width, &height);

        const Size2U newSize(static_cast<std::uint32_t>(width),
                             static_cast<std::uint32_t>(height));

        size = newSize;
        resolution = size;

        Event sizeChangeEvent(Event::Type::sizeChange);
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::resolutionChange);
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindow::handleFullscreenChange(bool newFullscreen)
    {
        fullscreen = newFullscreen;

        Event fullscreenChangeEvent(Event::Type::fullscreenChange);
        fullscreenChangeEvent.fullscreen = fullscreen;
        sendEvent(fullscreenChangeEvent);
    }
}
