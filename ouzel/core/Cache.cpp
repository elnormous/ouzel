// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Cache.hpp"
#include "Engine.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/TextureResource.hpp"
#include "graphics/ShaderResource.hpp"
#include "scene/ParticleDefinition.hpp"
#include "scene/SpriteFrame.hpp"
#include "files/FileSystem.hpp"
#include "audio/SoundDataWave.hpp"
#include "audio/SoundDataVorbis.hpp"
#include "gui/BMFont.hpp"
#include "gui/TTFont.hpp"

namespace ouzel
{
    Cache::Cache()
    {
    }

    void Cache::clear()
    {
        releaseTextures();
        releaseShaders();
        releaseParticleDefinitions();
        releaseBlendStates();
        releaseSpriteDefinitions();
        releaseFonts();
        releaseMaterials();
        releaseModelDefinitions();
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

    bool Cache::preloadSpriteDefinition(const std::string& filename, bool mipmaps,
                                        uint32_t spritesX, uint32_t spritesY,
                                        const Vector2& pivot)
    {
        std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

        scene::SpriteDefinition spriteDefinition;

        if (extension == "json")
        {
            if (!spriteDefinition.load(filename, mipmaps))
            {
                return false;
            }
        }
        else
        {
            spriteDefinition.texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

            if (spriteDefinition.texture)
            {
                return false;
            }

            Size2 spriteSize = Size2(spriteDefinition.texture->getSize().width / spritesX,
                                     spriteDefinition.texture->getSize().height / spritesY);

            for (uint32_t x = 0; x < spritesX; ++x)
            {
                for (uint32_t y = 0; y < spritesY; ++y)
                {
                    Rectangle rectangle(spriteSize.width * x,
                                        spriteSize.height * y,
                                        spriteSize.width,
                                        spriteSize.height);

                    scene::SpriteFrame frame = scene::SpriteFrame(filename, spriteDefinition.texture->getSize(), rectangle, false, spriteSize, Vector2(), pivot);
                    spriteDefinition.frames.push_back(frame);
                }
            }
        }

        spriteDefinitions[filename] = spriteDefinition;

        return true;
    }

    const scene::SpriteDefinition& Cache::getSpriteDefinition(const std::string& filename, bool mipmaps,
                                                              uint32_t spritesX, uint32_t spritesY,
                                                              const Vector2& pivot) const
    {
        auto i = spriteDefinitions.find(filename);

        if (i != spriteDefinitions.end())
        {
            return i->second;
        }
        else
        {
            std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

            scene::SpriteDefinition spriteDefinition;

            if (extension == "json")
            {
                spriteDefinition.load(filename, mipmaps);
            }
            else if (spritesX > 0 && spritesY > 0)
            {
                spriteDefinition.texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

                if (spriteDefinition.texture)
                {
                    Size2 spriteSize = Size2(spriteDefinition.texture->getSize().width / spritesX,
                                             spriteDefinition.texture->getSize().height / spritesY);

                    for (uint32_t x = 0; x < spritesX; ++x)
                    {
                        for (uint32_t y = 0; y < spritesY; ++y)
                        {
                            Rectangle rectangle(spriteSize.width * x,
                                                spriteSize.height * y,
                                                spriteSize.width,
                                                spriteSize.height);

                            scene::SpriteFrame frame = scene::SpriteFrame(filename, spriteDefinition.texture->getSize(), rectangle, false, spriteSize, Vector2(), pivot);
                            spriteDefinition.frames.push_back(frame);
                        }
                    }
                }
            }

            i = spriteDefinitions.insert(std::make_pair(filename, spriteDefinition)).first;

            return i->second;
        }
    }

    void Cache::setSpriteDefinition(const std::string& filename, const scene::SpriteDefinition& spriteDefinition)
    {
        spriteDefinitions[filename] = spriteDefinition;
    }

    void Cache::releaseSpriteDefinitions()
    {
        spriteDefinitions.clear();
    }

    bool Cache::preloadParticleDefinition(const std::string& filename, bool mipmaps)
    {
        std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

        if (extension == "json")
        {
            scene::ParticleDefinition particleDefinition;
            if (!particleDefinition.load(filename, mipmaps))
            {
                return false;
            }

            particleDefinitions[filename] = particleDefinition;
        }

        return true;
    }

    const scene::ParticleDefinition& Cache::getParticleDefinition(const std::string& filename, bool mipmaps) const
    {
        auto i = particleDefinitions.find(filename);

        if (i != particleDefinitions.end())
        {
            return i->second;
        }
        else
        {
            scene::ParticleDefinition particleDefinition;

            std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

            if (extension == "json")
            {
                particleDefinition.load(filename, mipmaps);

                i = particleDefinitions.insert(std::make_pair(filename, particleDefinition)).first;
            }

            return i->second;
        }
    }

    void Cache::setParticleDefinition(const std::string& filename, const scene::ParticleDefinition& particleDefinition)
    {
        particleDefinitions[filename] = particleDefinition;
    }

    void Cache::releaseParticleDefinitions()
    {
        particleDefinitions.clear();
    }

    bool Cache::preloadFont(const std::string& filename, bool mipmaps)
    {
        auto i = fonts.find(filename);

        if (i == fonts.end())
        {
            std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

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
            std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

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
            std::shared_ptr<audio::SoundDataWave> newSoundData = std::make_shared<audio::SoundDataWave>();
            if (!newSoundData->init(filename))
            {
                return false;
            }
            soundData[filename] = newSoundData;
        }
        else if (extension == "ogg")
        {
            std::shared_ptr<audio::SoundDataVorbis> newSoundData = std::make_shared<audio::SoundDataVorbis>();
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
        std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

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

            std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

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

    bool Cache::preloadModelDefinition(const std::string& filename, bool mipmaps)
    {
        scene::ModelDefinition modelDefinition;
        if (!modelDefinition.load(filename, mipmaps))
        {
            return false;
        }

        modelDefinitions[filename] = modelDefinition;

        return true;
    }

    const scene::ModelDefinition& Cache::getModelDefinition(const std::string& filename, bool mipmaps) const
    {
        auto i = modelDefinitions.find(filename);

        if (i != modelDefinitions.end())
        {
            return i->second;
        }
        else
        {
            scene::ModelDefinition modelDefinition;
            modelDefinition.load(filename, mipmaps);

            i = modelDefinitions.insert(std::make_pair(filename, modelDefinition)).first;

            return i->second;
        }
    }

    void Cache::setModelDefinition(const std::string& filename, const scene::ModelDefinition& modelDefinition)
    {
        modelDefinitions[filename] = modelDefinition;
    }

    void Cache::releaseModelDefinitions()
    {
        particleDefinitions.clear();
    }
}
