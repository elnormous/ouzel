// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_WINDOW_HPP
#define OUZEL_CORE_WINDOW_HPP

#include <memory>
#include <string>
#include "core/NativeWindow.hpp"
#include "graphics/Renderer.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    class Engine;

    class Window final
    {
    public:
        Window(Engine& initEngine,
               const Size2<float>& newSize,
               bool newResizable,
               bool newFullscreen,
               bool newExclusiveFullscreen,
               const std::string& newTitle,
               graphics::Driver graphicsDriver,
               bool newHighDpi,
               bool depth);
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        inline NativeWindow* getNativeWindow() const { return nativeWindow.get(); }

        void close();
        void update();

        inline const Size2<float>& getSize() const { return size; }
        void setSize(const Size2<float>& newSize);

        inline const Size2<float>& getResolution() const { return resolution; }

        inline bool isResizable() const { return resizable; }

        void setFullscreen(bool newFullscreen);
        bool isFullscreen() const { return fullscreen; }

        bool isExclusiveFullscreen() const { return exclusiveFullscreen; }

        inline const std::string& getTitle() const { return title; }
        void setTitle(const std::string& newTitle);

        inline Vector2<float> convertWindowToNormalizedLocation(const Vector2<float>& position) const
        {
            return Vector2<float>(position.v[0] / size.width, position.v[1] / size.height);
        }

        inline Vector2<float> convertNormalizedToWindowLocation(const Vector2<float>& position) const
        {
            return Vector2<float>(position.v[0] * size.width, position.v[1] * size.height);
        }

    private:
        void eventCallback(const NativeWindow::Event& event);
        void handleEvent(const NativeWindow::Event& event);

        Engine& engine;
        std::unique_ptr<NativeWindow> nativeWindow;

        Size2<float> size;
        Size2<float> resolution;
        bool resizable = false;
        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true;
        uint32_t displayId = 0;

        std::string title;

        std::mutex eventQueueMutex;
        std::queue<NativeWindow::Event> eventQueue;
    };
}

#endif // OUZEL_CORE_WINDOW_HPP
