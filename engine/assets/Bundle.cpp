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
        cache(initCache), fileSystem(initFileSystem)
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
            Loader* loader = i->get();
            if (loader->getType() == loaderType &&
                loader->loadAsset(*this, name, data, mipmaps))
                return;
        }

        throw std::runtime_error("Failed to load asset " + filename);
    }

    void Bundle::loadAssets(const std::string& filename)
    {
        const auto data = json::parse(fileSystem.readFile(filename));

        for (const auto& asset : data["assets"])
        {
            const auto file = asset["filename"].as<std::string>();
            const auto name = asset.hasMember("name") ? asset["name"].as<std::string>() : file;
            const auto mipmaps = asset.hasMember("mipmaps") ? asset["mipmaps"].as<bool>() : true;
            loadAsset(static_cast<Loader::Type>(asset["type"].as<std::uint32_t>()), name, file, mipmaps);
        }
    }

    void Bundle::loadAssets(const std::vector<Asset>& assets)
    {
        for (const Asset& asset : assets)
            loadAsset(asset.type, asset.name, asset.filename, asset.mipmaps);
    }

    std::shared_ptr<graphics::Texture> Bundle::getTexture(const std::string& name) const
    {
        const auto i = textures.find(name);

        if (i != textures.end())
            return i->second;

        return nullptr;
    }

    void Bundle::setTexture(const std::string& name, const std::shared_ptr<graphics::Texture>& texture)
    {
        textures[name] = texture;
    }

    void Bundle::releaseTextures()
    {
        textures.clear();
    }

    const graphics::Shader* Bundle::getShader(const std::string& name) const
    {
        const auto i = shaders.find(name);

        if (i != shaders.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setShader(const std::string& name, std::unique_ptr<graphics::Shader> shader)
    {
        shaders[name] = std::move(shader);
    }

    void Bundle::releaseShaders()
    {
        shaders.clear();
    }

    const graphics::BlendState* Bundle::getBlendState(const std::string& name) const
    {
        const auto i = blendStates.find(name);

        if (i != blendStates.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setBlendState(const std::string& name, std::unique_ptr<graphics::BlendState> blendState)
    {
        blendStates[name] = std::move(blendState);
    }

    void Bundle::releaseBlendStates()
    {
        blendStates.clear();
    }

    const graphics::DepthStencilState* Bundle::getDepthStencilState(const std::string& name) const
    {
        const auto i = depthStencilStates.find(name);

        if (i != depthStencilStates.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setDepthStencilState(const std::string& name, std::unique_ptr<graphics::DepthStencilState> depthStencilState)
    {
        depthStencilStates[name] = std::move(depthStencilState);
    }

    void Bundle::releaseDepthStencilStates()
    {
        depthStencilStates.clear();
    }

    void Bundle::preloadSpriteData(const std::string& filename, bool mipmaps,
                                   std::uint32_t spritesX, std::uint32_t spritesY,
                                   const Vector<float, 2>& pivot)
    {
        auto extension = std::string(storage::Path(filename).getExtension());
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
                    textureSize.v[0] / spritesX,
                    textureSize.v[1] / spritesY
                };

                scene::SpriteData::Animation animation;
                animation.frames.reserve(spritesX * spritesY);

                for (std::uint32_t x = 0; x < spritesX; ++x)
                {
                    for (std::uint32_t y = 0; y < spritesY; ++y)
                    {
                        Rect<float> rectangle(spriteSize.v[0] * x,
                                        spriteSize.v[1] * y,
                                        spriteSize.v[0],
                                        spriteSize.v[1]);

                        animation.frames.emplace_back(filename, textureSize, rectangle, false, spriteSize, Vector<float, 2>{}, pivot);
                    }
                }

                newSpriteData.animations[""] = std::move(animation);

                spriteData[filename] = newSpriteData;
            }
        }
        else
            loadAsset(Loader::Type::sprite, filename, filename, mipmaps);
    }

    const scene::SpriteData* Bundle::getSpriteData(const std::string& name) const
    {
        const auto i = spriteData.find(name);

        if (i != spriteData.end())
            return &i->second;

        return nullptr;
    }

    void Bundle::setSpriteData(const std::string& name, const scene::SpriteData& newSpriteData)
    {
        spriteData[name] = newSpriteData;
    }

    void Bundle::releaseSpriteData()
    {
        spriteData.clear();
    }

    const scene::ParticleSystemData* Bundle::getParticleSystemData(const std::string& name) const
    {
        const auto i = particleSystemData.find(name);

        if (i != particleSystemData.end())
            return &i->second;

        return nullptr;
    }

    void Bundle::setParticleSystemData(const std::string& name, const scene::ParticleSystemData& newParticleSystemData)
    {
        particleSystemData[name] = newParticleSystemData;
    }

    void Bundle::releaseParticleSystemData()
    {
        particleSystemData.clear();
    }

    const gui::Font* Bundle::getFont(const std::string& name) const
    {
        const auto i = fonts.find(name);

        if (i != fonts.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setFont(const std::string& name, std::unique_ptr<gui::Font> font)
    {
        fonts[name] = std::move(font);
    }

    void Bundle::releaseFonts()
    {
        fonts.clear();
    }

    const audio::Cue* Bundle::getCue(const std::string& name) const
    {
        const auto i = cues.find(name);

        if (i != cues.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setCue(const std::string& name, std::unique_ptr<audio::Cue> cue)
    {
        cues[name] = std::move(cue);
    }

    void Bundle::releaseCues()
    {
        cues.clear();
    }

    const audio::Sound* Bundle::getSound(const std::string& name) const
    {
        const auto i = sounds.find(name);

        if (i != sounds.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setSound(const std::string& name, std::unique_ptr<audio::Sound> sound)
    {
        sounds[name] = std::move(sound);
    }

    void Bundle::releaseSounds()
    {
        sounds.clear();
    }

    const graphics::Material* Bundle::getMaterial(const std::string& name) const
    {
        const auto i = materials.find(name);

        if (i != materials.end())
            return i->second.get();

        return nullptr;
    }

    void Bundle::setMaterial(const std::string& name, std::unique_ptr<graphics::Material> material)
    {
        materials[name] = std::move(material);
    }

    void Bundle::releaseMaterials()
    {
        materials.clear();
    }

    const scene::SkinnedMeshData* Bundle::getSkinnedMeshData(const std::string& name) const
    {
        const auto i = skinnedMeshData.find(name);

        if (i != skinnedMeshData.end())
            return &i->second;

        return nullptr;
    }

    void Bundle::setSkinnedMeshData(const std::string& name, scene::SkinnedMeshData&& newSkinnedMeshData)
    {
        skinnedMeshData[name] = std::move(newSkinnedMeshData);
    }

    void Bundle::releaseSkinnedMeshData()
    {
        skinnedMeshData.clear();
    }

    const scene::StaticMeshData* Bundle::getStaticMeshData(const std::string& name) const
    {
        const auto i = staticMeshData.find(name);

        if (i != staticMeshData.end())
            return &i->second;

        return nullptr;
    }

    void Bundle::setStaticMeshData(const std::string& name, scene::StaticMeshData&& newStaticMeshData)
    {
        staticMeshData[name] = std::move(newStaticMeshData);
    }

    void Bundle::releaseStaticMeshData()
    {
        staticMeshData.clear();
    }
}
