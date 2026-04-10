#ifndef FLUXION_AUDIO_SOUND_HPP
#define FLUXION_AUDIO_SOUND_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace fluxion::audio
{
    class Audio;

    class Sound
    {
        friend Audio;
    public:
        enum class Format
        {
            pcm,
            vorbis
        };

        Sound(Audio& initAudio, std::size_t initSourceId, Format initFormat);
        virtual ~Sound();

        Sound(const Sound&) = delete;
        Sound& operator=(const Sound&) = delete;

        Sound(Sound&&) = delete;
        Sound& operator=(Sound&&) = delete;

        auto getSourceId() const noexcept { return sourceId; }
        auto getFormat() const noexcept { return format; }

    protected:
        Audio& audio;
        std::size_t sourceId = 0;
        Format format;
    };
}

#endif // FLUXION_AUDIO_SOUND_HPP
