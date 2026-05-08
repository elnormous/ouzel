#ifndef FLUXION_ASSETS_VORBISLOADER_HPP
#define FLUXION_ASSETS_VORBISLOADER_HPP

#include "Bundle.hpp"
#include "../audio/VorbisClip.hpp"
#include "../core/Engine.hpp"

namespace fluxion::assets
{
    inline bool loadVorbis(Cache&,
                           Bundle& bundle,
                           const Uuid& uuid,
                           const std::string& name,
                           const std::vector<std::byte>& data,
                           const Asset::Options&)
    {
        try
        {
            auto sound = std::make_unique<audio::VorbisClip>(engine->getAudio(), data);
            bundle.setSound(name, std::move(sound), uuid);
        }
        catch (const std::exception&)
        {
            return false;
        }

        return true;
    }
}

#endif // FLUXION_ASSETS_VORBISLOADER_HPP
