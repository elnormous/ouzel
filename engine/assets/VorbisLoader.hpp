// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_VORBISLOADER_HPP
#define OUZEL_ASSETS_VORBISLOADER_HPP

#include "Loader.hpp"
#include "Bundle.hpp"
#include "../audio/VorbisClip.hpp"
#include "../core/Engine.hpp"

namespace ouzel::assets
{
    class VorbisLoader final: public Loader
    {
    public:
        explicit VorbisLoader() noexcept: Loader{Asset::Type::sound} {}

        bool loadAsset(Cache&,
                       Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       const Asset::Options&) override
        {
            try
            {
                auto sound = std::make_unique<audio::VorbisClip>(*engine->getAudio(), data);
                bundle.setSound(name, std::move(sound));
            }
            catch (const std::exception&)
            {
                return false;
            }

            return true;
        }
    };
}

#endif // OUZEL_ASSETS_VORBISLOADER_HPP
