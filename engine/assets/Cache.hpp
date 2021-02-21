// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_CACHE_HPP
#define OUZEL_ASSETS_CACHE_HPP

#include <memory>
#include <string>
#include "Bundle.hpp"

namespace ouzel::assets
{
    class Bundle;
    class Loader;

    class Cache final
    {
        friend Bundle;
    public:
        Cache();

        Cache(const Cache&) = delete;
        Cache& operator=(const Cache&) = delete;

        Cache(Cache&&) = delete;
        Cache& operator=(Cache&&) = delete;

        auto& getBundles() const noexcept { return bundles; }
        auto& getLoaders() const noexcept { return loaders; }

        std::shared_ptr<graphics::Texture> getTexture(const std::string& name) const;
        const graphics::Shader* getShader(const std::string& name) const;
        const graphics::BlendState* getBlendState(const std::string& name) const;
        const graphics::DepthStencilState* getDepthStencilState(const std::string& name) const;
        const scene::SpriteData* getSpriteData(const std::string& name) const;
        const scene::ParticleSystemData* getParticleSystemData(const std::string& name) const;
        const gui::Font* getFont(const std::string& name) const;
        const audio::Cue* getCue(const std::string& name) const;
        const audio::Sound* getSound(const std::string& name) const;
        const graphics::Material* getMaterial(const std::string& name) const;
        const scene::SkinnedMeshData* getSkinnedMeshData(const std::string& name) const;
        const scene::StaticMeshData* getStaticMeshData(const std::string& name) const;

    private:
        void addBundle(const Bundle* bundle);
        void removeBundle(const Bundle* bundle);

        void addLoader(std::unique_ptr<Loader> loader);
        void removeLoader(const Loader* loader);

        std::vector<const Bundle*> bundles;
        std::vector<std::unique_ptr<Loader>> loaders;
    };
}

#endif // OUZEL_ASSETS_CACHE_HPP
