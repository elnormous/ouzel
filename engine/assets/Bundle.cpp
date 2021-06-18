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

    void Bundle::loadAsset(Loader::Type loaderType, const std::string& name,
                           const std::string& filename, bool mipmaps)
    {
        const auto data = fileSystem.readFile(filename);

        const auto& loaders = cache.getLoaders();

        for (auto i = loaders.rbegin(); i != loaders.rend(); ++i)
        {
            const auto loader = i->get();
            if (loader->getType() == loaderType &&
                loader->loadAsset(cache, *this, name, data, mipmaps))
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
            const auto mipmaps = asset.hasMember("mipmaps") ? asset["mipmaps"].as<bool>() : true;
            loadAsset(static_cast<Loader::Type>(asset["type"].as<std::uint32_t>()), name, file, mipmaps);
        }
    }

    void Bundle::loadAssets(const std::vector<Asset>& assets)
    {
        for (const auto& asset : assets)
            loadAsset(asset.type, asset.name, asset.filename, asset.mipmaps);
    }

    std::shared_ptr<graphics::Texture> Bundle::getTexture(std::string_view name) const
    {
        if (const auto i = textures.find(name); i != textures.end())
            return i->second;

        return nullptr;
    }

    void Bundle::setTexture(std::string_view name, const std::shared_ptr<graphics::Texture>& texture)
    {
        textures.insert(std::make_pair(name, texture));
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
        shaders.insert(std::make_pair(name, std::move(shader)));
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
        blendStates.insert(std::make_pair(name, std::move(blendState)));
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
        depthStencilStates.insert(std::make_pair(name, std::move(depthStencilState)));
    }

    void Bundle::releaseDepthStencilStates()
    {
        depthStencilStates.clear();
    }

    void Bundle::preloadSpriteData(const std::string& filename, bool mipmaps,
                                   std::uint32_t spritesX, std::uint32_t spritesY,
                                   const Vector<float, 2>& pivot)
    {
        auto extension = std::string(storage::Path{filename}.getExtension());
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](char c) noexcept { return static_cast<char>(std::tolower(c)); });
        const std::vector<std::string> imageExtensions{"jpg", "jpeg", "png", "bmp", "tga"};

        if (std::find(imageExtensions.begin(), imageExtensions.end(), extension) != imageExtensions.end())
        {
            scene::SpriteData newSpriteData;

            if (spritesX == 0) spritesX = 1;
            if (spritesY == 0) spritesY = 1;

            newSpriteData.texture = getTexture(filename);

            const Size<float, 2> textureSize{
                static_cast<float>(newSpriteData.texture->getSize().v[0]),
                static_cast<float>(newSpriteData.texture->getSize().v[1])
            };

            if (newSpriteData.texture)
            {
                const auto spriteSize = Size<float, 2>{
                    textureSize.v[0] / static_cast<float>(spritesX),
                    textureSize.v[1] / static_cast<float>(spritesY)
                };

                scene::SpriteData::Animation animation;
                animation.frames.reserve(spritesX * spritesY);

                for (std::uint32_t x = 0; x < spritesX; ++x)
                    for (std::uint32_t y = 0; y < spritesY; ++y)
                    {
                        const Rect<float> rectangle{
                            spriteSize.v[0] * static_cast<float>(x),
                            spriteSize.v[1] * static_cast<float>(y),
                            spriteSize.v[0],
                            spriteSize.v[1]
                        };

                        animation.frames.emplace_back(filename, textureSize, rectangle, false, spriteSize, Vector<float, 2>{}, pivot);
                    }

                newSpriteData.animations[""] = std::move(animation);

                spriteData[filename] = newSpriteData;
            }
        }
        else
            loadAsset(Loader::Type::sprite, filename, filename, mipmaps);
    }

    const scene::SpriteData* Bundle::getSpriteData(std::string_view name) const
    {
        if (const auto i = spriteData.find(name); i != spriteData.end())
            return &i->second;

        return nullptr;
    }

    void Bundle::setSpriteData(std::string_view name, const scene::SpriteData& newSpriteData)
    {
        spriteData.insert(std::make_pair(name, newSpriteData));
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
        particleSystemData.insert(std::make_pair(name, newParticleSystemData));
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
        fonts.insert(std::make_pair(name, std::move(font)));
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
        cues.insert(std::make_pair(name, std::move(cue)));
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
        sounds.insert(std::make_pair(name, std::move(sound)));
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
        materials.insert(std::make_pair(name, std::move(material)));
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
        skinnedMeshData.insert(std::make_pair(name, std::move(newSkinnedMeshData)));
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
        staticMeshData.insert(std::make_pair(name, std::move(newStaticMeshData)));
    }

    void Bundle::releaseStaticMeshData()
    {
        staticMeshData.clear();
    }
}
