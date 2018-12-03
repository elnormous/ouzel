// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Listener.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "scene/Actor.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        Listener::Listener(Audio& initAudio):
            audio(initAudio),
            scene::Component(scene::Component::LISTENER)
        {
        }

        void Listener::updateTransform()
        {
            transformDirty = true;
        }
    } // namespace audio
} // namespace ouzel
