// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_OBJLOADER_HPP
#define OUZEL_ASSETS_OBJLOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class ObjLoader final: public Loader
    {
    public:
        explicit ObjLoader(): Loader{Asset::Type::staticMesh} {}
        bool loadAsset(Cache& cache,
                       Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       const Asset::Options& options) override;
    };
}

#endif // OUZEL_ASSETS_OBJLOADER_HPP
