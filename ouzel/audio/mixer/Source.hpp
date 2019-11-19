// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_SOURCE_HPP
#define OUZEL_AUDIO_MIXER_SOURCE_HPP

#include <cstdint>
#include <vector>
#include "audio/mixer/Object.hpp"
#include "audio/mixer/Emitter.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Source: public Object
            {
            public:
                Source() noexcept = default;
                explicit Source(std::unique_ptr<Emitter> initEmitter) noexcept:
                    emitter(std::move(initEmitter))
                {
                }
                
                void play()
                {
                    if (emitter)
                    {
                        playing = true;
                        emitter->play();
                    }
                }

                void stop(bool shouldReset)
                {
                    playing = false;

                    if (emitter)
                        emitter->stop(shouldReset);
                }

                inline bool isPlaying() const { return playing; }

                void getSamples(uint32_t frames, uint32_t channels, uint32_t sampleRate, std::vector<float>& samples)
                {
                    samples.resize(frames * channels);
                    std::fill(samples.begin(), samples.end(), 0.0F); // TODO: fill only the needed samples

                    if (emitter)
                        emitter->getSamples(frames, channels, sampleRate, samples);
                }

            private:
                std::unique_ptr<Emitter> emitter;
                bool playing = false;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_SOURCE_HPP
