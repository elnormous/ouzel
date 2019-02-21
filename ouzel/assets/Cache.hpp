// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_CACHE_HPP
#define OUZEL_ASSETS_CACHE_HPP

#include <map>
#include <memory>
#include <string>
#include "assets/Bundle.hpp"
#include "assets/BmfLoader.hpp"
#include "assets/ColladaLoader.hpp"
#include "assets/GltfLoader.hpp"
#include "assets/ImageLoader.hpp"
#include "assets/MtlLoader.hpp"
#include "assets/ObjLoader.hpp"
#include "assets/ParticleSystemLoader.hpp"
#include "assets/SpriteLoader.hpp"
#include "assets/TtfLoader.hpp"
#include "assets/VorbisLoader.hpp"
#include "assets/WaveLoader.hpp"

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
            Cache();

            Cache(const Cache&) = delete;
            Cache& operator=(const Cache&) = delete;

            Cache(Cache&&) = delete;
            Cache& operator=(Cache&&) = delete;

            const std::vector<Bundle*>& getBundles() const { return bundles; }
            const std::vector<Loader*>& getLoaders() const { return loaders; }

            std::shared_ptr<graphics::Texture> getTexture(const std::string& filename) const;
            std::shared_ptr<graphics::Shader> getShader(const std::string& shaderName) const;
            std::shared_ptr<graphics::BlendState> getBlendState(const std::string& blendStateName) const;
            std::shared_ptr<graphics::DepthStencilState> getDepthStencilState(const std::string& depthStencilStateName) const;
            const scene::SpriteData* getSpriteData(const std::string& filename) const;
            const scene::ParticleSystemData* getParticleSystemData(const std::string& filename) const;
            std::shared_ptr<gui::Font> getFont(const std::string& filename) const;
            std::shared_ptr<audio::Sound> getSound(const std::string& filename) const;
            std::shared_ptr<graphics::Material> getMaterial(const std::string& filename) const;
            const scene::SkinnedMeshData* getSkinnedMeshData(const std::string& filename) const;
            const scene::StaticMeshData* getStaticMeshData(const std::string& filename) const;

        private:
            void addBundle(Bundle* bundle);
            void removeBundle(Bundle* bundle);

            void addLoader(Loader* loader);
            void removeLoader(Loader* loader);

            std::vector<Bundle*> bundles;
            std::vector<Loader*> loaders;

            BmfLoader loaderBMF;
            ColladaLoader loaderCollada;
            GltfLoader loaderGLTF;
            ImageLoader loaderImage;
            MtlLoader loaderMTL;
            ObjLoader loaderOBJ;
            ParticleSystemLoader loaderParticleSystem;
            SpriteLoader loaderSprite;
            TtfLoader loaderTTF;
            VorbisLoader loaderVorbis;
            WaveLoader loaderWave;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_ASSETS_CACHE_HPP
