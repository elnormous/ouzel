// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "Cache.h"
#include "Engine.h"
#include "Application.h"
#include "graphics/Renderer.h"
#include "graphics/Texture.h"
#include "graphics/Shader.h"
#include "scene/ParticleDefinition.h"
#include "scene/SpriteFrame.h"
#include "files/FileSystem.h"
#include "utils/Utils.h"

namespace ouzel
{
    Cache::Cache()
    {
    }

    void Cache::preloadTexture(const std::string& filename, bool dynamic, bool mipmaps)
    {
        std::unordered_map<std::string, graphics::TexturePtr>::const_iterator i = textures.find(filename);

        if (i == textures.end())
        {
            graphics::TexturePtr texture = sharedEngine->getRenderer()->createTexture();
            texture->initFromFile(filename, dynamic, mipmaps);

            textures[filename] = texture;
        }
    }

    graphics::TexturePtr Cache::getTexture(const std::string& filename, bool dynamic, bool mipmaps) const
    {
        graphics::TexturePtr result;

        std::unordered_map<std::string, graphics::TexturePtr>::const_iterator i = textures.find(filename);

        if (i != textures.end())
        {
            return i->second;
        }
        else
        {
            result = sharedEngine->getRenderer()->createTexture();
            result->initFromFile(filename, dynamic, mipmaps);

            textures[filename] = result;
        }

        return result;
    }

    void Cache::setTexture(const std::string& filename, const graphics::TexturePtr& texture)
    {
        textures[filename] = texture;
    }

    void Cache::releaseTextures()
    {
        textures.clear();
    }

    void Cache::preloadSpriteFrames(const std::string& filename, bool mipmaps)
    {
        std::string extension = sharedApplication->getFileSystem()->getExtensionPart(filename);

        std::vector<scene::SpriteFramePtr> frames;

        if (extension == "json")
        {
            frames = scene::SpriteFrame::loadSpriteFrames(filename, mipmaps);
        }
        else
        {
            graphics::TexturePtr texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

            if (!texture)
            {
                return;
            }

            Rectangle rectangle(0, 0, texture->getSize().width, texture->getSize().height);

            scene::SpriteFramePtr frame = std::make_shared<scene::SpriteFrame>(texture, rectangle, false, texture->getSize(), Vector2(), Vector2(0.5f, 0.5f));
            frames.push_back(frame);
        }

        spriteFrames[filename] = frames;
    }

    std::vector<scene::SpriteFramePtr> Cache::getSpriteFrames(const std::string& filename, bool mipmaps) const
    {
        std::unordered_map<std::string, std::vector<scene::SpriteFramePtr>>::const_iterator i = spriteFrames.find(filename);

        if (i != spriteFrames.end())
        {
            return i->second;
        }
        else
        {
            std::string extension = sharedApplication->getFileSystem()->getExtensionPart(filename);

            std::vector<scene::SpriteFramePtr> frames;

            if (extension == "json")
            {
                frames = scene::SpriteFrame::loadSpriteFrames(filename, mipmaps);
            }
            else
            {
                graphics::TexturePtr texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

                if (!texture)
                {
                    return frames;
                }

                Rectangle rectangle(0.0f, 0.0f, texture->getSize().width, texture->getSize().height);

                scene::SpriteFramePtr frame = std::make_shared<scene::SpriteFrame>(texture, rectangle, false, texture->getSize(), Vector2(), Vector2(0.5f, 0.5f));
                frames.push_back(frame);
            }

            spriteFrames[filename] = frames;

            return frames;
        }
    }

    void Cache::setSpriteFrames(const std::string& filename, const std::vector<scene::SpriteFramePtr>& frames)
    {
        spriteFrames[filename] = frames;
    }

    void Cache::releaseSpriteFrames()
    {
        spriteFrames.clear();
    }

    graphics::ShaderPtr Cache::getShader(const std::string& shaderName) const
    {
        std::unordered_map<std::string, graphics::ShaderPtr>::const_iterator i = shaders.find(shaderName);

        if (i != shaders.end())
        {
            return i->second;
        }
        else
        {
            return nullptr;
        }
    }

    void Cache::setShader(const std::string& shaderName, const graphics::ShaderPtr& shader)
    {
        shaders[shaderName] = shader;
    }

    void Cache::preloadParticleDefinition(const std::string& filename)
    {
        std::unordered_map<std::string, scene::ParticleDefinitionPtr>::const_iterator i = particleDefinitions.find(filename);

        if (i == particleDefinitions.end())
        {
            scene::ParticleDefinitionPtr result = scene::ParticleDefinition::loadParticleDefinition(filename);

            if (result)
            {
                particleDefinitions[filename] = result;
            }
        }
    }

    scene::ParticleDefinitionPtr Cache::getParticleDefinition(const std::string& filename) const
    {
        scene::ParticleDefinitionPtr result;

        std::unordered_map<std::string, scene::ParticleDefinitionPtr>::const_iterator i = particleDefinitions.find(filename);

        if (i != particleDefinitions.end())
        {
            return i->second;
        }
        else
        {
            result = scene::ParticleDefinition::loadParticleDefinition(filename);

            if (result)
            {
                particleDefinitions[filename] = result;
            }
        }

        return result;
    }

    graphics::BlendStatePtr Cache::getBlendState(const std::string& blendStateName) const
    {
        std::unordered_map<std::string, graphics::BlendStatePtr>::const_iterator i = blendStates.find(blendStateName);

        if (i != blendStates.end())
        {
            return i->second;
        }
        else
        {
            return nullptr;
        }
    }

    void Cache::setBlendState(const std::string& blendStateName, const graphics::BlendStatePtr& blendState)
    {
        blendStates[blendStateName] = blendState;
    }
}
