// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Cache.hpp"
#include "Loader.hpp"
#include "core/Engine.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/TextureResource.hpp"
#include "graphics/ShaderResource.hpp"
#include "scene/ParticleSystemData.hpp"
#include "scene/SpriteFrame.hpp"
#include "files/FileSystem.hpp"
#include "audio/SoundDataWave.hpp"
#include "audio/SoundDataVorbis.hpp"
#include "gui/BMFont.hpp"
#include "gui/TTFont.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace assets
    {
        Cache::Cache()
        {
            addLoader(&loaderBMF);
            addLoader(&loaderCollada);
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
            {
                loader->cache = nullptr;
            }
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
            releaseModelData();
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

        bool Cache::loadAsset(const std::string& filename)
        {
            std::vector<uint8_t> data;
            if (!engine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            std::string extension = engine->getFileSystem()->getExtensionPart(filename);

            for (auto i = loaders.rbegin(); i != loaders.rend(); ++i)
            {
                Loader* loader = *i;
                if (std::find(loader->extensions.begin(), loader->extensions.end(), extension) != loader->extensions.end())
                {
                    if (loader->loadAsset(filename, data)) return true;
                }
            }

            Log(Log::Level::ERR) << "Failed to load asset " << filename;
            return false;
        }

        bool Cache::loadAssets(const std::vector<std::string>& filenames)
        {
            for (const std::string& filename : filenames)
            {
                loadAsset(filename);
            }

            return true;
        }

        bool Cache::preloadTexture(const std::string& filename, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<graphics::Texture> texture = std::make_shared<graphics::Texture>();
            if (!texture->init(filename, dynamic ? graphics::Texture::DYNAMIC : 0, mipmaps ? 0 : 1))
            {
                return false;
            }

            textures[filename] = texture;

            return true;
        }

        const std::shared_ptr<graphics::Texture>& Cache::getTexture(const std::string& filename, bool dynamic, bool mipmaps) const
        {
            auto i = textures.find(filename);

            if (i != textures.end())
            {
                return i->second;
            }
            else
            {
                std::shared_ptr<graphics::Texture> result = std::make_shared<graphics::Texture>();
                result->init(filename, dynamic ? graphics::Texture::DYNAMIC : 0, mipmaps ? 0 : 1);

                i = textures.insert(std::make_pair(filename, result)).first;

                return i->second;
            }
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
                {
                    ++i;
                }
                else
                {
                    i = textures.erase(i);
                }
            }
        }

        const std::shared_ptr<graphics::Shader>& Cache::getShader(const std::string& shaderName) const
        {
            auto i = shaders.find(shaderName);

            if (i != shaders.end())
            {
                return i->second;
            }
            else
            {
                i = shaders.insert(std::make_pair(shaderName, nullptr)).first;

                return i->second;
            }
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
                {
                    ++i;
                }
                else
                {
                    i = shaders.erase(i);
                }
            }
        }

        const std::shared_ptr<graphics::BlendState>& Cache::getBlendState(const std::string& blendStateName) const
        {
            auto i = blendStates.find(blendStateName);

            if (i != blendStates.end())
            {
                return i->second;
            }
            else
            {
                i = blendStates.insert(std::make_pair(blendStateName, nullptr)).first;

                return i->second;
            }
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
                {
                    ++i;
                }
                else
                {
                    i = blendStates.erase(i);
                }
            }
        }

        bool Cache::preloadSpriteData(const std::string& filename, bool mipmaps,
                                            uint32_t spritesX, uint32_t spritesY,
                                            const Vector2& pivot)
        {
            std::string extension = engine->getFileSystem()->getExtensionPart(filename);

            scene::SpriteData newSpriteData;

            if (extension == "json")
            {
                if (!newSpriteData.init(filename, mipmaps))
                {
                    return false;
                }
            }
            else
            {
                newSpriteData.texture = engine->getCache()->getTexture(filename, false, mipmaps);

                if (newSpriteData.texture)
                {
                    return false;
                }

                Size2 spriteSize = Size2(newSpriteData.texture->getSize().width / spritesX,
                                         newSpriteData.texture->getSize().height / spritesY);

                for (uint32_t x = 0; x < spritesX; ++x)
                {
                    for (uint32_t y = 0; y < spritesY; ++y)
                    {
                        Rectangle rectangle(spriteSize.width * x,
                                            spriteSize.height * y,
                                            spriteSize.width,
                                            spriteSize.height);

                        scene::SpriteFrame frame = scene::SpriteFrame(filename, newSpriteData.texture->getSize(), rectangle, false, spriteSize, Vector2(), pivot);
                        newSpriteData.frames.push_back(frame);
                    }
                }
            }

            spriteData[filename] = newSpriteData;

            return true;
        }

        const scene::SpriteData& Cache::getSpriteData(const std::string& filename, bool mipmaps,
                                                                  uint32_t spritesX, uint32_t spritesY,
                                                                  const Vector2& pivot) const
        {
            auto i = spriteData.find(filename);

            if (i != spriteData.end())
            {
                return i->second;
            }
            else
            {
                std::string extension = engine->getFileSystem()->getExtensionPart(filename);

                scene::SpriteData newSpriteData;

                if (extension == "json")
                {
                    newSpriteData.init(filename, mipmaps);
                }
                else if (spritesX > 0 && spritesY > 0)
                {
                    newSpriteData.texture = engine->getCache()->getTexture(filename, false, mipmaps);

                    if (newSpriteData.texture)
                    {
                        Size2 spriteSize = Size2(newSpriteData.texture->getSize().width / spritesX,
                                                 newSpriteData.texture->getSize().height / spritesY);

                        for (uint32_t x = 0; x < spritesX; ++x)
                        {
                            for (uint32_t y = 0; y < spritesY; ++y)
                            {
                                Rectangle rectangle(spriteSize.width * x,
                                                    spriteSize.height * y,
                                                    spriteSize.width,
                                                    spriteSize.height);

                                scene::SpriteFrame frame = scene::SpriteFrame(filename, newSpriteData.texture->getSize(), rectangle, false, spriteSize, Vector2(), pivot);
                                newSpriteData.frames.push_back(frame);
                            }
                        }
                    }
                }

                i = spriteData.insert(std::make_pair(filename, newSpriteData)).first;

                return i->second;
            }
        }

        void Cache::setSpriteData(const std::string& filename, const scene::SpriteData& newSpriteData)
        {
            spriteData[filename] = newSpriteData;
        }

        void Cache::releaseSpriteData()
        {
            spriteData.clear();
        }

        bool Cache::preloadParticleSystemData(const std::string& filename, bool mipmaps)
        {
            std::string extension = engine->getFileSystem()->getExtensionPart(filename);

            if (extension == "json")
            {
                scene::ParticleSystemData newParticleSystemData;
                if (!newParticleSystemData.init(filename, mipmaps))
                {
                    return false;
                }

                particleSystemData[filename] = newParticleSystemData;
            }

            return true;
        }

        const scene::ParticleSystemData& Cache::getParticleSystemData(const std::string& filename, bool mipmaps) const
        {
            auto i = particleSystemData.find(filename);

            if (i != particleSystemData.end())
            {
                return i->second;
            }
            else
            {
                scene::ParticleSystemData newParticleSystemData;

                std::string extension = engine->getFileSystem()->getExtensionPart(filename);

                if (extension == "json")
                {
                    newParticleSystemData.init(filename, mipmaps);

                    i = particleSystemData.insert(std::make_pair(filename, newParticleSystemData)).first;
                }

                return i->second;
            }
        }

        void Cache::setParticleSystemData(const std::string& filename, const scene::ParticleSystemData& newParticleSystemData)
        {
            particleSystemData[filename] = newParticleSystemData;
        }

        void Cache::releaseParticleSystemData()
        {
            particleSystemData.clear();
        }

        bool Cache::preloadFont(const std::string& filename, bool mipmaps)
        {
            auto i = fonts.find(filename);

            if (i == fonts.end())
            {
                std::string extension = engine->getFileSystem()->getExtensionPart(filename);

                if (extension == "fnt")
                {
                    std::shared_ptr<BMFont> font = std::make_shared<BMFont>();
                    if (!font->init(filename, mipmaps))
                    {
                        return false;
                    }

                    fonts[filename] = font;
                }
                else if (extension == "ttf")
                {
                    std::shared_ptr<TTFont> font = std::make_shared<TTFont>();
                    if (!font->init(filename))
                    {
                        return false;
                    }

                    fonts[filename] = font;
                }
            }

            return true;
        }

        const std::shared_ptr<Font>& Cache::getFont(const std::string& filename, bool mipmaps) const
        {
            auto i = fonts.find(filename);

            if (i != fonts.end())
            {
                return i->second;
            }
            else
            {
                std::string extension = engine->getFileSystem()->getExtensionPart(filename);

                std::shared_ptr<Font> font;

                if (extension == "fnt")
                {
                    font = std::make_shared<BMFont>(filename, mipmaps);
                }
                else if (extension == "ttf")
                {
                    font = std::make_shared<TTFont>(filename, mipmaps);
                }

                i = fonts.insert(std::make_pair(filename, font)).first;

                return i->second;
            }
        }

        void Cache::setFont(const std::string& filename, const std::shared_ptr<Font>& font)
        {
            fonts[filename] = font;
        }

        void Cache::releaseFonts()
        {
            fonts.clear();
        }

        bool Cache::preloadSoundData(const std::string& filename)
        {
            std::string extension = FileSystem::getExtensionPart(filename);

            if (extension == "wav")
            {
                std::shared_ptr<audio::SoundData> newSoundData = std::make_shared<audio::SoundDataWave>();
                if (!newSoundData->init(filename))
                {
                    return false;
                }
                soundData[filename] = newSoundData;
            }
            else if (extension == "ogg")
            {
                std::shared_ptr<audio::SoundData> newSoundData = std::make_shared<audio::SoundDataVorbis>();
                if (!newSoundData->init(filename))
                {
                    return false;
                }
                soundData[filename] = newSoundData;
            }

            return true;
        }

        const std::shared_ptr<audio::SoundData>& Cache::getSoundData(const std::string& filename) const
        {
            auto i = soundData.find(filename);

            if (i != soundData.end())
            {
                return i->second;
            }
            else
            {
                std::shared_ptr<audio::SoundData> newSoundData;

                std::string extension = FileSystem::getExtensionPart(filename);

                if (extension == "wav")
                {
                    newSoundData = std::make_shared<audio::SoundDataWave>();
                    newSoundData->init(filename);
                    soundData[filename] = newSoundData;
                }
                else if (extension == "ogg")
                {
                    newSoundData = std::make_shared<audio::SoundDataVorbis>();
                    newSoundData->init(filename);
                    soundData[filename] = newSoundData;
                }

                i = soundData.insert(std::make_pair(filename, newSoundData)).first;

                return i->second;
            }
        }

        void Cache::setSoundData(const std::string& filename, const std::shared_ptr<audio::SoundData>& newSoundData)
        {
            soundData[filename] = newSoundData;
        }

        void Cache::releaseSoundData()
        {
            soundData.clear();
        }

        bool Cache::preloadMaterial(const std::string& filename, bool mipmaps)
        {
            std::string extension = engine->getFileSystem()->getExtensionPart(filename);

            if (extension == "mtl")
            {
                std::shared_ptr<graphics::Material> material = std::make_shared<graphics::Material>();
                if (!material->init(filename, mipmaps))
                {
                    return false;
                }

                materials[filename] = material;
            }

            return true;
        }

        const std::shared_ptr<graphics::Material>& Cache::getMaterial(const std::string& filename, bool mipmaps) const
        {
            auto i = materials.find(filename);

            if (i != materials.end())
            {
                return i->second;
            }
            else
            {
                std::shared_ptr<graphics::Material> result = std::make_shared<graphics::Material>();

                std::string extension = engine->getFileSystem()->getExtensionPart(filename);

                if (extension == "mtl")
                {
                    result->init(filename, mipmaps);

                    i = materials.insert(std::make_pair(filename, result)).first;
                }

                return i->second;
            }
        }

        void Cache::setMaterial(const std::string& filename, const std::shared_ptr<graphics::Material>& material)
        {
            materials[filename] = material;
        }

        void Cache::releaseMaterials()
        {
            materials.clear();
        }

        bool Cache::preloadModelData(const std::string& filename, bool mipmaps)
        {
            scene::ModelData newModelData;
            if (!newModelData.init(filename, mipmaps))
            {
                return false;
            }

            modelData[filename] = newModelData;

            return true;
        }

        const scene::ModelData& Cache::getModelData(const std::string& filename, bool mipmaps) const
        {
            auto i = modelData.find(filename);

            if (i != modelData.end())
            {
                return i->second;
            }
            else
            {
                scene::ModelData newModelData;
                newModelData.init(filename, mipmaps);

                i = modelData.insert(std::make_pair(filename, newModelData)).first;

                return i->second;
            }
        }

        void Cache::setModelData(const std::string& filename, const scene::ModelData& newModelData)
        {
            modelData[filename] = newModelData;
        }

        void Cache::releaseModelData()
        {
            particleSystemData.clear();
        }
    } // namespace assets
} // namespace ouzel
