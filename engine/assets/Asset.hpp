#ifndef FLUXION_ASSETS_ASSET_HPP
#define FLUXION_ASSETS_ASSET_HPP

#include <string>

namespace fluxion::assets
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

#endif // FLUXION_ASSETS_ASSET_HPP
