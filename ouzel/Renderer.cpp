// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Renderer.h"
#include "Engine.h"
#include "Texture.h"
#include "Node.h"
#include "Utils.h"
#include "Shader.h"
#include "Sprite.h"
#include "Camera.h"
#include "EventHander.h"
#include "Scene.h"

namespace ouzel
{
    Renderer::Renderer(Engine* engine)
    {
        _engine = engine;
    }

    Renderer::~Renderer()
    {
        for (std::pair<std::string, Texture*> texture : _textures)
        {
            texture.second->release();
        }
    }
    
    void Renderer::resize(uint32_t width, uint32_t height)
    {
        _width = width;
        _height = height;
        
        Matrix4::createOrthographic(_width, _height, 1.0f, 1000.0f, &_projection);
    }
    
    void Renderer::preloadTexture(const std::string& filename)
    {
        std::unordered_map<std::string, Texture*>::const_iterator i = _textures.find(filename);
        
        if (i == _textures.end())
        {
            Texture* texture = new Texture(filename, this);
            _textures[filename] = texture;
        }
    }

    Texture* Renderer::getTexture(const std::string& filename)
    {
        Texture* result = nullptr;
        
        std::unordered_map<std::string, Texture*>::const_iterator i = _textures.find(filename);
        
        if (i != _textures.end())
        {
            return i->second;
        }
        else
        {
            result = new Texture(filename, this);
            _textures[filename] = result;
        }
            
        return result;
    }
    
    Shader* Renderer::getShader(const std::string& shaderName) const
    {
        std::unordered_map<std::string, Shader*>::const_iterator i = _shaders.find(shaderName);
        
        if (i != _shaders.end())
        {
            return i->second;
        }
        else
        {
            return nullptr;
        }
    }
    
    void Renderer::setShader(const std::string& shaderName, Shader* shader)
    {
        std::unordered_map<std::string, Shader*>::iterator i = _shaders.find(shaderName);
        
        if (i != _shaders.end())
        {
            i->second->release();
        }
        
        _shaders[shaderName] = shader;
        
        if (shader)
        {
            shader->retain();
        }
    }

    Vector2 Renderer::absoluteToWorldLocation(const Vector2& position)
    {
        Camera* camera = _engine->getScene()->getCamera();
        
        if (camera)
        {
            float x = 2.0f * position.x / _width - 1.0f;
            float y = 2.0f * position.y / _height - 1.0f;
            
            Matrix4 projViewMatrix = _projection * camera->getTransform();
            Matrix4 inverseViewMatrix = projViewMatrix;
            inverseViewMatrix.invert();
            
            Vector3 result = Vector3(x, y, 0.0f);
            inverseViewMatrix.transformPoint(&result);
            
            return Vector2(result.x, result.y);
        }
        else
        {
            return Vector2();
        }
    }

    Vector2 Renderer::worldToAbsoluteLocation(const Vector2& position)
    {
        Camera* camera = _engine->getScene()->getCamera();
        
        if (camera)
        {
            Matrix4 projViewMatrix = _projection * camera->getTransform();
            
            Vector3 result = Vector3(position.x, position.y, 0.0f);
            projViewMatrix.transformPoint(&result);
            
            float x = (result.x + 1.0f) / 2.0f * _width;
            float y = (result.y + 1.0f) / 2.0f * _height;
            
            return Vector2(x, y);
        }
        else
        {
            return Vector2();
        }
    }
}
