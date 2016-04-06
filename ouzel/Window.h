// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Noncopyable.h"
#include "Size2.h"
#include "Renderer.h"

namespace ouzel
{
    class Window: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~Window();

        virtual void close();

        virtual const Size2& getSize() const { return _size; }
        virtual void setSize(const Size2& size);
        virtual bool getResizable() const { return _resizable; }

        virtual void setFullscreen(bool fullscreen);
        virtual bool isFullscreen() const { return _fullscreen; }

        virtual const std::string& getTitle() const { return _title; }
        virtual void setTitle(const std::string& title);

    protected:
        Window(const Size2& size, bool resizable, bool fullscreen, const std::string& title, video::Renderer::Driver driver);
        virtual bool init();

        Size2 _size;
        bool _resizable = false;
        bool _fullscreen = false;

        std::string _title;
        
        video::Renderer::Driver _driver;
    };
}
