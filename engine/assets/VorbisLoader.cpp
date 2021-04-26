// Ouzel by Elviss Strazdins

#include "VorbisLoader.hpp"
#include "Bundle.hpp"
#include "../audio/VorbisClip.hpp"
#include "../core/Engine.hpp"

namespace ouzel::assets
{
    VorbisLoader::VorbisLoader():
        Loader{Type::sound}
    {
    }

    bool VorbisLoader::loadAsset(Cache& cache,
                                 Bundle& bundle,
                                 const std::string& name,
                                 const std::vector<std::byte>& data,
                                 bool)
    {
        (void)cache;

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
}
