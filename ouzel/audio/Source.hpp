// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SOURCE_HPP
#define OUZEL_AUDIO_SOURCE_HPP

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Source
        {
            friend Audio;
        public:
            Source(Audio& initAudio, uintptr_t initSourceId);
            virtual ~Source();

            Source(const Source&) = delete;
            Source& operator=(const Source&) = delete;

            Source(Source&&) = delete;
            Source& operator=(Source&&) = delete;

            inline uintptr_t getSourceId() const { return sourceId; }

        protected:
            Audio& audio;
            uintptr_t sourceId = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_SOURCE_HPP
