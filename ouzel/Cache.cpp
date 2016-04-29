// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "Cache.h"
#include "Engine.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include "File.h"
#include "ParticleDefinition.h"
#include "Utils.h"

namespace ouzel
{
    Cache::Cache()
    {

    }

    Cache::~Cache()
    {

    }

    void Cache::preloadTexture(const std::string& filename, bool dynamic, bool mipmaps)
    {
        std::unordered_map<std::string, graphics::TexturePtr>::const_iterator i = textures.find(filename);

        if (i == textures.end())
        {
            graphics::TexturePtr texture = sharedEngine->getRenderer()->loadTextureFromFile(filename, dynamic, mipmaps);

            if (texture)
            {
                textures[filename] = texture;
            }
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
            result = sharedEngine->getRenderer()->loadTextureFromFile(filename, dynamic, mipmaps);

            if (result)
            {
                textures[filename] = result;
            }
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
    }

    std::vector<scene::SpriteFramePtr> Cache::getSpriteFrames(const std::string& filename) const
    {
        std::unordered_map<std::string, graphics::TexturePtr>::const_iterator i = textures.find(filename);

        if (i == textures.end())
        {

        }

        return std::vector<scene::SpriteFramePtr>();
    }

    void Cache::setSpriteFrames(const std::string& filename, const std::vector<scene::SpriteFramePtr>& spriteFrames)
    {
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
