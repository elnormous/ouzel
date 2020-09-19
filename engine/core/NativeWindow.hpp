// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOW_HPP
#define OUZEL_CORE_NATIVEWINDOW_HPP

#include <mutex>
#include <string>
#include <queue>
#include "../math/Size.hpp"

namespace ouzel::core
{
    class NativeWindow
    {
    public:
        class Command final
        {
        public:
            enum class Type
            {
                changeSize,
                changeFullscreen,
                close,
                setTitle,
                bringToFront,
                show,
                hide,
                minimize,
                maximize,
                restore
            };

            Command() = default;
            explicit Command(Type initType) noexcept: type(initType) {}

            Type type;
            Size2U size;
            bool fullscreen = false;
            std::string title;
        };

        class Event final
        {
        public:
            enum class Type
            {
                sizeChange,
                resolutionChange,
                fullscreenChange,
                screenChange,
                focusChange,
                close,
                show,
                hide,
                minimize,
                maximize,
                restore
            };

            Event() = default;
            explicit Event(Type initType) noexcept: type(initType) {}

            Type type;

            Size2U size;
            union
            {
                bool fullscreen = false;
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

        auto& getSize() const noexcept { return size; }
        auto& getResolution() const noexcept { return resolution; }
        auto getContentScale() const noexcept { return contentScale; }
        auto isResizable() const noexcept { return resizable; }
        auto isFullscreen() const noexcept { return fullscreen; }
        auto isExclusiveFullscreen() const noexcept { return exclusiveFullscreen; }
        auto& getTitle() const noexcept { return title; }

    protected:
        virtual void executeCommand(const Command&) {}
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
