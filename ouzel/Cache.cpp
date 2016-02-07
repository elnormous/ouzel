// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Cache.h"
#include "Engine.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"

namespace ouzel
{
    Cache::Cache()
    {
        
    }
    
    Cache::~Cache()
    {
        
    }
    
    void Cache::preloadTexture(const std::string& filename, bool dynamic)
    {
        std::unordered_map<std::string, TexturePtr>::const_iterator i = _textures.find(filename);
        
        if (i == _textures.end())
        {
            TexturePtr texture = Engine::getInstance()->getRenderer()->loadTextureFromFile(filename, dynamic);
            
            if (texture)
            {
                _textures[filename] = texture;
            }
        }
    }
    
    TexturePtr Cache::getTexture(const std::string& filename)
    {
        TexturePtr result;
        
        std::unordered_map<std::string, TexturePtr>::const_iterator i = _textures.find(filename);
        
        if (i != _textures.end())
        {
            return i->second;
        }
        else
        {
            result = Engine::getInstance()->getRenderer()->loadTextureFromFile(filename);
            
            if (result)
            {
                _textures[filename] = result;
            }
        }
        
        return result;
    }
    
    void Cache::setTexture(const std::string& filename, const TexturePtr& texture)
    {
        _textures[filename] = texture;
    }
    
    void Cache::releaseTextures()
    {
        _textures.clear();
    }
    
    ShaderPtr Cache::getShader(const std::string& shaderName) const
    {
        std::unordered_map<std::string, ShaderPtr>::const_iterator i = _shaders.find(shaderName);
        
        if (i != _shaders.end())
        {
            return i->second;
        }
        else
        {
            return nullptr;
        }
    }
    
    void Cache::setShader(const std::string& shaderName, const ShaderPtr& shader)
    {
        _shaders[shaderName] = shader;
    }
}