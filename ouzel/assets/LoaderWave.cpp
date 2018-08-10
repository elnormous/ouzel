// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderWave.hpp"
#include "Cache.hpp"
#include "audio/SoundDataWave.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderWave::LoaderWave():
            Loader(TYPE)
        {
        }

        bool LoaderWave::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool)
        {
            try
            {
                std::shared_ptr<audio::SoundData> soundData = std::make_shared<audio::SoundDataWave>(data);
                cache->setSoundData(filename, soundData);
            }
            catch (...)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
