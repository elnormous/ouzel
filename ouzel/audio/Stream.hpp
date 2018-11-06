// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STREAM_HPP
#define OUZEL_STREAM_HPP

#include <cstdint>
#include <atomic>
#include <mutex>

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
            std::atomic_bool playing;
            std::atomic_bool repeating;
            std::atomic_bool shouldReset;

            std::mutex listenerMutex;
            EventListener* eventListener = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_STREAM_HPP
