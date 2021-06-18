// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_CACHE_HPP
#define OUZEL_ASSETS_CACHE_HPP

#include <memory>
#include <string_view>
#include <vector>
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

        std::shared_ptr<graphics::Texture> getTexture(std::string_view name) const;
        const graphics::Shader* getShader(std::string_view name) const;
        const graphics::BlendState* getBlendState(std::string_view name) const;
        const graphics::DepthStencilState* getDepthStencilState(std::string_view name) const;
        const scene::SpriteData* getSpriteData(std::string_view name) const;
        const scene::ParticleSystemData* getParticleSystemData(std::string_view name) const;
        const gui::Font* getFont(std::string_view name) const;
        const audio::Cue* getCue(std::string_view name) const;
        const audio::Sound* getSound(std::string_view name) const;
        const graphics::Material* getMaterial(std::string_view name) const;
        const scene::SkinnedMeshData* getSkinnedMeshData(std::string_view name) const;
        const scene::StaticMeshData* getStaticMeshData(std::string_view name) const;

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
