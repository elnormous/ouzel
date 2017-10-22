// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "LoaderWave.hpp"
#include "Cache.hpp"
#include "audio/SoundDataWave.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderWave::LoaderWave():
            Loader({"wav"})
        {
        }

        bool LoaderWave::loadAsset(const std::string& filename)
        {
            std::shared_ptr<audio::SoundDataWave> soundData = std::make_shared<audio::SoundDataWave>();
            if (!soundData->init(filename))
            {
                return false;
            }

            cache->setSoundData(filename, soundData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
