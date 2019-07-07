// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_CACHE_HPP
#define OUZEL_ASSETS_CACHE_HPP

#include <memory>
#include <string>
#include "assets/Bundle.hpp"

namespace ouzel
{
    namespace assets
    {
        class Bundle;
        class Loader;

        class Cache final
        {
            friend Bundle;
            friend Loader;
        public:
            Cache() = default;

            Cache(const Cache&) = delete;
            Cache& operator=(const Cache&) = delete;

            Cache(Cache&&) = delete;
            Cache& operator=(Cache&&) = delete;

            const std::vector<Bundle*>& getBundles() const { return bundles; }
            const std::vector<Loader*>& getLoaders() const { return loaders; }

            std::shared_ptr<graphics::Texture> getTexture(const std::string& name) const;
            std::shared_ptr<graphics::Shader> getShader(const std::string& name) const;
            std::shared_ptr<graphics::BlendState> getBlendState(const std::string& name) const;
            std::shared_ptr<graphics::DepthStencilState> getDepthStencilState(const std::string& name) const;
            const scene::SpriteData* getSpriteData(const std::string& name) const;
            const scene::ParticleSystemData* getParticleSystemData(const std::string& name) const;
            std::shared_ptr<gui::Font> getFont(const std::string& name) const;
            std::shared_ptr<audio::Bank> getBank(const std::string& name) const;
            std::shared_ptr<audio::Sound> getSound(const std::string& name) const;
            std::shared_ptr<graphics::Material> getMaterial(const std::string& name) const;
            const scene::SkinnedMeshData* getSkinnedMeshData(const std::string& name) const;
            const scene::StaticMeshData* getStaticMeshData(const std::string& name) const;

        private:
            void addBundle(Bundle* bundle);
            void removeBundle(Bundle* bundle);

            void addLoader(Loader* loader);
            void removeLoader(Loader* loader);

            std::vector<Bundle*> bundles;
            std::vector<Loader*> loaders;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_ASSETS_CACHE_HPP
