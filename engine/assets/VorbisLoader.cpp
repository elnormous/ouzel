// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "VorbisLoader.hpp"
#include "Bundle.hpp"
#include "../audio/VorbisClip.hpp"
#include "../core/Engine.hpp"

namespace ouzel
{
    namespace assets
    {
        VorbisLoader::VorbisLoader(Cache& initCache):
            Loader(initCache, Loader::sound)
        {
        }

        bool VorbisLoader::loadAsset(Bundle& bundle,
                                     const std::string& name,
                                     const std::vector<std::uint8_t>& data,
                                     bool)
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
    } // namespace assets
} // namespace ouzel
