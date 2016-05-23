// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Noncopyable.h"
#include "Size2.h"
#include "Settings.h"
#include "Renderer.h"

namespace ouzel
{
    class Engine;
    
    class Window: public Noncopyable
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

    protected:
        Window(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle);
        virtual bool init();

        Size2 size;
        bool resizable = false;
        bool fullscreen = false;

        std::string title;
    };
}
