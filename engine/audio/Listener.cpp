// Ouzel by Elviss Strazdins

#include "Listener.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Submix.hpp"
#include "../scene/Actor.hpp"
#include "../math/Scalar.hpp"

namespace ouzel::audio
{
    Listener::Listener(Audio& initAudio) noexcept:
        audio{initAudio}
    {
    }

    Listener::~Listener()
    {
        if (mix) mix->removeListener(this);
    }

    void Listener::setMix(Mix* newMix)
    {
        if (mix) mix->removeListener(this);
        mix = newMix;
        if (mix) mix->addListener(this);
    }

    void Listener::updateTransform()
    {
        transformDirty = true;
    }
}
