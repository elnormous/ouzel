// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_BUNDLE_HPP
#define OUZEL_ASSETS_BUNDLE_HPP

#include <map>
#include <memory>
#include <string>
#include "audio/Sound.hpp"
#include "files/FileSystem.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/DepthStencilState.hpp"
#include "graphics/Material.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"
#include "gui/Font.hpp"
#include "scene/SkinnedMeshRenderer.hpp"
#include "scene/StaticMeshRenderer.hpp"
#include "scene/Sprite.hpp"
#include "scene/ParticleSystem.hpp"

namespace ouzel
{
    namespace assets
    {
        class Cache;

        class Asset final
        {
        public:
            Asset(uint32_t initType,
                  std::string initFilename,
                  bool initMipmaps = true):
                type(initType),
                filename(initFilename),
                mipmaps(initMipmaps)
            {
            }

            uint32_t type;
            std::string filename;
            bool mipmaps;
        };

        class Bundle final
        {
            friend Cache;
        public:
            Bundle(Cache& initCache, FileSystem& initFileSystem);
            ~Bundle();

            Bundle(const Bundle&) = delete;
            Bundle& operator=(const Bundle&) = delete;

            Bundle(Bundle&&) = delete;
            Bundle& operator=(Bundle&&) = delete;

            void loadAsset(uint32_t loaderType, const std::string& filename, bool mipmaps = true);
            void loadAssets(const std::string& filename);
            void loadAssets(const std::vector<Asset>& assets);

            void clear();

            std::shared_ptr<graphics::Texture> getTexture(const std::string& filename) const;
            void setTexture(const std::string& filename, const std::shared_ptr<graphics::Texture>& texture);
            void releaseTextures();

            std::shared_ptr<graphics::Shader> getShader(const std::string& shaderName) const;
            void setShader(const std::string& shaderName, const std::shared_ptr<graphics::Shader>& shader);
            void releaseShaders();

            std::shared_ptr<graphics::BlendState> getBlendState(const std::string& blendStateName) const;
            void setBlendState(const std::string& blendStateName, const std::shared_ptr<graphics::BlendState>& blendState);
            void releaseBlendStates();

            std::shared_ptr<graphics::DepthStencilState> getDepthStencilState(const std::string& depthStencilStateName) const;
            void setDepthStencilState(const std::string& depthStencilStateName, const std::shared_ptr<graphics::DepthStencilState>& depthStencilState);
            void releaseDepthStencilStates();

            void preloadSpriteData(const std::string& filename, bool mipmaps = true,
                                   uint32_t spritesX = 1, uint32_t spritesY = 1,
                                   const Vector2<float>& pivot = Vector2<float>(0.5F, 0.5F));
            const scene::SpriteData* getSpriteData(const std::string& filename) const;
            void setSpriteData(const std::string& filename, const scene::SpriteData& newSpriteData);
            void releaseSpriteData();

            const scene::ParticleSystemData* getParticleSystemData(const std::string& filename) const;
            void setParticleSystemData(const std::string& filename, const scene::ParticleSystemData& newParticleSystemData);
            void releaseParticleSystemData();

            std::shared_ptr<Font> getFont(const std::string& filename) const;
            void setFont(const std::string& filename, const std::shared_ptr<Font>& font);
            void releaseFonts();

            std::shared_ptr<audio::Sound> getSound(const std::string& filename) const;
            void setSound(const std::string& filename, const std::shared_ptr<audio::Sound>& newSound);
            void releaseSound();

            std::shared_ptr<graphics::Material> getMaterial(const std::string& filename) const;
            void setMaterial(const std::string& filename, const std::shared_ptr<graphics::Material>& material);
            void releaseMaterials();

            const scene::SkinnedMeshData* getSkinnedMeshData(const std::string& filename) const;
            void setSkinnedMeshData(const std::string& filename, const scene::SkinnedMeshData& newSkinnedMeshData);
            void releaseSkinnedMeshData();

            const scene::StaticMeshData* getStaticMeshData(const std::string& filename) const;
            void setStaticMeshData(const std::string& filename, const scene::StaticMeshData& newStaticMeshData);
            void releaseStaticMeshData();

        private:
            Cache& cache;
            FileSystem& fileSystem;

            std::map<std::string, std::shared_ptr<graphics::Texture>> textures;
            std::map<std::string, std::shared_ptr<graphics::Shader>> shaders;
            std::map<std::string, scene::ParticleSystemData> particleSystemData;
            std::map<std::string, std::shared_ptr<graphics::BlendState>> blendStates;
            std::map<std::string, std::shared_ptr<graphics::DepthStencilState>> depthStencilStates;
            std::map<std::string, scene::SpriteData> spriteData;
            std::map<std::string, std::shared_ptr<Font>> fonts;
            std::map<std::string, std::shared_ptr<audio::Sound>> sounds;
            std::map<std::string, std::shared_ptr<graphics::Material>> materials;
            std::map<std::string, scene::SkinnedMeshData> skinnedMeshData;
            std::map<std::string, scene::StaticMeshData> staticMeshData;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_ASSETS_BUNDLE_HPP
