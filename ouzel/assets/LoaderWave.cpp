// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderWave.hpp"
#include "Bundle.hpp"
#include "audio/WaveSound.hpp"
#include "core/Engine.hpp"

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
                std::shared_ptr<audio::Sound> sound = std::make_shared<audio::WaveSound>(*engine->getAudio(), data);
                bundle.setSound(filename, sound);
            }
            catch (const std::exception&)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
