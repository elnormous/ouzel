// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "LoaderVorbis.hpp"
#include "Cache.hpp"
#include "audio/SoundDataVorbis.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderVorbis::LoaderVorbis():
            Loader(TYPE, {"ogg"})
        {
        }

        bool LoaderVorbis::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool)
        {
            std::shared_ptr<audio::SoundData> soundData = std::make_shared<audio::SoundDataVorbis>();
            if (!soundData->init(data))
                return false;

            cache->setSoundData(filename, soundData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
