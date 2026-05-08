#include <cctype>
#include "AssetError.hpp"
#include "Bundle.hpp"
#include "Cache.hpp"
#include "../formats/Json.hpp"

namespace fluxion::assets
{
    namespace
    {
        // Drop the resource's previous Uuid binding (if any) before we
        // record a new one. Keeps the reverse map consistent when the
        // same name is set twice with different ids.
        void unbindUuid(std::map<std::string, Uuid, std::less<>>& forward,
                        std::unordered_map<Uuid, std::string>& reverse,
                        std::string_view name)
        {
            if (const auto i = forward.find(name); i != forward.end())
            {
                if (!i->second.isNull())
                    reverse.erase(i->second);
                forward.erase(i);
            }
        }

        void bindUuid(std::map<std::string, Uuid, std::less<>>& forward,
                      std::unordered_map<Uuid, std::string>& reverse,
                      std::string_view name,
                      const Uuid& uuid)
        {
            unbindUuid(forward, reverse, name);
            if (uuid.isNull()) return;
            const std::string nameStr{name};
            forward.try_emplace(nameStr, uuid);
            reverse[uuid] = nameStr;
        }

        Uuid lookupId(const std::map<std::string, Uuid, std::less<>>& forward,
                      std::string_view name)
        {
            if (const auto i = forward.find(name); i != forward.end())
                return i->second;
            return {};
        }
    }

    Bundle::Bundle(Cache& initCache, storage::FileSystem& initFileSystem):
        cache{initCache}, fileSystem{initFileSystem}
    {
        cache.addBundle(this);
    }

    Bundle::~Bundle()
    {
        cache.removeBundle(this);
    }

    void Bundle::loadAsset(Asset::Type assetType, const std::string& name,
                           const std::string& filename,
                           const Asset::Options& options,
                           const Uuid& uuid)
    {
        const auto data = fileSystem.readFile(filename);

        const auto& loaders = cache.getLoaders();

        // A null Uuid from the caller means "I don't care about a
        // stable id" — we still mint one so the reverse index works
        // uniformly. Editor-side imports always pass a real Uuid.
        const Uuid effectiveUuid = uuid.isNull() ? Uuid::generate() : uuid;

        for (auto i = loaders.rbegin(); i != loaders.rend(); ++i)
            if (i->first == assetType &&
                i->second(cache, *this, effectiveUuid, name, data, options))
                return;

        throw Error{"Failed to load asset " + filename};
    }

    void Bundle::loadAssets(const std::string& filename)
    {
        const auto data = json::parse(fileSystem.readFile(filename));

        for (const auto& asset : data["assets"])
        {
            const auto& file = asset["filename"].as<std::string>();
            const auto& name = asset.hasMember("name") ? asset["name"].as<std::string>() : file;

            Asset::Options options;
            options.mipmaps = asset.hasMember("mipmaps") ? asset["mipmaps"].as<bool>() : true;
            loadAsset(static_cast<Asset::Type>(asset["type"].as<std::uint32_t>()), name, file, options);
        }
    }

    void Bundle::loadAssets(const std::vector<Asset>& assets)
    {
        for (const auto& asset : assets)
            loadAsset(asset.type, asset.name, asset.filename, asset.options);
    }

    std::shared_ptr<graphics::Texture> Bundle::getTexture(std::string_view name) const
    {
        if (const auto i = textures.find(name); i != textures.end())
            return i->second;

        return nullptr;
    }

    std::shared_ptr<graphics::Texture> Bundle::getTexture(const Uuid& uuid) const
    {
        if (uuid.isNull()) return nullptr;
        if (const auto i = texturesById.find(uuid); i != texturesById.end())
            return getTexture(i->second);
        return nullptr;
    }

    void Bundle::setTexture(std::string_view name,
                            const std::shared_ptr<graphics::Texture>& texture,
                            const Uuid& uuid)
    {
        const auto i = textures.find(name);
        if (i == textures.end())
            textures.try_emplace(std::string(name), texture);
        else
            i->second = texture;

        bindUuid(textureIdsByName, texturesById, name, uuid);
    }

    void Bundle::releaseTextures()
    {
        textures.clear();
        textureIdsByName.clear();
        texturesById.clear();
    }

