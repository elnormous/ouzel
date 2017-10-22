// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "LoaderVorbis.hpp"
#include "Cache.hpp"
#include "SoundDataVorbis.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderVorbis::LoaderVorbis():
            Loader({"ogg"})
        {
        }

        bool LoaderVorbis::loadAsset(const std::string& filename)
        {
            std::shared_ptr<audio::SoundDataVorbis> soundData = std::make_shared<audio::SoundDataVorbis>();
            if (!soundData->init(filename))
            {
                return false;
            }

            cache->setSoundData(filename, soundData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
