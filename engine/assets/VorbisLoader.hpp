// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_VORBISLOADER_HPP
#define OUZEL_ASSETS_VORBISLOADER_HPP

#include "Bundle.hpp"
#include "../audio/VorbisClip.hpp"
#include "../core/Engine.hpp"

namespace ouzel::assets
{
    inline bool loadVorbis(Cache&,
                           Bundle& bundle,
                           const std::string& name,
                           const std::vector<std::byte>& data,
                           const Asset::Options&)
    {
        try
        {
            auto sound = std::make_unique<audio::VorbisClip>(engine->getAudio(), data);
            bundle.setSound(name, std::move(sound));
        }
        catch (const std::exception&)
        {
            return false;
        }

        return true;
    }
}

#endif // OUZEL_ASSETS_VORBISLOADER_HPP
