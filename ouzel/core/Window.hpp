// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <string>
#include "core/NativeWindow.hpp"
#include "graphics/Renderer.hpp"
#include "math/Size2.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    class Engine;

    class Window: public NativeWindow::Listener
    {
        friend Engine;
    public:
        virtual ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        inline NativeWindow* getNativeWindow() const { return nativeWindow; }

        void close();

        inline const Size2& getSize() const { return size; }
        void setSize(const Size2& newSize);

        inline const Size2& getResolution() const { return resolution; }

        inline bool isResizable() const { return resizable; }

        void setFullscreen(bool newFullscreen);
        bool isFullscreen() const { return fullscreen; }

        bool isExclusiveFullscreen() const { return exclusiveFullscreen; }

        inline const std::string& getTitle() const { return title; }
        virtual void setTitle(const std::string& newTitle);

        inline Vector2 convertWindowToNormalizedLocation(const Vector2& position) const
        {
            return Vector2(position.x / size.width, position.y / size.height);
        }

        inline Vector2 convertNormalizedToWindowLocation(const Vector2& position) const
        {
            return Vector2(position.x * size.width, position.y * size.height);
        }

    protected:
        Window(const Size2& newSize,
               bool newResizable,
               bool newFullscreen,
               bool newExclusiveFullscreen,
               const std::string& newTitle,
               graphics::Renderer::Driver graphicsDriver,
               bool newHighDpi,
               bool depth);

        virtual void onSizeChange(const Size2& newSize) override;
        virtual void onResolutionChange(const Size2& newResolution) override;
        virtual void onFullscreenChange(bool newFullscreen) override;
        virtual void onScreenChange(uint32_t newDisplayId) override;
        virtual void onClose() override;

        NativeWindow* nativeWindow = nullptr;

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
