// Ouzel by Elviss Strazdins

#include <cctype>
#include <stdexcept>
#include "Bundle.hpp"
#include "Cache.hpp"
#include "Loader.hpp"
#include "../formats/Json.hpp"

namespace ouzel::assets
{
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
                           const Asset::Options& options)
    {
        const auto data = fileSystem.readFile(filename);

        const auto& loaders = cache.getLoaders();

        for (auto i = loaders.rbegin(); i != loaders.rend(); ++i)
        {
            const auto loader = i->get();
            if (loader->getType() == assetType &&
                loader->loadAsset(cache, *this, name, data, options))
                return;
        }

        throw std::runtime_error("Failed to load asset " + filename);
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

    void Bundle::setTexture(std::string_view name, const std::shared_ptr<graphics::Texture>& texture)
    {
        const auto i = textures.find(name);
        if (i == textures.end())
            textures.insert(std::make_pair(name, texture));
        else
            i->second = texture;
    }

    void Bundle::releaseTextures()
    {
        textures.clear();
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
            shaders.insert(std::make_pair(name, std::move(shader)));
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
            blendStates.insert(std::make_pair(name, std::move(blendState)));
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
            depthStencilStates.insert(std::make_pair(name, std::move(depthStencilState)));
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

    void Bundle::setSpriteData(std::string_view name, const scene::SpriteData& newSpriteData)
    {
        const auto i = spriteData.find(name);
        if (i == spriteData.end())
            spriteData.insert(std::make_pair(name, newSpriteData));
        else
            i->second = newSpriteData;
    }

    void Bundle::releaseSpriteData()
    {
        spriteData.clear();
    }

    const scene::ParticleSystemData* Bundle::getParticleSystemData(std::string_view name) const
    {
        if (const auto i = particleSystemData.find(name); i != particleSystemData.end())
            return &i->second;

        return nullptr;
    }

    void Bundle::setParticleSystemData(std::string_view name, const scene::ParticleSystemData& newParticleSystemData)
    {
        const auto i = particleSystemData.find(name);
        if (i == particleSystemData.end())
            particleSystemData.insert(std::make_pair(name, newParticleSystemData));
        else
            i->second = newParticleSystemData;
    }

    void Bundle::releaseParticleSystemData()
    {
        particleSystemData.clear();
    }

    const gui::Font* Bundle::getFont(std::string_view name) const
    {
        if (const auto i = fonts.find(name); i != fonts.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setFont(std::string_view name, std::unique_ptr<gui::Font> font)
    {
        const auto i = fonts.find(name);
        if (i == fonts.end())
            fonts.insert(std::make_pair(name, std::move(font)));
        else
            i->second = std::move(font);
    }

    void Bundle::releaseFonts()
    {
        fonts.clear();
    }

    const audio::Cue* Bundle::getCue(std::string_view name) const
    {
        if (const auto i = cues.find(name); i != cues.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setCue(std::string_view name, std::unique_ptr<audio::Cue> cue)
    {
        const auto i = cues.find(name);
        if (i == cues.end())
            cues.insert(std::make_pair(name, std::move(cue)));
        else
            i->second = std::move(cue);
    }

    void Bundle::releaseCues()
    {
        cues.clear();
    }

    const audio::Sound* Bundle::getSound(std::string_view name) const
    {
        if (const auto i = sounds.find(name); i != sounds.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setSound(std::string_view name, std::unique_ptr<audio::Sound> sound)
    {
        const auto i = sounds.find(name);
        if (i == sounds.end())
            sounds.insert(std::make_pair(name, std::move(sound)));
        else
            i->second = std::move(sound);
    }

    void Bundle::releaseSounds()
    {
        sounds.clear();
    }

    const graphics::Material* Bundle::getMaterial(std::string_view name) const
    {
        if (const auto i = materials.find(name); i != materials.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setMaterial(std::string_view name, std::unique_ptr<graphics::Material> material)
    {
        const auto i = materials.find(name);
        if (i == materials.end())
            materials.insert(std::make_pair(name, std::move(material)));
        else
            i->second = std::move(material);
    }

    void Bundle::releaseMaterials()
    {
        materials.clear();
    }

    const scene::SkinnedMeshData* Bundle::getSkinnedMeshData(std::string_view name) const
    {
        if (const auto i = skinnedMeshData.find(name); i != skinnedMeshData.end())
            return &i->second;

        return nullptr;
    }

    void Bundle::setSkinnedMeshData(std::string_view name, scene::SkinnedMeshData&& newSkinnedMeshData)
    {
        const auto i = skinnedMeshData.find(name);
        if (i == skinnedMeshData.end())
            skinnedMeshData.insert(std::make_pair(name, std::move(newSkinnedMeshData)));
        else
            i->second = std::move(newSkinnedMeshData);
    }

    void Bundle::releaseSkinnedMeshData()
    {
        skinnedMeshData.clear();
    }

    const scene::StaticMeshData* Bundle::getStaticMeshData(std::string_view name) const
    {
        if (const auto i = staticMeshData.find(name); i != staticMeshData.end())
            return &i->second;

        return nullptr;
    }

    void Bundle::setStaticMeshData(std::string_view name, scene::StaticMeshData&& newStaticMeshData)
    {
        const auto i = staticMeshData.find(name);
        if (i == staticMeshData.end())
            staticMeshData.insert(std::make_pair(name, std::move(newStaticMeshData)));
        else
            i->second = std::move(newStaticMeshData);
    }

    void Bundle::releaseStaticMeshData()
    {
        staticMeshData.clear();
    }
}
