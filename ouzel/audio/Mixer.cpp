// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Mixer.hpp"
#include "MixerResource.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace audio
    {
        Mixer::Mixer()
        {
            resource = sharedEngine->getAudio()->getDevice()->createMixer();
        }

        Mixer::~Mixer()
        {
            if (sharedEngine) sharedEngine->getAudio()->getDevice()->deleteResource(resource);
        }
    } // namespace audio
} // namespace ouzel
