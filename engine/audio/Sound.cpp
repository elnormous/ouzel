// Ouzel by Elviss Strazdins

#include "Sound.hpp"
#include "Audio.hpp"

namespace ouzel::audio
{
    Sound::Sound(Audio& initAudio, std::size_t initSourceId, Format initFormat):
        audio{initAudio},
        sourceId{initSourceId},
        format{initFormat}
    {
    }

    Sound::~Sound()
    {
        if (sourceId) audio.deleteObject(sourceId);
    }
}
