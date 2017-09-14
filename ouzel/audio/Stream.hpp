// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <atomic>
#include "utils/Noncopyable.hpp"

namespace ouzel
{
    namespace audio
    {
        class Stream: public Noncopyable
        {
        public:
            Stream();
        	virtual ~Stream();

            virtual void reset();

            bool isPlaying() const { return playing; }
            void setPlaying(bool newPlaying) { playing = newPlaying; }

            bool isRepeating() { return repeating; }
            void setRepeating(bool newRepeating) { repeating = newRepeating; }

            bool getShouldReset() const { return shouldReset; }
            void setShouldReset(bool newReset) { shouldReset = newReset; }

            uint32_t getResetCount() { return resetCount; }

        private:
            std::atomic<bool> playing;
            std::atomic<bool> repeating;
            std::atomic<bool> shouldReset;
            std::atomic<uint32_t> resetCount;
        };
    } // namespace audio
} // namespace ouzel
