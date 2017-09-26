// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <mutex>
#include "utils/Noncopyable.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    class WindowResource: public Noncopyable
    {
    public:
        class Listener
        {
        public:
            virtual ~Listener() = 0;
            virtual void onSizeChange(const Size2& newSize) = 0;
            virtual void onResolutionChange(const Size2& newResolution) = 0;
            virtual void onFullscreenChange(bool newFullscreen) = 0;
            virtual void onScreenChange(uint32_t newDisplayId) = 0;
            virtual void onClose() = 0;
        };

        virtual ~WindowResource() {}

        virtual bool init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth);

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

        void setListener(Listener* newListener);

    protected:
        Size2 size;
        Size2 resolution;
        bool resizable = false;
        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true;

        std::string title;
        
        std::mutex listenerMutex;
        Listener* listener = nullptr;
    };
}

