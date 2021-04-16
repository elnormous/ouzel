// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_OBJLOADER_HPP
#define OUZEL_ASSETS_OBJLOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class ObjLoader final: public Loader
    {
    public:
        explicit ObjLoader(Cache& initCache);
        bool loadAsset(Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       bool mipmaps = true) override;
    };
}

#endif // OUZEL_ASSETS_OBJLOADER_HPP
