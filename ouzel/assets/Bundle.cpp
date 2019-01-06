// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cctype>
#include <stdexcept>
#include "Bundle.hpp"
#include "Cache.hpp"
#include "Loader.hpp"
#include "utils/JSON.hpp"

namespace ouzel
{
    namespace assets
    {
        Bundle::Bundle(Cache& initCache, FileSystem& initFileSystem):
            cache(initCache), fileSystem(initFileSystem)
        {
            cache.addBundle(this);
        }

        Bundle::~Bundle()
        {
            cache.removeBundle(this);
        }

        void Bundle::loadAsset(uint32_t loaderType, const std::string& filename, bool mipmaps)
        {
            std::vector<uint8_t> data = fileSystem.readFile(filename);

            auto loaders = cache.getLoaders();

            for (auto i = loaders.rbegin(); i != loaders.rend(); ++i)
            {
                Loader* loader = *i;
                if (loader->getType() == loaderType &&
                    loader->loadAsset(*this, filename, data, mipmaps))
                    return;
            }

            throw std::runtime_error("Failed to load asset " + filename);
        }

        void Bundle::loadAssets(const std::string& filename)
        {
            json::Data data(fileSystem.readFile(filename));

            for (const json::Value& asset : data["assets"].as<json::Value::Array>())
            {
                bool mipmaps = asset.hasMember("mipmaps") ? asset["mipmaps"].as<bool>() : true;
                loadAsset(asset["type"].as<uint32_t>(), asset["filename"].as<std::string>(), mipmaps);
            }
        }

        void Bundle::loadAssets(const std::vector<Asset>& assets)
        {
            for (const Asset& asset : assets)
                loadAsset(asset.type, asset.filename, asset.mipmaps);
        }

        graphics::Texture* Bundle::getTexture(const std::string& filename) const
        {
            auto i = textures.find(filename);

            if (i != textures.end())
                return i->second.get();

            return nullptr;
        }

        void Bundle::setTexture(const std::string& filename, std::unique_ptr<graphics::Texture>&& texture)
        {
            textures[filename] = std::move(texture);
        }

        void Bundle::releaseTextures()
        {
            textures.clear();
        }

        graphics::Shader* Bundle::getShader(const std::string& shaderName) const
        {
            auto i = shaders.find(shaderName);

            if (i != shaders.end())
                return i->second.get();

            return nullptr;
        }

        void Bundle::setShader(const std::string& shaderName, std::unique_ptr<graphics::Shader>&& shader)
        {
            shaders[shaderName] = std::move(shader);
        }

        void Bundle::releaseShaders()
        {
            shaders.clear();
        }

        graphics::BlendState* Bundle::getBlendState(const std::string& blendStateName) const
        {
            auto i = blendStates.find(blendStateName);

            if (i != blendStates.end())
                return i->second.get();

            return nullptr;
        }

        void Bundle::setBlendState(const std::string& blendStateName, std::unique_ptr<graphics::BlendState>&& blendState)
        {
            blendStates[blendStateName] = std::move(blendState);
        }

        void Bundle::releaseBlendStates()
        {
            blendStates.clear();
        }

        graphics::DepthStencilState* Bundle::getDepthStencilState(const std::string& depthStencilStateName) const
        {
            auto i = depthStencilStates.find(depthStencilStateName);

            if (i != depthStencilStates.end())
                return i->second.get();

            return nullptr;
        }

        void Bundle::setDepthStencilState(const std::string& depthStencilStateName, std::unique_ptr<graphics::DepthStencilState>&& depthStencilState)
        {
            depthStencilStates[depthStencilStateName] = std::move(depthStencilState);
        }

        void Bundle::releaseDepthStencilStates()
        {
            depthStencilStates.clear();
        }

        void Bundle::preloadSpriteData(const std::string& filename, bool mipmaps,
                                      uint32_t spritesX, uint32_t spritesY,
                                      const Vector2<float>& pivot)
        {
            std::string extension = FileSystem::getExtensionPart(filename);
            std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c){ return std::tolower(c); });
            std::vector<std::string> imageExtensions = {"jpg", "jpeg", "png", "bmp", "tga"};

            if (std::find(imageExtensions.begin(), imageExtensions.end(),
                          extension) != imageExtensions.end())
            {
                scene::SpriteData newSpriteData;

                if (spritesX == 0) spritesX = 1;
                if (spritesY == 0) spritesY = 1;

                newSpriteData.texture = getTexture(filename);

