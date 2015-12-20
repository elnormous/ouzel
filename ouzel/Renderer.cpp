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
    Renderer::Renderer(Engine* engine, Driver driver)
    {
        _engine = engine;
        _driver = driver;
    }

    Renderer::~Renderer()
    {
        for (std::pair<std::string, Texture*> texture : _textures)
        {
            texture.second->release();
        }
    }
    
    void Renderer::resize(const Size2& size)
    {
        _size = size;
        
        Matrix4::createOrthographic(_size.width, _size.height, 1.0f, 1000.0f, &_projection);
    }
    
    void Renderer::clear()
    {
    }
    
    void Renderer::flush()
    {
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
            result = loadTexture(filename);
            _textures[filename] = result;
        }
            
        return result;
    }
    
    void Renderer::activateTexture(Texture* texture, uint32_t layer)
    {
    }
    
    Texture* Renderer::loadTexture(const std::string& filename)
    {
        Texture* texture = new Texture(filename, this);
        
        return texture;
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
    
    Shader* Renderer::loadShader(const std::string& fragmentShader, const std::string& vertexShader)
    {
        Shader* shader = new Shader(fragmentShader, vertexShader, this);
        
        return shader;
    }
    
    void Renderer::activateShader(Shader* shader)
    {
    }

    Vector2 Renderer::absoluteToWorldLocation(const Vector2& position)
    {
        Camera* camera = _engine->getScene()->getCamera();
        
        if (camera)
        {
            float x = 2.0f * position.x / _size.width - 1.0f;
            float y = 2.0f * position.y / _size.height - 1.0f;
            
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
            
            float x = (result.x + 1.0f) / 2.0f * _size.width;
            float y = (result.y + 1.0f) / 2.0f * _size.height;
            
            return Vector2(x, y);
        }
        else
        {
            return Vector2();
        }
    }
    
    void drawLine(const Vector2& start, const Vector2& finish, const Vector3& color, const Matrix4& transform = Matrix4())
    {
    }
    
    void drawRectangle(const Rectangle& rectangle, const Vector3& color, const Matrix4& transform = Matrix4())
    {
    }
    
    void drawQuad(const Rectangle& rectangle, const Vector3& color, const Matrix4& transform = Matrix4())
    {
    }
}
