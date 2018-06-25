// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "Stream.hpp"
#include "thread/Lock.hpp"

namespace ouzel
{
    namespace audio
    {
        Stream::EventListener::~EventListener()
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
            Lock lock(listenerMutex);
            if (eventListener) eventListener->onReset();
            if (!repeating)
            {
                playing = false;
                if (eventListener) eventListener->onStop();
            }
        }

        void Stream::setEventListener(EventListener* newEventListener)
        {
            Lock lock(listenerMutex);
            eventListener = newEventListener;
        }
    } // namespace audio
} // namespace ouzel
