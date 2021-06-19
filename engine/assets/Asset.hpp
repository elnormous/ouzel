// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_ASSET_HPP
#define OUZEL_ASSETS_ASSET_HPP

#include <string>

namespace ouzel::assets
{
    class Asset final
    {
    public:
        enum class Type
        {
            bank,
            font,
            staticMesh,
            skinnedMesh,
            image,
            material,
            particleSystem,
            sprite,
            sound,
            cue
        };

        struct Options final
        {
            bool mipmaps = true;
        };

        Asset(Type initType,
              const std::string& initName,
              const std::string& initFilename,
              const Options& initOptions):
            type{initType},
            name{initName},
            filename{initFilename},
            options{initOptions}
        {
        }

        Type type;
        std::string name;
        std::string filename;
        Options options;
    };
}

#endif // OUZEL_ASSETS_ASSET_HPP
