// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOW_HPP
#define OUZEL_CORE_NATIVEWINDOW_HPP

#include <mutex>
#include <string>
#include <queue>
#include "math/Size.hpp"

namespace ouzel
{
    class NativeWindow
    {
    public:
        class Command final
        {
        public:
            enum class Type
            {
                CHANGE_SIZE,
                CHANGE_FULLSCREEN,
                CLOSE,
                SET_TITLE
            };

            Command() {}
            explicit Command(Type initType): type(initType) {}

            Type type;

            union
            {
                Size2U size;
                bool fullscreen;
            };

            std::string title;
        };

        class Event final
        {
        public:
            enum class Type
            {
                SIZE_CHANGE,
                RESOLUTION_CHANGE,
                FULLSCREEN_CHANGE,
                SCREEN_CHANGE,
                FOCUS_CHANGE,
                CLOSE
            };

            Event() {}
            explicit Event(Type initType): type(initType) {}

            Type type;

            union
            {
                Size2U size;
                Size2U resolution;
                bool fullscreen;
                uint32_t displayId;
                bool focus;
            };
        };

        NativeWindow(const std::function<void(const Event&)>& initCallback,
                     const Size2U& newSize,
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

        void addCommand(const Command& command);
        virtual void executeCommand(const Command&) {}

        inline const Size2U& getSize() const { return size; }
        inline const Size2U& getResolution() const { return resolution; }
        inline float getContentScale() const { return contentScale; }
        inline bool isResizable() const { return resizable; }
        inline bool isFullscreen() const { return fullscreen; }
        inline bool isExclusiveFullscreen() const { return exclusiveFullscreen; }
        inline const std::string& getTitle() const { return title; }

    protected:
        void sendEvent(const Event& event);

        Size2U size;
        Size2U resolution;
        float contentScale = 1.0F;
        bool resizable = false;
        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true;

        std::string title;

    private:
        std::function<void(const Event&)> callback;

        std::mutex commandQueueMutex;
        std::queue<Event> commandQueue;
    };
}

#endif // OUZEL_CORE_NATIVEWINDOW_HPP
