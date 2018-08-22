// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <mutex>
#include <string>
#include <queue>
#include "math/Size2.hpp"
#include "thread/Mutex.hpp"

namespace ouzel
{
    class NativeWindow
    {
    public:
        class Event
        {
        public:
            enum class Type
            {
                SIZE_CHANGE,
                RESOLUTION_CHANGE,
                FULLSCREEN_CHANGE,
                SCREEN_CHANGE,
                CLOSE
            };

            Event(Type initType): type(initType) {}
            Event(Type initType, const Size2& initSize): type(initType), size(initSize) {}
            Event(Type initType, bool initFullscreen): type(initType), fullscreen(initFullscreen) {}
            Event(Type initType, uint32_t initDisplayId): type(initType), displayId(initDisplayId) {}

            Type type;

            union
            {
                Size2 size;
                bool fullscreen;
                uint32_t displayId;
            };
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

        std::vector<Event> getEvents() const;

    protected:
        void pushEvent(const Event& event);

        Size2 size;
        Size2 resolution;
        float contentScale = 1.0F;
        bool resizable = false;
        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true;

        std::string title;

        mutable std::mutex eventQueueMutex;
        mutable std::queue<Event> eventQueue;
    };
}