                const Size2<float> textureSize(static_cast<float>(newSpriteData.texture->getSize().v[0]),
                                               static_cast<float>(newSpriteData.texture->getSize().v[1]));

                if (newSpriteData.texture)
                {
                    Size2<float> spriteSize = Size2<float>(textureSize.v[0] / spritesX,
                                                           textureSize.v[1] / spritesY);

                    scene::SpriteData::Animation animation;
                    animation.frames.reserve(spritesX * spritesY);

                    for (uint32_t x = 0; x < spritesX; ++x)
                    {
                        for (uint32_t y = 0; y < spritesY; ++y)
                        {
                            Rect<float> rectangle(spriteSize.v[0] * x,
                                                  spriteSize.v[1] * y,
                                                  spriteSize.v[0],
                                                  spriteSize.v[1]);

                            scene::SpriteData::Frame frame = scene::SpriteData::Frame(filename, textureSize, rectangle, false, spriteSize, Vector2<float>(), pivot);
                            animation.frames.push_back(frame);
                        }
                    }

                    newSpriteData.animations[""] = std::move(animation);

                    spriteData[filename] = newSpriteData;
                }
            }
            else
                loadAsset(Loader::SPRITE, filename, mipmaps);
        }

        const scene::SpriteData* Bundle::getSpriteData(const std::string& filename) const
        {
            auto i = spriteData.find(filename);

            if (i != spriteData.end())
                return &i->second;

            return nullptr;
        }

        void Bundle::setSpriteData(const std::string& filename, const scene::SpriteData& newSpriteData)
        {
            spriteData[filename] = newSpriteData;
        }

        void Bundle::releaseSpriteData()
        {
            spriteData.clear();
        }

        const scene::ParticleSystemData* Bundle::getParticleSystemData(const std::string& filename) const
        {
            auto i = particleSystemData.find(filename);

            if (i != particleSystemData.end())
                return &i->second;

            return nullptr;
        }

        void Bundle::setParticleSystemData(const std::string& filename, const scene::ParticleSystemData& newParticleSystemData)
        {
            particleSystemData[filename] = newParticleSystemData;
        }

        void Bundle::releaseParticleSystemData()
        {
            particleSystemData.clear();
        }

        Font* Bundle::getFont(const std::string& filename) const
        {
            auto i = fonts.find(filename);

            if (i != fonts.end())
                return i->second.get();

            return nullptr;
        }

        void Bundle::setFont(const std::string& filename, std::unique_ptr<Font>&& font)
        {
            fonts[filename] = std::move(font);
        }

        void Bundle::releaseFonts()
        {
            fonts.clear();
        }

        audio::Sound* Bundle::getSound(const std::string& filename) const
        {
            auto i = sounds.find(filename);

            if (i != sounds.end())
                return i->second.get();

            return nullptr;
        }

        void Bundle::setSound(const std::string& filename, std::unique_ptr<audio::Sound>&& newSound)
        {
            sounds[filename] = std::move(newSound);
        }

        void Bundle::releaseSound()
        {
            sounds.clear();
        }

        graphics::Material* Bundle::getMaterial(const std::string& filename) const
        {
            auto i = materials.find(filename);

            if (i != materials.end())
                return i->second.get();

            return nullptr;
        }

        void Bundle::setMaterial(const std::string& filename, std::unique_ptr<graphics::Material>&& material)
        {
            materials[filename] = std::move(material);
        }

        void Bundle::releaseMaterials()
        {
            materials.clear();
        }

        const scene::SkinnedMeshData* Bundle::getSkinnedMeshData(const std::string& filename) const
        {
            auto i = skinnedMeshData.find(filename);

            if (i != skinnedMeshData.end())
                return &i->second;

            return nullptr;
        }

        void Bundle::setSkinnedMeshData(const std::string& filename, const scene::SkinnedMeshData& newSkinnedMeshData)
        {
            skinnedMeshData[filename] = newSkinnedMeshData;
        }

        void Bundle::releaseSkinnedMeshData()
        {
            skinnedMeshData.clear();
        }

        const scene::StaticMeshData* Bundle::getStaticMeshData(const std::string& filename) const
        {
            auto i = staticMeshData.find(filename);

            if (i != staticMeshData.end())
                return &i->second;

            return nullptr;
        }

        void Bundle::setStaticMeshData(const std::string& filename, const scene::StaticMeshData& newStaticMeshData)
        {
            staticMeshData[filename] = newStaticMeshData;
        }

        void Bundle::releaseStaticMeshData()
        {
            staticMeshData.clear();
        }
    } // namespace assets
} // namespace ouzel
