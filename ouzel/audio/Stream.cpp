// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Stream.hpp"

namespace ouzel
{
    namespace audio
    {
        Stream::Listener::~Listener()
        {
        }

        Stream::Stream():
            playing(false), shouldReset(false)
        {
        }

        Stream::~Stream()
        {
        }

        void Stream::reset()
        {
            std::unique_lock<std::mutex> lock(listenerMutex);
            if (listener) listener->onReset();
            if (!repeating)
            {
                playing = false;
                if (listener) listener->onStop();
            }
        }

        void Stream::setListener(Listener* newListener)
        {
            std::unique_lock<std::mutex> lock(listenerMutex);
            listener = newListener;
        }
    } // namespace audio
} // namespace ouzel
