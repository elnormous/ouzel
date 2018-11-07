// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderWave.hpp"
#include "Bundle.hpp"
#include "audio/SoundDataWave.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderWave::LoaderWave(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool LoaderWave::loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool)
        {
            try
            {
                std::shared_ptr<audio::SoundData> soundData = std::make_shared<audio::SoundDataWave>(data);
                bundle.setSoundData(filename, soundData);
            }
            catch (const std::exception&)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
