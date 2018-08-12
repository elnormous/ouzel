// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Bundle.hpp"
#include "Cache.hpp"
#include "Loader.hpp"
#include "utils/Errors.hpp"
#include "utils/JSON.hpp"

namespace ouzel
{
    namespace assets
    {
        Bundle::Bundle(Cache& initCache):
            cache(initCache)
        {
            cache.addBundle(this);
        }

        Bundle::~Bundle()
        {
            cache.removeBundle(this);
        }

        void Bundle::loadAsset(uint32_t loaderType, const std::string& filename, bool mipmaps)
        {
            std::vector<uint8_t> data = cache.getFileSystem()->readFile(filename);

            std::string extension = FileSystem::getExtensionPart(filename);
            std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c){ return std::tolower(c); });

            auto loaders = cache.getLoaders();

            for (auto i = loaders.rbegin(); i != loaders.rend(); ++i)
            {
                Loader* loader = *i;
                if (loader->getType() == loaderType)
                {
                    if (loader->loadAsset(*this, filename, data, mipmaps)) return;
                }
            }

            throw FileError("Failed to load asset " + filename);
        }

        void Bundle::loadAssets(const std::string& filename)
        {
            json::Data data(cache.getFileSystem()->readFile(filename));

            for (const json::Value& asset : data["assets"].asArray())
            {
                bool mipmaps = asset.hasMember("mipmaps") ? asset["mipmaps"].asBool() : true;
                loadAsset(asset["type"].asUInt32(), asset["filename"].asString(), mipmaps);
            }
        }

        std::shared_ptr<graphics::Texture> Bundle::getTexture(const std::string& filename) const
        {
            auto i = textures.find(filename);

            if (i != textures.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setTexture(const std::string& filename, const std::shared_ptr<graphics::Texture>& texture)
        {
            textures[filename] = texture;
        }

        void Bundle::releaseTextures()
        {
            for (auto i = textures.begin(); i != textures.end();)
                i = textures.erase(i);
        }

        std::shared_ptr<graphics::Shader> Bundle::getShader(const std::string& shaderName) const
        {
            auto i = shaders.find(shaderName);

            if (i != shaders.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setShader(const std::string& shaderName, const std::shared_ptr<graphics::Shader>& shader)
        {
            shaders[shaderName] = shader;
        }

        void Bundle::releaseShaders()
        {
            for (auto i = shaders.begin(); i != shaders.end();)
                i = shaders.erase(i);
        }

        std::shared_ptr<graphics::BlendState> Bundle::getBlendState(const std::string& blendStateName) const
        {
            auto i = blendStates.find(blendStateName);

            if (i != blendStates.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setBlendState(const std::string& blendStateName, const std::shared_ptr<graphics::BlendState>& blendState)
        {
            blendStates[blendStateName] = blendState;
        }

        void Bundle::releaseBlendStates()
        {
            for (auto i = blendStates.begin(); i != blendStates.end();)
                i = blendStates.erase(i);
        }

        void Bundle::preloadSpriteData(const std::string& filename, bool mipmaps,
                                      uint32_t spritesX, uint32_t spritesY,
                                      const Vector2& pivot)
        {
            std::vector<std::string> imageExtensions = {"jpg", "jpeg", "png", "bmp", "tga"};

            if (std::find(imageExtensions.begin(), imageExtensions.end(),
                          FileSystem::getExtensionPart(filename)) != imageExtensions.end())
            {
                scene::SpriteData newSpriteData;

                if (spritesX == 0) spritesX = 1;
                if (spritesY == 0) spritesY = 1;

                newSpriteData.texture = getTexture(filename);

                if (newSpriteData.texture)
                {
                    Size2 spriteSize = Size2(newSpriteData.texture->getSize().width / spritesX,
                                             newSpriteData.texture->getSize().height / spritesY);

                    scene::SpriteData::Animation animation;
                    animation.frames.reserve(spritesX * spritesY);

                    for (uint32_t x = 0; x < spritesX; ++x)
                    {
                        for (uint32_t y = 0; y < spritesY; ++y)
                        {
                            Rect rectangle(spriteSize.width * x,
                                           spriteSize.height * y,
                                           spriteSize.width,
                                           spriteSize.height);

                            scene::SpriteData::Frame frame = scene::SpriteData::Frame(filename, newSpriteData.texture->getSize(), rectangle, false, spriteSize, Vector2(), pivot);
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

        std::shared_ptr<Font> Bundle::getFont(const std::string& filename) const
        {
            auto i = fonts.find(filename);

            if (i != fonts.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setFont(const std::string& filename, const std::shared_ptr<Font>& font)
        {
            fonts[filename] = font;
        }

        void Bundle::releaseFonts()
        {
            fonts.clear();
        }

        std::shared_ptr<audio::SoundData> Bundle::getSoundData(const std::string& filename) const
        {
            auto i = soundData.find(filename);

            if (i != soundData.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setSoundData(const std::string& filename, const std::shared_ptr<audio::SoundData>& newSoundData)
        {
            soundData[filename] = newSoundData;
        }

        void Bundle::releaseSoundData()
        {
            soundData.clear();
        }

        std::shared_ptr<graphics::Material> Bundle::getMaterial(const std::string& filename) const
        {
            auto i = materials.find(filename);

            if (i != materials.end())
                return i->second;

            return nullptr;
        }

        void Bundle::setMaterial(const std::string& filename, const std::shared_ptr<graphics::Material>& material)
        {
            materials[filename] = material;
        }

        void Bundle::releaseMaterials()
        {
            materials.clear();
        }

        const scene::MeshData* Bundle::getMeshData(const std::string& filename) const
        {
            auto i = meshData.find(filename);

            if (i != meshData.end())
                return &i->second;

            return nullptr;
        }

        void Bundle::setMeshData(const std::string& filename, const scene::MeshData& newMeshData)
        {
            meshData[filename] = newMeshData;
        }

        void Bundle::releaseMeshData()
        {
            particleSystemData.clear();
        }
    } // namespace assets
} // namespace ouzel
