// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIX_HPP
#define OUZEL_AUDIO_MIX_HPP

#include <cstdint>
#include <vector>

namespace ouzel
{
    namespace audio
    {
        class Audio;
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

            inline auto getBusId() const noexcept { return busId; }

        protected:
            void addInput(Submix* submix);
            void removeInput(Submix* submix);
            void addInput(Voice* voice);
            void removeInput(Voice* voice);
            void addListener(Listener* listener);
            void removeListener(Listener* listener);

            Audio& audio;
            uintptr_t busId;
            std::vector<Submix*> inputSubmixes;
            std::vector<Voice*> inputVoices;
            std::vector<Listener*> listeners;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIX_HPP
