// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_SOOURCE_HPP
#define OUZEL_AUDIO_MIXER_SOOURCE_HPP

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Source
            {
            public:
                virtual ~Source() {}

                virtual void getSamples(uint32_t frames, std::vector<float>& samples) = 0;
            };
        }
    }
}

#endif // OUZEL_AUDIO_MIXER_SOOURCE_HPP
