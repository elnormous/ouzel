// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "utils/Noncopyable.hpp"
#include "math/Size2.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    class Engine;

    class Window: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~Window();

        virtual void close();

        const Size2& getSize() const { return size; }
        virtual void setSize(const Size2& newSize);

        const Size2& getResolution() const { return resolution; }

        bool getResizable() const { return resizable; }

        virtual void setFullscreen(bool newFullscreen);
        bool isFullscreen() const { return fullscreen; }

        bool isExclusiveFullscreen() const { return exclusiveFullscreen; }

        const std::string& getTitle() const { return title; }
        virtual void setTitle(const std::string& newTitle);

        Vector2 convertWindowToNormalizedLocation(const Vector2& position) const
        {
            return Vector2(position.x / size.width,
                           1.0f - (position.y / size.height));
        }

        Vector2 convertWindowToNormalizedLocationRelative(const Vector2& position) const
        {
            return Vector2(position.x / size.width,
                           -position.y / size.height);
        }

        Vector2 convertNormalizedToWindowLocation(const Vector2& position) const
        {
            return Vector2(position.x * size.width,
                           (1.0f - position.y) * size.height);
        }

        Vector2 convertNormalizedToWindowLocationRelative(const Vector2& position) const
        {
            return Vector2(position.x * size.width,
                           -position.y * size.height);
        }

    protected:
        Window();
        virtual bool init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth);

        bool handleWindowChange(Event::Type type, const WindowEvent& event);

        Size2 size;
        Size2 resolution;
        bool resizable = false;
        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true;

        std::string title;

        ouzel::EventHandler eventHandler;
    };
}
