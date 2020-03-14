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
                ChangeSize,
                ChangeFullscreen,
                Close,
                SetTitle,
                BringToFront,
                Show,
                Hide,
                Minimize,
                Maximize,
                Restore
            };

            Command() = default;
            explicit Command(Type initType) noexcept: type(initType) {}

            Type type;
            Size2U size;
            bool fullscreen;
            std::string title;
        };

        class Event final
        {
        public:
            enum class Type
            {
                SizeChange,
                ResolutionChange,
                FullscreenChange,
                ScreenChange,
                FocusChange,
                Close,
                Show,
                Hide,
                Minimize,
                Maximize,
                Restore
            };

            Event() = default;
            explicit Event(Type initType) noexcept: type(initType) {}

            Type type;

            Size2U size;
            union
            {
                bool fullscreen;
                std::uint32_t displayId;
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
        virtual ~NativeWindow() = default;

        NativeWindow(const NativeWindow&) = delete;
        NativeWindow& operator=(const NativeWindow&) = delete;

        NativeWindow(NativeWindow&&) = delete;
        NativeWindow& operator=(NativeWindow&&) = delete;

        void addCommand(const Command& command);
        virtual void executeCommand(const Command&) {}

        inline auto& getSize() const noexcept { return size; }
        inline auto& getResolution() const noexcept { return resolution; }
        inline auto getContentScale() const noexcept { return contentScale; }
        inline auto isResizable() const noexcept { return resizable; }
        inline auto isFullscreen() const noexcept { return fullscreen; }
        inline auto isExclusiveFullscreen() const noexcept { return exclusiveFullscreen; }
        inline auto& getTitle() const noexcept { return title; }

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
