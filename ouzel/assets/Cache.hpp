// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CACHE_HPP
#define OUZEL_CACHE_HPP

#include <memory>
#include <string>
#include <map>
#include "assets/Bundle.hpp"
#include "assets/LoaderBMF.hpp"
#include "assets/LoaderCollada.hpp"
#include "assets/LoaderGLTF.hpp"
#include "assets/LoaderImage.hpp"
#include "assets/LoaderMTL.hpp"
#include "assets/LoaderOBJ.hpp"
#include "assets/LoaderParticleSystem.hpp"
#include "assets/LoaderSprite.hpp"
#include "assets/LoaderTTF.hpp"
#include "assets/LoaderVorbis.hpp"
#include "assets/LoaderWave.hpp"
#include "files/FileSystem.hpp"

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
            explicit Cache(FileSystem& initFileSystem);

            Cache(const Cache&) = delete;
            Cache& operator=(const Cache&) = delete;

            Cache(Cache&&) = delete;
            Cache& operator=(Cache&&) = delete;

            FileSystem& getFileSystem() const { return fileSystem; }
            const std::vector<Bundle*>& getBundles() const { return bundles; }
            const std::vector<Loader*>& getLoaders() const { return loaders; }

            std::shared_ptr<graphics::Texture> getTexture(const std::string& filename) const;
            std::shared_ptr<graphics::Shader> getShader(const std::string& shaderName) const;
            std::shared_ptr<graphics::BlendState> getBlendState(const std::string& blendStateName) const;
            const scene::SpriteData* getSpriteData(const std::string& filename) const;
            const scene::ParticleSystemData* getParticleSystemData(const std::string& filename) const;
            std::shared_ptr<Font> getFont(const std::string& filename) const;
            std::shared_ptr<audio::SoundData> getSoundData(const std::string& filename) const;
            std::shared_ptr<graphics::Material> getMaterial(const std::string& filename) const;
            const scene::SkinnedMeshData* getSkinnedMeshData(const std::string& filename) const;
            const scene::StaticMeshData* getStaticMeshData(const std::string& filename) const;

        private:
            void addBundle(Bundle* bundle);
            void removeBundle(Bundle* bundle);

            void addLoader(Loader* loader);
            void removeLoader(Loader* loader);

            FileSystem& fileSystem;

            std::vector<Bundle*> bundles;
            std::vector<Loader*> loaders;

            LoaderBMF loaderBMF;
            LoaderCollada loaderCollada;
            LoaderGLTF loaderGLTF;
            LoaderImage loaderImage;
            LoaderMTL loaderMTL;
            LoaderOBJ loaderOBJ;
            LoaderParticleSystem loaderParticleSystem;
            LoaderSprite loaderSprite;
            LoaderTTF loaderTTF;
            LoaderVorbis loaderVorbis;
            LoaderWave loaderWave;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_CACHE_HPP
