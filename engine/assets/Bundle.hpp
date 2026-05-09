#ifndef FLUXION_ASSETS_BUNDLE_HPP
#define FLUXION_ASSETS_BUNDLE_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include "Asset.hpp"
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
#include "../utils/Uuid.hpp"

namespace fluxion::assets
{
    class Cache;

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

        // Load a single asset. If `uuid` is null, a fresh v4 Uuid is
        // generated and assigned to the resulting resource — this is
        // the path samples take when they don't care about scene-stable
        // ids. Editor-side imports pass the .meta-stored Uuid so the
        // same resource keeps its identity across runs.
        void loadAsset(Asset::Type assetType, const std::string& name,
                       const std::string& filename, const Asset::Options& options,
                       const Uuid& uuid = {});
        void loadAssets(const std::string& filename);
        void loadAssets(const std::vector<Asset>& assets);

        std::shared_ptr<graphics::Texture> getTexture(std::string_view name) const;
        std::shared_ptr<graphics::Texture> getTexture(const Uuid& uuid) const;
        void setTexture(std::string_view name, const std::shared_ptr<graphics::Texture>& texture, const Uuid& uuid = {});
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

        const scene::SpriteData* getSpriteData(std::string_view name) const;
        const scene::SpriteData* getSpriteData(const Uuid& uuid) const;
        void setSpriteData(std::string_view name, const scene::SpriteData& newSpriteData, const Uuid& uuid = {});
        void releaseSpriteData();

        const scene::ParticleSystemData* getParticleSystemData(std::string_view name) const;
        const scene::ParticleSystemData* getParticleSystemData(const Uuid& uuid) const;
        void setParticleSystemData(std::string_view name, const scene::ParticleSystemData& newParticleSystemData, const Uuid& uuid = {});
        void releaseParticleSystemData();

        const gui::Font* getFont(std::string_view name) const;
        const gui::Font* getFont(const Uuid& uuid) const;
        void setFont(std::string_view name, std::unique_ptr<gui::Font> font, const Uuid& uuid = {});
        void releaseFonts();

        const audio::Cue* getCue(std::string_view name) const;
        const audio::Cue* getCue(const Uuid& uuid) const;
        void setCue(std::string_view name, std::unique_ptr<audio::Cue> cue, const Uuid& uuid = {});
        void releaseCues();

        const audio::Sound* getSound(std::string_view name) const;
        const audio::Sound* getSound(const Uuid& uuid) const;
        void setSound(std::string_view name, std::unique_ptr<audio::Sound> sound, const Uuid& uuid = {});
        void releaseSounds();

        const graphics::Material* getMaterial(std::string_view name) const;
        const graphics::Material* getMaterial(const Uuid& uuid) const;
        void setMaterial(std::string_view name, std::unique_ptr<graphics::Material> material, const Uuid& uuid = {});
        void releaseMaterials();

        const scene::SkinnedMeshData* getSkinnedMeshData(std::string_view name) const;
        const scene::SkinnedMeshData* getSkinnedMeshData(const Uuid& uuid) const;
        void setSkinnedMeshData(std::string_view name, scene::SkinnedMeshData&& newSkinnedMeshData, const Uuid& uuid = {});
        void releaseSkinnedMeshData();

        const scene::StaticMeshData* getStaticMeshData(std::string_view name) const;
        const scene::StaticMeshData* getStaticMeshData(const Uuid& uuid) const;
        void setStaticMeshData(std::string_view name, scene::StaticMeshData&& newStaticMeshData, const Uuid& uuid = {});
        void releaseStaticMeshData();

        // Reverse lookup: given a resource's name, return its Uuid (or
        // null Uuid if no id is registered). Useful for editor tools
        // that need to write the id into a .meta file.
        Uuid getTextureId(std::string_view name) const;
        Uuid getSpriteDataId(std::string_view name) const;
        Uuid getParticleSystemDataId(std::string_view name) const;
        Uuid getFontId(std::string_view name) const;
        Uuid getCueId(std::string_view name) const;
        Uuid getSoundId(std::string_view name) const;
        Uuid getMaterialId(std::string_view name) const;
        Uuid getSkinnedMeshDataId(std::string_view name) const;
        Uuid getStaticMeshDataId(std::string_view name) const;

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

        // Reverse indices: Uuid -> name for the maps above. Each
        // resource gets at most one entry in its reverse map (the last
        // setX call wins). The string-keyed maps stay primary, so
        // existing samples keep working without any code change.
        std::unordered_map<Uuid, std::string> texturesById;
        std::unordered_map<Uuid, std::string> spriteDataById;
        std::unordered_map<Uuid, std::string> particleSystemDataById;
        std::unordered_map<Uuid, std::string> fontsById;
        std::unordered_map<Uuid, std::string> cuesById;
        std::unordered_map<Uuid, std::string> soundsById;
        std::unordered_map<Uuid, std::string> materialsById;
        std::unordered_map<Uuid, std::string> skinnedMeshDataById;
        std::unordered_map<Uuid, std::string> staticMeshDataById;

        // Forward index used by getXId(name) so we don't have to scan
        // the reverse map. Same caveat: last setX call wins.
        std::map<std::string, Uuid, std::less<>> textureIdsByName;
        std::map<std::string, Uuid, std::less<>> spriteDataIdsByName;
        std::map<std::string, Uuid, std::less<>> particleSystemDataIdsByName;
        std::map<std::string, Uuid, std::less<>> fontIdsByName;
        std::map<std::string, Uuid, std::less<>> cueIdsByName;
        std::map<std::string, Uuid, std::less<>> soundIdsByName;
        std::map<std::string, Uuid, std::less<>> materialIdsByName;
        std::map<std::string, Uuid, std::less<>> skinnedMeshDataIdsByName;
        std::map<std::string, Uuid, std::less<>> staticMeshDataIdsByName;
    };
}

#endif // FLUXION_ASSETS_BUNDLE_HPP
