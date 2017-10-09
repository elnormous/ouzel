// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <atomic>
#include <mutex>
#include "utils/Noncopyable.hpp"

namespace ouzel
{
    namespace audio
    {
        class Stream: public Noncopyable
        {
        public:
            class Listener
            {
            public:
                virtual ~Listener() = 0;
                virtual void onReset() = 0;
                virtual void onStop() = 0;
            };

            Stream();
            virtual ~Stream();

            virtual void reset();

            bool isPlaying() const { return playing; }
            void setPlaying(bool newPlaying) { playing = newPlaying; }

            bool isRepeating() { return repeating; }
            void setRepeating(bool newRepeating) { repeating = newRepeating; }

            bool getShouldReset() const { return shouldReset; }
            void setShouldReset(bool newReset) { shouldReset = newReset; }

            void setListener(Listener* newListener);

        private:
            std::atomic<bool> playing;
            std::atomic<bool> repeating;
            std::atomic<bool> shouldReset;

            std::mutex listenerMutex;
            Listener* listener = nullptr;
        };
    } // namespace audio
} // namespace ouzel
