// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cctype>
#include <algorithm>
#include "Cache.hpp"
#include "Loader.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/TextureResource.hpp"
#include "graphics/ShaderResource.hpp"
#include "scene/ParticleSystemData.hpp"
#include "scene/SpriteData.hpp"
#include "files/FileSystem.hpp"
#include "audio/SoundDataWave.hpp"
#include "audio/SoundDataVorbis.hpp"
#include "gui/BMFont.hpp"
#include "gui/TTFont.hpp"
#include "utils/Errors.hpp"
#include "utils/JSON.hpp"

namespace ouzel
{
    namespace assets
    {
        Cache::Cache(FileSystem& initFileSystem):
            fileSystem(initFileSystem)
        {
            addLoader(&loaderBMF);
            addLoader(&loaderCollada);
            addLoader(&loaderGLTF);
            addLoader(&loaderImage);
            addLoader(&loaderMTL);
            addLoader(&loaderOBJ);
            addLoader(&loaderParticleSystem);
            addLoader(&loaderSprite);
            addLoader(&loaderTTF);
            addLoader(&loaderVorbis);
            addLoader(&loaderWave);
        }

        Cache::~Cache()
        {
            for (Loader* loader : loaders)
                loader->cache = nullptr;
        }

        void Cache::clear()
        {
            releaseTextures();
            releaseShaders();
            releaseParticleSystemData();
            releaseBlendStates();
            releaseSpriteData();
            releaseFonts();
            releaseMaterials();
            releaseMeshData();
        }

        void Cache::addLoader(Loader* loader)
        {
            auto i = std::find(loaders.begin(), loaders.end(), loader);
            if (i == loaders.end())
            {
                if (loader->cache) loader->cache->removeLoader(loader);
                loader->cache = this;
                loaders.push_back(loader);
            }
        }

        void Cache::removeLoader(Loader* loader)
        {
            auto i = std::find(loaders.begin(), loaders.end(), loader);
            if (i != loaders.end())
            {
                loader->cache = nullptr;
                loaders.erase(i);
            }
        }

        void Cache::loadAsset(uint32_t loaderType, const std::string& filename, bool mipmaps)
        {
            std::vector<uint8_t> data = fileSystem.readFile(filename);

            std::string extension = fileSystem.getExtensionPart(filename);
            std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c){ return std::tolower(c); });

            for (auto i = loaders.rbegin(); i != loaders.rend(); ++i)
            {
                Loader* loader = *i;
                if (loader->getType() == loaderType)
                {
                    if (loader->loadAsset(filename, data, mipmaps)) return;
                }
            }

