// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_WINDOW_HPP
#define OUZEL_CORE_WINDOW_HPP

#include <memory>
#include <string>
#include "core/NativeWindow.hpp"
#include "graphics/Renderer.hpp"
#include "math/Size.hpp"

namespace ouzel
{
    class Engine;

    class Window final
    {
    public:
        Window(Engine& initEngine,
               const Size<2, uint32_t>& newSize,
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

        inline const Size<2, uint32_t>& getSize() const { return size; }
        void setSize(const Size<2, uint32_t>& newSize);

        inline const Size<2, uint32_t>& getResolution() const { return resolution; }

        inline bool isResizable() const { return resizable; }

        void setFullscreen(bool newFullscreen);
        bool isFullscreen() const { return fullscreen; }

        bool isExclusiveFullscreen() const { return exclusiveFullscreen; }

        inline const std::string& getTitle() const { return title; }
        void setTitle(const std::string& newTitle);

        inline Vector2F convertWindowToNormalizedLocation(const Vector2F& position) const
        {
            return Vector2F(position.v[0] / size.v[0], position.v[1] / size.v[1]);
        }

        inline Vector2F convertNormalizedToWindowLocation(const Vector2F& position) const
        {
            return Vector2F(position.v[0] * size.v[0], position.v[1] * size.v[1]);
        }

    private:
        void eventCallback(const NativeWindow::Event& event);
        void handleEvent(const NativeWindow::Event& event);

        Engine& engine;
        std::unique_ptr<NativeWindow> nativeWindow;

        Size<2, uint32_t> size;
        Size<2, uint32_t> resolution;
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
