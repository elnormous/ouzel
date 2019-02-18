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

        bool VorbisLoader::loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool)
        {
            try
            {
                std::unique_ptr<audio::Sound> sound(new audio::VorbisSound(*engine->getAudio(), data));
                bundle.setSound(filename, std::move(sound));
            }
            catch (const std::exception&)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
