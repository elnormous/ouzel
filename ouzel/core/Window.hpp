// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_WINDOW_HPP
#define OUZEL_WINDOW_HPP

#include <memory>
#include <string>
#include "core/NativeWindow.hpp"
#include "graphics/Renderer.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    class Window final: public NativeWindow::EventHandler
    {
    public:
        Window(const Size2& newSize,
               bool newResizable,
               bool newFullscreen,
               bool newExclusiveFullscreen,
               const std::string& newTitle,
               graphics::Renderer::Driver graphicsDriver,
               bool newHighDpi,
               bool depth);
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        inline NativeWindow* getNativeWindow() const { return nativeWindow.get(); }

        void close();
        void update();

        inline const Size2& getSize() const { return size; }
        void setSize(const Size2& newSize);

        inline const Size2& getResolution() const { return resolution; }

        inline bool isResizable() const { return resizable; }

        void setFullscreen(bool newFullscreen);
        bool isFullscreen() const { return fullscreen; }

        bool isExclusiveFullscreen() const { return exclusiveFullscreen; }

        inline const std::string& getTitle() const { return title; }
        void setTitle(const std::string& newTitle);

        inline Vector2 convertWindowToNormalizedLocation(const Vector2& position) const
        {
            return Vector2(position.x / size.width, position.y / size.height);
        }

        inline Vector2 convertNormalizedToWindowLocation(const Vector2& position) const
        {
            return Vector2(position.x * size.width, position.y * size.height);
        }

    private:
        bool handleEvent(const NativeWindow::Event& event) override;

        std::unique_ptr<NativeWindow> nativeWindow;

        Size2 size;
        Size2 resolution;
        bool resizable = false;
        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true;
        uint32_t displayId = 0;

        std::string title;
    };
}

#endif // OUZEL_WINDOW_HPP
