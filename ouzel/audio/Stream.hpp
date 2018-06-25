// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <atomic>
#include "thread/Mutex.hpp"

namespace ouzel
{
    namespace audio
    {
        class Stream
        {
        public:
            class EventListener
            {
            public:
                virtual ~EventListener() = 0;
                virtual void onReset() = 0;
                virtual void onStop() = 0;
            };

            Stream();
            virtual ~Stream();

            Stream(const Stream&) = delete;
            Stream& operator=(const Stream&) = delete;

            Stream(Stream&&) = delete;
            Stream& operator=(Stream&&) = delete;

            virtual void reset();

            bool isPlaying() const { return playing; }
            void setPlaying(bool newPlaying) { playing = newPlaying; }

            bool isRepeating() { return repeating; }
            void setRepeating(bool newRepeating) { repeating = newRepeating; }

            inline bool getShouldReset() const { return shouldReset; }
            inline void setShouldReset(bool newReset) { shouldReset = newReset; }

            void setEventListener(EventListener* newEventListener);

        private:
            std::atomic<bool> playing;
            std::atomic<bool> repeating;
            std::atomic<bool> shouldReset;

            Mutex listenerMutex;
            EventListener* eventListener = nullptr;
        };
    } // namespace audio
} // namespace ouzel
