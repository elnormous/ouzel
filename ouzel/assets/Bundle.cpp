// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <cctype>
#include <stdexcept>
#include "Bundle.hpp"
#include "Cache.hpp"
#include "Loader.hpp"
#include "utils/Json.hpp"

namespace ouzel
{
    namespace assets
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

        void Bundle::loadAsset(uint32_t loaderType, const std::string& name,
                               const std::string& filename, bool mipmaps)
        {
            std::vector<uint8_t> data = fileSystem.readFile(filename);

            auto loaders = cache.getLoaders();

            for (auto i = loaders.rbegin(); i != loaders.rend(); ++i)
            {
                Loader* loader = *i;
                if (loader->getType() == loaderType &&
                    loader->loadAsset(*this, name, data, mipmaps))
                    return;
            }

            throw std::runtime_error("Failed to load asset " + filename);
        }

        void Bundle::loadAssets(const std::string& filename)
        {
            json::Data data(fileSystem.readFile(filename));

            for (const json::Value& asset : data["assets"].as<json::Value::Array>())
            {
                std::string file = asset["filename"].as<std::string>();
                std::string name = asset.hasMember("name") ? asset["name"].as<std::string>() : file;
                bool mipmaps = asset.hasMember("mipmaps") ? asset["mipmaps"].as<bool>() : true;
                loadAsset(asset["type"].as<uint32_t>(), name, file, mipmaps);
            }
        }

        void Bundle::loadAssets(const std::vector<Asset>& assets)
        {
            for (const Asset& asset : assets)
                loadAsset(asset.type, asset.name, asset.filename, asset.mipmaps);
        }

        std::shared_ptr<graphics::Texture> Bundle::getTexture(const std::string& name) const
        {
            auto i = textures.find(name);

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

        std::shared_ptr<graphics::Shader> Bundle::getShader(const std::string& name) const
        {
            auto i = shaders.find(name);

            if (i != shaders.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setShader(const std::string& name, const std::shared_ptr<graphics::Shader>& shader)
        {
            shaders[name] = shader;
        }

        void Bundle::releaseShaders()
        {
            shaders.clear();
        }

        std::shared_ptr<graphics::BlendState> Bundle::getBlendState(const std::string& name) const
        {
            auto i = blendStates.find(name);

            if (i != blendStates.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setBlendState(const std::string& name, const std::shared_ptr<graphics::BlendState>& blendState)
        {
            blendStates[name] = blendState;
        }

        void Bundle::releaseBlendStates()
        {
            blendStates.clear();
        }

        std::shared_ptr<graphics::DepthStencilState> Bundle::getDepthStencilState(const std::string& name) const
        {
            auto i = depthStencilStates.find(name);

            if (i != depthStencilStates.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setDepthStencilState(const std::string& name, const std::shared_ptr<graphics::DepthStencilState>& depthStencilState)
        {
            depthStencilStates[name] = depthStencilState;
        }

        void Bundle::releaseDepthStencilStates()
        {
            depthStencilStates.clear();
        }

        void Bundle::preloadSpriteData(const std::string& filename, bool mipmaps,
                                      uint32_t spritesX, uint32_t spritesY,
                                      const Vector2F& pivot)
        {
            std::string extension = storage::FileSystem::getExtensionPart(filename);
            std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c){ return std::tolower(c); });
            std::vector<std::string> imageExtensions = {"jpg", "jpeg", "png", "bmp", "tga"};

            if (std::find(imageExtensions.begin(), imageExtensions.end(),
                          extension) != imageExtensions.end())
            {
                scene::SpriteData newSpriteData;

                if (spritesX == 0) spritesX = 1;
                if (spritesY == 0) spritesY = 1;

                newSpriteData.texture = getTexture(filename);

                const Size2F textureSize(static_cast<float>(newSpriteData.texture->getSize().v[0]),
                                         static_cast<float>(newSpriteData.texture->getSize().v[1]));

                if (newSpriteData.texture)
                {
                    Size2F spriteSize = Size2F(textureSize.v[0] / spritesX,
                                               textureSize.v[1] / spritesY);

                    scene::SpriteData::Animation animation;
                    animation.frames.reserve(spritesX * spritesY);

                    for (uint32_t x = 0; x < spritesX; ++x)
                    {
                        for (uint32_t y = 0; y < spritesY; ++y)
                        {
                            RectF rectangle(spriteSize.v[0] * x,
                                            spriteSize.v[1] * y,
                                            spriteSize.v[0],
                                            spriteSize.v[1]);

                            scene::SpriteData::Frame frame = scene::SpriteData::Frame(filename, textureSize, rectangle, false, spriteSize, Vector2F(), pivot);
                            animation.frames.push_back(frame);
                        }
                    }

                    newSpriteData.animations[""] = std::move(animation);

                    spriteData[filename] = newSpriteData;
                }
            }
            else
                loadAsset(Loader::SPRITE, filename, filename, mipmaps);
        }

        const scene::SpriteData* Bundle::getSpriteData(const std::string& name) const
        {
            auto i = spriteData.find(name);

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
            auto i = particleSystemData.find(name);

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

        std::shared_ptr<gui::Font> Bundle::getFont(const std::string& name) const
        {
            auto i = fonts.find(name);

            if (i != fonts.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setFont(const std::string& name, const std::shared_ptr<gui::Font>& font)
        {
            fonts[name] = font;
        }

        void Bundle::releaseFonts()
        {
            fonts.clear();
        }

        std::shared_ptr<audio::Sound> Bundle::getSound(const std::string& name) const
        {
            auto i = sounds.find(name);

            if (i != sounds.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setSound(const std::string& name, const std::shared_ptr<audio::Sound>& newSound)
        {
            sounds[name] = newSound;
        }

        void Bundle::releaseSound()
        {
            sounds.clear();
        }

        std::shared_ptr<graphics::Material> Bundle::getMaterial(const std::string& name) const
        {
            auto i = materials.find(name);

            if (i != materials.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setMaterial(const std::string& name, const std::shared_ptr<graphics::Material>& material)
        {
            materials[name] = material;
        }

        void Bundle::releaseMaterials()
        {
            materials.clear();
        }

        const scene::SkinnedMeshData* Bundle::getSkinnedMeshData(const std::string& name) const
        {
            auto i = skinnedMeshData.find(name);

            if (i != skinnedMeshData.end())
                return &i->second;

            return nullptr;
        }

        void Bundle::setSkinnedMeshData(const std::string& name, const scene::SkinnedMeshData& newSkinnedMeshData)
        {
            skinnedMeshData[name] = newSkinnedMeshData;
        }

        void Bundle::releaseSkinnedMeshData()
        {
            skinnedMeshData.clear();
        }

        const scene::StaticMeshData* Bundle::getStaticMeshData(const std::string& name) const
        {
            auto i = staticMeshData.find(name);

            if (i != staticMeshData.end())
                return &i->second;

            return nullptr;
        }

        void Bundle::setStaticMeshData(const std::string& name, const scene::StaticMeshData& newStaticMeshData)
        {
            staticMeshData[name] = newStaticMeshData;
        }

        void Bundle::releaseStaticMeshData()
        {
            staticMeshData.clear();
        }
    } // namespace assets
} // namespace ouzel
