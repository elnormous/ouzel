// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_BUNDLE_HPP
#define OUZEL_ASSETS_BUNDLE_HPP

#include <map>
#include <memory>
#include <string>
#include "Loader.hpp"
#include "../audio/Cue.hpp"
#include "../audio/Sound.hpp"
#include "../graphics/BlendState.hpp"
#include "../graphics/DepthStencilState.hpp"
#include "../graphics/Material.hpp"
#include "../graphics/Shader.hpp"
#include "../graphics/Texture.hpp"
#include "../gui/Font.hpp"
#include "../scene/SkinnedMeshRenderer.hpp"
#include "../scene/StaticMeshRenderer.hpp"
#include "../scene/SpriteRenderer.hpp"
#include "../scene/ParticleSystem.hpp"
#include "../storage/FileSystem.hpp"

namespace ouzel::assets
{
    class Cache;

    class Asset final
    {
    public:
        Asset(Loader::Type initType,
              const std::string& initName,
              const std::string& initFilename,
              bool initMipmaps = true):
            type(initType),
            name(initName),
            filename(initFilename),
            mipmaps(initMipmaps)
        {
        }

        Loader::Type type;
        std::string name;
        std::string filename;
        bool mipmaps;
    };

    class Bundle final
    {
        friend Cache;
    public:
        Bundle(Cache& initCache, storage::FileSystem& initFileSystem);
        ~Bundle();

        Bundle(const Bundle&) = delete;
        Bundle& operator=(const Bundle&) = delete;

        Bundle(Bundle&&) = delete;
        Bundle& operator=(Bundle&&) = delete;

        void loadAsset(Loader::Type loaderType, const std::string& name,
                       const std::string& filename, bool mipmaps = true);
        void loadAssets(const std::string& filename);
        void loadAssets(const std::vector<Asset>& assets);

        std::shared_ptr<graphics::Texture> getTexture(const std::string& name) const;
        void setTexture(const std::string& name, const std::shared_ptr<graphics::Texture>& texture);
        void releaseTextures();

        const graphics::Shader* getShader(const std::string& shaderName) const;
        void setShader(const std::string& name, std::unique_ptr<graphics::Shader> shader);
        void releaseShaders();

        const graphics::BlendState* getBlendState(const std::string& name) const;
        void setBlendState(const std::string& name, std::unique_ptr<graphics::BlendState> blendState);
        void releaseBlendStates();

        const graphics::DepthStencilState* getDepthStencilState(const std::string& name) const;
        void setDepthStencilState(const std::string& name, std::unique_ptr<graphics::DepthStencilState> depthStencilState);
        void releaseDepthStencilStates();

        void preloadSpriteData(const std::string& filename, bool mipmaps = true,
                               std::uint32_t spritesX = 1, std::uint32_t spritesY = 1,
                               const Vector2F& pivot = Vector2F{0.5F, 0.5F});
        const scene::SpriteData* getSpriteData(const std::string& name) const;
        void setSpriteData(const std::string& name, const scene::SpriteData& newSpriteData);
        void releaseSpriteData();

        const scene::ParticleSystemData* getParticleSystemData(const std::string& name) const;
        void setParticleSystemData(const std::string& name, const scene::ParticleSystemData& newParticleSystemData);
        void releaseParticleSystemData();

        const gui::Font* getFont(const std::string& name) const;
        void setFont(const std::string& name, std::unique_ptr<gui::Font> font);
        void releaseFonts();

        const audio::Cue* getCue(const std::string& name) const;
        void setCue(const std::string& name, std::unique_ptr<audio::Cue> cue);
        void releaseCues();

        const audio::Sound* getSound(const std::string& name) const;
        void setSound(const std::string& name, std::unique_ptr<audio::Sound> sound);
        void releaseSounds();

        const graphics::Material* getMaterial(const std::string& name) const;
        void setMaterial(const std::string& name, std::unique_ptr<graphics::Material> material);
        void releaseMaterials();

        const scene::SkinnedMeshData* getSkinnedMeshData(const std::string& name) const;
        void setSkinnedMeshData(const std::string& name, scene::SkinnedMeshData&& newSkinnedMeshData);
        void releaseSkinnedMeshData();

        const scene::StaticMeshData* getStaticMeshData(const std::string& name) const;
        void setStaticMeshData(const std::string& name, scene::StaticMeshData&& newStaticMeshData);
        void releaseStaticMeshData();

    private:
        Cache& cache;
        storage::FileSystem& fileSystem;

        std::map<std::string, std::shared_ptr<graphics::Texture>> textures;
        std::map<std::string, std::unique_ptr<graphics::Shader>> shaders;
        std::map<std::string, scene::ParticleSystemData> particleSystemData;
        std::map<std::string, std::unique_ptr<graphics::BlendState>> blendStates;
        std::map<std::string, std::unique_ptr<graphics::DepthStencilState>> depthStencilStates;
        std::map<std::string, scene::SpriteData> spriteData;
        std::map<std::string, std::unique_ptr<gui::Font>> fonts;
        std::map<std::string, std::unique_ptr<audio::Cue>> cues;
        std::map<std::string, std::unique_ptr<audio::Sound>> sounds;
        std::map<std::string, std::unique_ptr<graphics::Material>> materials;
        std::map<std::string, scene::SkinnedMeshData> skinnedMeshData;
        std::map<std::string, scene::StaticMeshData> staticMeshData;
    };
}

#endif // OUZEL_ASSETS_BUNDLE_HPP
