#ifndef FLUXION_AUDIO_SUBMIX_HPP
#define FLUXION_AUDIO_SUBMIX_HPP

#include "Mix.hpp"

namespace fluxion::audio
{
    class Submix final: public Mix
    {
        friend Mix;
    public:
        explicit Submix(Audio& initAudio);
        ~Submix() override;

        Submix(const Submix&) = delete;
        Submix& operator=(const Submix&) = delete;
        Submix(Submix&&) = delete;
        Submix& operator=(Submix&&) = delete;

        void setOutput(Mix* newOutput);

    private:
        Mix* output = nullptr;
    };
}

#endif // FLUXION_AUDIO_SUBMIX_HPP
