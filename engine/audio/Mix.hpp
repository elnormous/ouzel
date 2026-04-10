#ifndef FLUXION_AUDIO_MIX_HPP
#define FLUXION_AUDIO_MIX_HPP

#include <cstdint>
#include <vector>

namespace fluxion::audio
{
    class Audio;
    class Effect;
    class Listener;
    class Submix;
    class Voice;

    class Mix
    {
        friend Listener;
        friend Submix;
        friend Voice;
    public:
        explicit Mix(Audio& initAudio);
        virtual ~Mix();

        Mix(const Mix&) = delete;
        Mix& operator=(const Mix&) = delete;
        Mix(Mix&&) = delete;
        Mix& operator=(Mix&&) = delete;

        auto getBusId() const noexcept { return busId; }

        void addEffect(Effect* effect);
        void removeEffect(Effect* effect);

    protected:
        void addInput(Submix* submix);
        void removeInput(Submix* submix);
        void addInput(Voice* voice);
        void removeInput(Voice* voice);
        void addListener(Listener* listener);
        void removeListener(Listener* listener);

        Audio& audio;
        std::size_t busId = 0;
        std::vector<Submix*> inputSubmixes;
        std::vector<Voice*> inputVoices;
        std::vector<Effect*> effects;
        std::vector<Listener*> listeners;
    };
}

#endif // FLUXION_AUDIO_MIX_HPP
