// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_BUNDLE_HPP
#define OUZEL_ASSETS_BUNDLE_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
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

        std::shared_ptr<graphics::Texture> getTexture(std::string_view name) const;
        void setTexture(std::string_view name, const std::shared_ptr<graphics::Texture>& texture);
        void releaseTextures();

        const graphics::Shader* getShader(std::string_view shaderName) const;
        void setShader(std::string_view name, std::unique_ptr<graphics::Shader> shader);
        void releaseShaders();

        const graphics::BlendState* getBlendState(std::string_view name) const;
        void setBlendState(std::string_view name, std::unique_ptr<graphics::BlendState> blendState);
        void releaseBlendStates();

        const graphics::DepthStencilState* getDepthStencilState(std::string_view name) const;
        void setDepthStencilState(std::string_view name, std::unique_ptr<graphics::DepthStencilState> depthStencilState);
        void releaseDepthStencilStates();

        void preloadSpriteData(const std::string& filename, bool mipmaps = true,
                               std::uint32_t spritesX = 1, std::uint32_t spritesY = 1,
                               const Vector<float, 2>& pivot = Vector<float, 2>{0.5F, 0.5F});
        const scene::SpriteData* getSpriteData(std::string_view name) const;
        void setSpriteData(std::string_view name, const scene::SpriteData& newSpriteData);
        void releaseSpriteData();

        const scene::ParticleSystemData* getParticleSystemData(std::string_view name) const;
        void setParticleSystemData(std::string_view name, const scene::ParticleSystemData& newParticleSystemData);
        void releaseParticleSystemData();

        const gui::Font* getFont(std::string_view name) const;
        void setFont(std::string_view name, std::unique_ptr<gui::Font> font);
        void releaseFonts();

        const audio::Cue* getCue(std::string_view name) const;
        void setCue(std::string_view name, std::unique_ptr<audio::Cue> cue);
        void releaseCues();

        const audio::Sound* getSound(std::string_view name) const;
        void setSound(std::string_view name, std::unique_ptr<audio::Sound> sound);
        void releaseSounds();

        const graphics::Material* getMaterial(std::string_view name) const;
        void setMaterial(std::string_view name, std::unique_ptr<graphics::Material> material);
        void releaseMaterials();

        const scene::SkinnedMeshData* getSkinnedMeshData(std::string_view name) const;
        void setSkinnedMeshData(std::string_view name, scene::SkinnedMeshData&& newSkinnedMeshData);
        void releaseSkinnedMeshData();

        const scene::StaticMeshData* getStaticMeshData(std::string_view name) const;
        void setStaticMeshData(std::string_view name, scene::StaticMeshData&& newStaticMeshData);
        void releaseStaticMeshData();

    private:
        Cache& cache;
        storage::FileSystem& fileSystem;

        std::map<std::string, std::shared_ptr<graphics::Texture>, std::less<>> textures;
        std::map<std::string, std::unique_ptr<graphics::Shader>, std::less<>> shaders;
        std::map<std::string, scene::ParticleSystemData, std::less<>> particleSystemData;
        std::map<std::string, std::unique_ptr<graphics::BlendState>, std::less<>> blendStates;
        std::map<std::string, std::unique_ptr<graphics::DepthStencilState>, std::less<>> depthStencilStates;
        std::map<std::string, scene::SpriteData, std::less<>> spriteData;
        std::map<std::string, std::unique_ptr<gui::Font>, std::less<>> fonts;
        std::map<std::string, std::unique_ptr<audio::Cue>, std::less<>> cues;
        std::map<std::string, std::unique_ptr<audio::Sound>, std::less<>> sounds;
        std::map<std::string, std::unique_ptr<graphics::Material>, std::less<>> materials;
        std::map<std::string, scene::SkinnedMeshData, std::less<>> skinnedMeshData;
        std::map<std::string, scene::StaticMeshData, std::less<>> staticMeshData;
    };
}

#endif // OUZEL_ASSETS_BUNDLE_HPP
