// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "Cache.h"
#include "Engine.h"
#include "Application.h"
#include "graphics/Renderer.h"
#include "graphics/TextureResource.h"
#include "graphics/ShaderResource.h"
#include "scene/ParticleDefinition.h"
#include "scene/SpriteFrame.h"
#include "files/FileSystem.h"
#include "utils/Utils.h"

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
        releaseSpriteFrames();
        releaseBMFonts();
    }

    void Cache::preloadTexture(const std::string& filename, bool dynamic, bool mipmaps)
    {
        std::unordered_map<std::string, std::shared_ptr<graphics::Texture>>::const_iterator i = textures.find(filename);

        if (i == textures.end())
        {
            std::shared_ptr<graphics::Texture> texture = std::make_shared<graphics::Texture>();
            texture->initFromFile(filename, dynamic, mipmaps);

            textures[filename] = texture;
        }
    }

    const std::shared_ptr<graphics::Texture>& Cache::getTexture(const std::string& filename, bool dynamic, bool mipmaps) const
    {
        std::unordered_map<std::string, std::shared_ptr<graphics::Texture>>::const_iterator i = textures.find(filename);

        if (i != textures.end())
        {
            return i->second;
        }
        else
        {
            std::shared_ptr<graphics::Texture> result = std::make_shared<graphics::Texture>();
            result->initFromFile(filename, dynamic, mipmaps);

            auto i = textures.insert(std::make_pair(filename, result));

            return i.first->second;
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
        std::unordered_map<std::string, std::shared_ptr<graphics::Shader>>::const_iterator i = shaders.find(shaderName);

        if (i != shaders.end())
        {
            return i->second;
        }
        else
        {
            auto i = shaders.insert(std::make_pair(shaderName, nullptr));

            return i.first->second;
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
        std::unordered_map<std::string, std::shared_ptr<graphics::BlendState>>::const_iterator i = blendStates.find(blendStateName);

        if (i != blendStates.end())
        {
            return i->second;
        }
        else
        {
            auto i = blendStates.insert(std::make_pair(blendStateName, nullptr));

            return i.first->second;
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

    void Cache::preloadSpriteFrames(const std::string& filename, bool mipmaps)
    {
        std::string extension = sharedApplication->getFileSystem()->getExtensionPart(filename);

        std::vector<scene::SpriteFrame> frames;

        if (extension == "json")
        {
            frames = scene::SpriteFrame::loadSpriteFrames(filename, mipmaps);
        }
        else
        {
            std::shared_ptr<graphics::Texture> texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

            if (!texture)
            {
                return;
            }

            Rectangle rectangle(0, 0, texture->getSize().v[0], texture->getSize().v[1]);

            scene::SpriteFrame frame(texture, rectangle, false, texture->getSize(), Vector2(), Vector2(0.5f, 0.5f));
            frames.push_back(frame);
        }

        spriteFrames[filename] = frames;
    }

    const std::vector<scene::SpriteFrame>& Cache::getSpriteFrames(const std::string& filename, bool mipmaps) const
    {
        auto i = spriteFrames.find(filename);

        if (i != spriteFrames.end())
        {
            return i->second;
        }
        else
        {
            std::string extension = sharedApplication->getFileSystem()->getExtensionPart(filename);

            std::vector<scene::SpriteFrame> frames;

            if (extension == "json")
            {
                frames = scene::SpriteFrame::loadSpriteFrames(filename, mipmaps);
            }
            else
            {
                std::shared_ptr<graphics::Texture> texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

                if (texture)
                {
                    Rectangle rectangle(0.0f, 0.0f, texture->getSize().v[0], texture->getSize().v[1]);

                    scene::SpriteFrame frame = scene::SpriteFrame(texture, rectangle, false, texture->getSize(), Vector2(), Vector2(0.5f, 0.5f));
                    frames.push_back(frame);
                }
            }

            auto i = spriteFrames.insert(std::make_pair(filename, frames));

            return i.first->second;
        }
    }

    void Cache::setSpriteFrames(const std::string& filename, const std::vector<scene::SpriteFrame>& frames)
    {
        spriteFrames[filename] = frames;
    }

    void Cache::releaseSpriteFrames()
    {
        spriteFrames.clear();
    }

    void Cache::preloadParticleDefinition(const std::string& filename)
    {
        std::unordered_map<std::string, scene::ParticleDefinition>::const_iterator i = particleDefinitions.find(filename);

        if (i == particleDefinitions.end())
        {
            particleDefinitions[filename] = scene::ParticleDefinition::loadParticleDefinition(filename);
        }
    }

    const scene::ParticleDefinition& Cache::getParticleDefinition(const std::string& filename) const
    {
        auto i = particleDefinitions.find(filename);

        if (i != particleDefinitions.end())
        {
            return i->second;
        }
        else
        {
            auto i = particleDefinitions.insert(std::make_pair(filename, scene::ParticleDefinition::loadParticleDefinition(filename)));

            return i.first->second;
        }
    }

    void Cache::releaseParticleDefinitions()
    {
        particleDefinitions.clear();
    }

    void Cache::preloadBMFont(const std::string& filename)
    {
        std::unordered_map<std::string, BMFont>::const_iterator i = bmFonts.find(filename);

        if (i == bmFonts.end())
        {
            bmFonts[filename] = BMFont(filename);
        }
    }

    const BMFont& Cache::getBMFont(const std::string& filename) const
    {
        std::unordered_map<std::string, BMFont>::const_iterator i = bmFonts.find(filename);

        if (i != bmFonts.end())
        {
            return i->second;
        }
        else
        {
            auto i = bmFonts.insert(std::make_pair(filename, BMFont(filename)));

            return i.first->second;
        }
    }

    void Cache::releaseBMFonts()
    {
        bmFonts.clear();
    }
}