            throw FileError("Failed to load asset " + filename);
        }

        void Cache::loadAssets(const std::string filename)
        {
            json::Data data(fileSystem.readFile(filename));
            // TODO: parse data
        }

        std::shared_ptr<graphics::Texture> Cache::getTexture(const std::string& filename) const
        {
            auto i = textures.find(filename);

            if (i != textures.end())
                return i->second;

            return nullptr;
        }

        void Cache::setTexture(const std::string& filename, const std::shared_ptr<graphics::Texture>& texture)
        {
            textures[filename] = texture;
        }

        void Cache::releaseTextures()
        {
            for (auto i = textures.begin(); i != textures.end();)
            {
                // don't delete white pixel texture
                if (i->first == graphics::TEXTURE_WHITE_PIXEL)
                    ++i;
                else
                    i = textures.erase(i);
            }
        }

        std::shared_ptr<graphics::Shader> Cache::getShader(const std::string& shaderName) const
        {
            auto i = shaders.find(shaderName);

            if (i != shaders.end())
                return i->second;

            return nullptr;
        }

        void Cache::setShader(const std::string& shaderName, const std::shared_ptr<graphics::Shader>& shader)
        {
            shaders[shaderName] = shader;
        }

        void Cache::releaseShaders()
        {
            for (auto i = shaders.begin(); i != shaders.end();)
            {
                // don't delete default shaders
                if (i->first == graphics::SHADER_COLOR ||
                    i->first == graphics::SHADER_TEXTURE)
                    ++i;
                else
                    i = shaders.erase(i);
            }
        }

        std::shared_ptr<graphics::BlendState> Cache::getBlendState(const std::string& blendStateName) const
        {
            auto i = blendStates.find(blendStateName);

            if (i != blendStates.end())
                return i->second;

            return nullptr;
        }

        void Cache::setBlendState(const std::string& blendStateName, const std::shared_ptr<graphics::BlendState>& blendState)
        {
            blendStates[blendStateName] = blendState;
        }

        void Cache::releaseBlendStates()
        {
            for (auto i = blendStates.begin(); i != blendStates.end();)
            {
                // don't delete default blend states
                if (i->first == graphics::BLEND_NO_BLEND ||
                    i->first == graphics::BLEND_ADD ||
                    i->first == graphics::BLEND_MULTIPLY ||
                    i->first == graphics::BLEND_ALPHA)
                    ++i;
                else
                    i = blendStates.erase(i);
            }
        }

        void Cache::preloadSpriteData(const std::string& filename, bool mipmaps,
                                      uint32_t spritesX, uint32_t spritesY,
                                      const Vector2& pivot)
        {
            std::vector<std::string> imageExtensions = {"jpg", "jpeg", "png", "bmp", "tga"};

            if (std::find(imageExtensions.begin(), imageExtensions.end(),
                          fileSystem.getExtensionPart(filename)) != imageExtensions.end())
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

        const scene::SpriteData* Cache::getSpriteData(const std::string& filename) const
        {
            auto i = spriteData.find(filename);

            if (i != spriteData.end())
                return &i->second;

            return nullptr;
        }

        void Cache::setSpriteData(const std::string& filename, const scene::SpriteData& newSpriteData)
        {
            spriteData[filename] = newSpriteData;
        }

        void Cache::releaseSpriteData()
        {
            spriteData.clear();
        }

        const scene::ParticleSystemData* Cache::getParticleSystemData(const std::string& filename) const
        {
            auto i = particleSystemData.find(filename);

            if (i != particleSystemData.end())
                return &i->second;

            return nullptr;
        }

        void Cache::setParticleSystemData(const std::string& filename, const scene::ParticleSystemData& newParticleSystemData)
        {
            particleSystemData[filename] = newParticleSystemData;
        }

        void Cache::releaseParticleSystemData()
        {
            particleSystemData.clear();
        }

        std::shared_ptr<Font> Cache::getFont(const std::string& filename) const
        {
            auto i = fonts.find(filename);

            if (i != fonts.end())
                return i->second;

            return nullptr;
        }

        void Cache::setFont(const std::string& filename, const std::shared_ptr<Font>& font)
        {
            fonts[filename] = font;
        }

        void Cache::releaseFonts()
        {
            fonts.clear();
        }

        std::shared_ptr<audio::SoundData> Cache::getSoundData(const std::string& filename) const
        {
            auto i = soundData.find(filename);

            if (i != soundData.end())
                return i->second;

            return nullptr;
        }

        void Cache::setSoundData(const std::string& filename, const std::shared_ptr<audio::SoundData>& newSoundData)
        {
            soundData[filename] = newSoundData;
        }

        void Cache::releaseSoundData()
        {
            soundData.clear();
        }

        std::shared_ptr<graphics::Material> Cache::getMaterial(const std::string& filename) const
        {
            auto i = materials.find(filename);

            if (i != materials.end())
                return i->second;

            return nullptr;
        }

        void Cache::setMaterial(const std::string& filename, const std::shared_ptr<graphics::Material>& material)
        {
            materials[filename] = material;
        }

        void Cache::releaseMaterials()
        {
            materials.clear();
        }

        const scene::MeshData* Cache::getMeshData(const std::string& filename) const
        {
            auto i = meshData.find(filename);

            if (i != meshData.end())
                return &i->second;

            return nullptr;
        }

        void Cache::setMeshData(const std::string& filename, const scene::MeshData& newMeshData)
        {
            meshData[filename] = newMeshData;
        }

        void Cache::releaseMeshData()
        {
            particleSystemData.clear();
        }
    } // namespace assets
} // namespace ouzel
