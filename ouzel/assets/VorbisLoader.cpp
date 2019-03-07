// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "VorbisLoader.hpp"
#include "Bundle.hpp"
#include "audio/VorbisSound.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace assets
    {
        VorbisLoader::VorbisLoader(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool VorbisLoader::loadAsset(Bundle& bundle,
                                     const std::string& name,
                                     const std::vector<uint8_t>& data,
                                     bool)
        {
            try
            {
                std::shared_ptr<audio::Sound> sound = std::make_shared<audio::VorbisSound>(*engine->getAudio(), data);
                bundle.setSound(name, sound);
            }
            catch (const std::exception&)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
