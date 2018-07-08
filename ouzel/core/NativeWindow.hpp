// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <string>
#include "math/Size2.hpp"
#include "thread/Mutex.hpp"

namespace ouzel
{
    class NativeWindow
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

        NativeWindow(const Size2& newSize,
                     bool newResizable,
                     bool newFullscreen,
                     bool newExclusiveFullscreen,
                     const std::string& newTitle,
                     bool newHighDpi);
        virtual ~NativeWindow() {}

        NativeWindow(const NativeWindow&) = delete;
        NativeWindow& operator=(const NativeWindow&) = delete;

        NativeWindow(NativeWindow&&) = delete;
        NativeWindow& operator=(NativeWindow&&) = delete;

        virtual void close();

        inline const Size2& getSize() const { return size; }
        virtual void setSize(const Size2& newSize);
        inline const Size2& getResolution() const { return resolution; }
        inline float getContentScale() const { return contentScale; }

        inline bool isResizable() const { return resizable; }

        virtual void setFullscreen(bool newFullscreen);
        inline bool isFullscreen() const { return fullscreen; }

        inline bool isExclusiveFullscreen() const { return exclusiveFullscreen; }

        inline const std::string& getTitle() const { return title; }
        virtual void setTitle(const std::string& newTitle);

        void setListener(Listener* newListener);

    protected:
        Size2 size;
        Size2 resolution;
        float contentScale = 1.0F;
        bool resizable = false;
        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true;

        std::string title;

        Mutex listenerMutex;
        Listener* listener = nullptr;
    };
}

