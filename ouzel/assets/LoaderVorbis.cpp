// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderVorbis.hpp"
#include "Bundle.hpp"
#include "audio/SoundDataVorbis.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderVorbis::LoaderVorbis(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool LoaderVorbis::loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool)
        {
            try
            {
                std::shared_ptr<audio::SoundData> soundData = std::make_shared<audio::SoundDataVorbis>(data);
                bundle.setSoundData(filename, soundData);
            }
            catch (...)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
