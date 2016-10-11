// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <memory>
#include "utils/Noncopyable.h"
#include "math/Size2.h"
#include "graphics/Renderer.h"

namespace ouzel
{
    class Engine;

    class Window: public Noncopyable, public std::enable_shared_from_this<Window>
    {
        friend Engine;
    public:
        virtual ~Window();

        virtual void close();

        virtual const Size2& getSize() const { return size; }
        virtual void setSize(const Size2& newSize);
        virtual bool getResizable() const { return resizable; }

        virtual void setFullscreen(bool newFullscreen);
        virtual bool isFullscreen() const { return fullscreen; }

        virtual const std::string& getTitle() const { return title; }
        virtual void setTitle(const std::string& newTitle);

        virtual float getContentScale() const { return 1.0f; }

        Vector2 convertWindowToNormalizedLocation(const Vector2& position) const
        {
            return Vector2(position.x / size.width,
                           1.0f - (position.y / size.height));
        }

        Vector2 convertWindowToNormalizedLocationRelative(const Vector2& position) const
        {
            return Vector2(position.x / size.width,
                           -(position.y / size.height));
        }

        Vector2 convertNormalizedToWindowLocation(const Vector2& position) const
        {
            return Vector2(position.x * size.width,
                           (1.0f - position.y) * size.height);
        }

        Vector2 convertNormalizedToWindowLocationRelative(const Vector2& position) const
        {
            return Vector2(position.x * size.width,
                           -(position.y) * size.height);
        }

    protected:
        Window(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle);
        virtual bool init();

        Size2 size;
        bool resizable = false;
        bool fullscreen = false;

        std::string title;
    };
}
