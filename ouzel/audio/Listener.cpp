// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Listener.hpp"
#include "ListenerResource.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace audio
    {
        Listener::Listener()
        {
            resource = sharedEngine->getAudio()->getDevice()->createListener();
        }

        Listener::~Listener()
        {
            if (sharedEngine) sharedEngine->getAudio()->getDevice()->deleteResource(resource);
        }
    } // namespace audio
} // namespace ouzel