    const graphics::Shader* Bundle::getShader(std::string_view name) const
    {
        if (const auto i = shaders.find(name); i != shaders.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setShader(std::string_view name, std::unique_ptr<graphics::Shader> shader)
    {
        const auto i = shaders.find(name);
        if (i == shaders.end())
            shaders.try_emplace(std::string(name), std::move(shader));
        else
            i->second = std::move(shader);
    }

    void Bundle::releaseShaders()
    {
        shaders.clear();
    }

    const graphics::BlendState* Bundle::getBlendState(std::string_view name) const
    {
        if (const auto i = blendStates.find(name); i != blendStates.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setBlendState(std::string_view name, std::unique_ptr<graphics::BlendState> blendState)
    {
        const auto i = blendStates.find(name);
        if (i == blendStates.end())
            blendStates.try_emplace(std::string(name), std::move(blendState));
        else
            i->second = std::move(blendState);
    }

    void Bundle::releaseBlendStates()
    {
        blendStates.clear();
    }

    const graphics::DepthStencilState* Bundle::getDepthStencilState(std::string_view name) const
    {
        if (const auto i = depthStencilStates.find(name); i != depthStencilStates.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setDepthStencilState(std::string_view name, std::unique_ptr<graphics::DepthStencilState> depthStencilState)
    {
        const auto i = depthStencilStates.find(name);
        if (i == depthStencilStates.end())
            depthStencilStates.try_emplace(std::string(name), std::move(depthStencilState));
        else
            i->second = std::move(depthStencilState);
    }

    void Bundle::releaseDepthStencilStates()
    {
        depthStencilStates.clear();
    }

    const scene::SpriteData* Bundle::getSpriteData(std::string_view name) const
    {
        if (const auto i = spriteData.find(name); i != spriteData.end())
            return &i->second;

        return nullptr;
    }

    const scene::SpriteData* Bundle::getSpriteData(const Uuid& uuid) const
    {
        if (uuid.isNull()) return nullptr;
        if (const auto i = spriteDataById.find(uuid); i != spriteDataById.end())
            return getSpriteData(i->second);
        return nullptr;
    }

    void Bundle::setSpriteData(std::string_view name, const scene::SpriteData& newSpriteData, const Uuid& uuid)
    {
        const auto i = spriteData.find(name);
        if (i == spriteData.end())
            spriteData.try_emplace(std::string(name), newSpriteData);
        else
            i->second = newSpriteData;

        bindUuid(spriteDataIdsByName, spriteDataById, name, uuid);
    }

    void Bundle::releaseSpriteData()
    {
        spriteData.clear();
        spriteDataIdsByName.clear();
        spriteDataById.clear();
    }

    const scene::ParticleSystemData* Bundle::getParticleSystemData(std::string_view name) const
    {
        if (const auto i = particleSystemData.find(name); i != particleSystemData.end())
            return &i->second;

        return nullptr;
    }

    const scene::ParticleSystemData* Bundle::getParticleSystemData(const Uuid& uuid) const
    {
        if (uuid.isNull()) return nullptr;
        if (const auto i = particleSystemDataById.find(uuid); i != particleSystemDataById.end())
            return getParticleSystemData(i->second);
        return nullptr;
    }

    void Bundle::setParticleSystemData(std::string_view name, const scene::ParticleSystemData& newParticleSystemData, const Uuid& uuid)
    {
        const auto i = particleSystemData.find(name);
        if (i == particleSystemData.end())
            particleSystemData.try_emplace(std::string(name), newParticleSystemData);
        else
            i->second = newParticleSystemData;

        bindUuid(particleSystemDataIdsByName, particleSystemDataById, name, uuid);
    }

    void Bundle::releaseParticleSystemData()
    {
        particleSystemData.clear();
        particleSystemDataIdsByName.clear();
        particleSystemDataById.clear();
    }

    const gui::Font* Bundle::getFont(std::string_view name) const
    {
        if (const auto i = fonts.find(name); i != fonts.end())
            return i->second.get();

        return nullptr;
    }

    const gui::Font* Bundle::getFont(const Uuid& uuid) const
    {
        if (uuid.isNull()) return nullptr;
        if (const auto i = fontsById.find(uuid); i != fontsById.end())
            return getFont(i->second);
        return nullptr;
    }

    void Bundle::setFont(std::string_view name, std::unique_ptr<gui::Font> font, const Uuid& uuid)
    {
        const auto i = fonts.find(name);
        if (i == fonts.end())
            fonts.try_emplace(std::string(name), std::move(font));
        else
            i->second = std::move(font);

        bindUuid(fontIdsByName, fontsById, name, uuid);
    }

    void Bundle::releaseFonts()
    {
        fonts.clear();
        fontIdsByName.clear();
        fontsById.clear();
    }

    const audio::Cue* Bundle::getCue(std::string_view name) const
    {
        if (const auto i = cues.find(name); i != cues.end())
            return i->second.get();

        return nullptr;
    }

    const audio::Cue* Bundle::getCue(const Uuid& uuid) const
    {
        if (uuid.isNull()) return nullptr;
        if (const auto i = cuesById.find(uuid); i != cuesById.end())
            return getCue(i->second);
        return nullptr;
    }

    void Bundle::setCue(std::string_view name, std::unique_ptr<audio::Cue> cue, const Uuid& uuid)
    {
        const auto i = cues.find(name);
        if (i == cues.end())
            cues.try_emplace(std::string(name), std::move(cue));
        else
            i->second = std::move(cue);

        bindUuid(cueIdsByName, cuesById, name, uuid);
    }

    void Bundle::releaseCues()
    {
        cues.clear();
        cueIdsByName.clear();
        cuesById.clear();
    }

    const audio::Sound* Bundle::getSound(std::string_view name) const
    {
        if (const auto i = sounds.find(name); i != sounds.end())
            return i->second.get();

        return nullptr;
    }

    const audio::Sound* Bundle::getSound(const Uuid& uuid) const
    {
        if (uuid.isNull()) return nullptr;
        if (const auto i = soundsById.find(uuid); i != soundsById.end())
            return getSound(i->second);
        return nullptr;
    }

    void Bundle::setSound(std::string_view name, std::unique_ptr<audio::Sound> sound, const Uuid& uuid)
    {
        const auto i = sounds.find(name);
        if (i == sounds.end())
            sounds.try_emplace(std::string(name), std::move(sound));
        else
            i->second = std::move(sound);

        bindUuid(soundIdsByName, soundsById, name, uuid);
    }

    void Bundle::releaseSounds()
    {
        sounds.clear();
        soundIdsByName.clear();
        soundsById.clear();
    }

    const graphics::Material* Bundle::getMaterial(std::string_view name) const
    {
        if (const auto i = materials.find(name); i != materials.end())
            return i->second.get();

        return nullptr;
    }

    const graphics::Material* Bundle::getMaterial(const Uuid& uuid) const
    {
        if (uuid.isNull()) return nullptr;
        if (const auto i = materialsById.find(uuid); i != materialsById.end())
            return getMaterial(i->second);
        return nullptr;
    }

    void Bundle::setMaterial(std::string_view name, std::unique_ptr<graphics::Material> material, const Uuid& uuid)
    {
        const auto i = materials.find(name);
        if (i == materials.end())
            materials.try_emplace(std::string(name), std::move(material));
        else
            i->second = std::move(material);

        bindUuid(materialIdsByName, materialsById, name, uuid);
    }

    void Bundle::releaseMaterials()
    {
        materials.clear();
        materialIdsByName.clear();
        materialsById.clear();
    }

    const scene::SkinnedMeshData* Bundle::getSkinnedMeshData(std::string_view name) const
    {
        if (const auto i = skinnedMeshData.find(name); i != skinnedMeshData.end())
            return &i->second;

        return nullptr;
    }

    const scene::SkinnedMeshData* Bundle::getSkinnedMeshData(const Uuid& uuid) const
    {
        if (uuid.isNull()) return nullptr;
        if (const auto i = skinnedMeshDataById.find(uuid); i != skinnedMeshDataById.end())
            return getSkinnedMeshData(i->second);
        return nullptr;
    }

    void Bundle::setSkinnedMeshData(std::string_view name, scene::SkinnedMeshData&& newSkinnedMeshData, const Uuid& uuid)
    {
        const auto i = skinnedMeshData.find(name);
        if (i == skinnedMeshData.end())
            skinnedMeshData.try_emplace(std::string(name), std::move(newSkinnedMeshData));
        else
            i->second = std::move(newSkinnedMeshData);

        bindUuid(skinnedMeshDataIdsByName, skinnedMeshDataById, name, uuid);
    }

    void Bundle::releaseSkinnedMeshData()
    {
        skinnedMeshData.clear();
        skinnedMeshDataIdsByName.clear();
        skinnedMeshDataById.clear();
    }

    const scene::StaticMeshData* Bundle::getStaticMeshData(std::string_view name) const
    {
        if (const auto i = staticMeshData.find(name); i != staticMeshData.end())
            return &i->second;

        return nullptr;
    }

    const scene::StaticMeshData* Bundle::getStaticMeshData(const Uuid& uuid) const
    {
        if (uuid.isNull()) return nullptr;
        if (const auto i = staticMeshDataById.find(uuid); i != staticMeshDataById.end())
            return getStaticMeshData(i->second);
        return nullptr;
    }

    void Bundle::setStaticMeshData(std::string_view name, scene::StaticMeshData&& newStaticMeshData, const Uuid& uuid)
    {
        const auto i = staticMeshData.find(name);
        if (i == staticMeshData.end())
            staticMeshData.try_emplace(std::string(name), std::move(newStaticMeshData));
        else
            i->second = std::move(newStaticMeshData);

        bindUuid(staticMeshDataIdsByName, staticMeshDataById, name, uuid);
    }

    void Bundle::releaseStaticMeshData()
    {
        staticMeshData.clear();
        staticMeshDataIdsByName.clear();
        staticMeshDataById.clear();
    }

    Uuid Bundle::getTextureId(std::string_view name) const           { return lookupId(textureIdsByName, name); }
    Uuid Bundle::getSpriteDataId(std::string_view name) const        { return lookupId(spriteDataIdsByName, name); }
    Uuid Bundle::getParticleSystemDataId(std::string_view name) const{ return lookupId(particleSystemDataIdsByName, name); }
    Uuid Bundle::getFontId(std::string_view name) const              { return lookupId(fontIdsByName, name); }
    Uuid Bundle::getCueId(std::string_view name) const               { return lookupId(cueIdsByName, name); }
    Uuid Bundle::getSoundId(std::string_view name) const             { return lookupId(soundIdsByName, name); }
    Uuid Bundle::getMaterialId(std::string_view name) const          { return lookupId(materialIdsByName, name); }
    Uuid Bundle::getSkinnedMeshDataId(std::string_view name) const   { return lookupId(skinnedMeshDataIdsByName, name); }
    Uuid Bundle::getStaticMeshDataId(std::string_view name) const    { return lookupId(staticMeshDataIdsByName, name); }
}
