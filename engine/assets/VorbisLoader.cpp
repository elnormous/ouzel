// Ouzel by Elviss Strazdins

#include "VorbisLoader.hpp"
#include "Bundle.hpp"
#include "../audio/VorbisClip.hpp"
#include "../core/Engine.hpp"

namespace ouzel::assets
{
    VorbisLoader::VorbisLoader(Cache& initCache):
        Loader(initCache, Type::sound)
    {
    }

    bool VorbisLoader::loadAsset(Bundle& bundle,
                                 const std::string& name,
                                 const std::vector<std::byte>& data,
                                 bool)
    {
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
