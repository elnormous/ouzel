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
#include "MeshBuffer.h"

namespace ouzel
{
    Renderer::Renderer(const Size2& size, bool fullscreen, Engine* engine, Driver driver):
        _engine(engine), _driver(driver), _size(size)
    {
        for (int i = 0; i < TEXTURE_LAYERS; ++i)
        {
            _activeTextures[i] = nullptr;
        }
    }

    Renderer::~Renderer()
    {
        for (std::pair<std::string, Texture*> texture : _textures)
        {
            texture.second->release();
        }
    }
    
    void Renderer::recalculateProjection()
    {
        Matrix4::createOrthographic(_size.width, _size.height, 1.0f, 1000.0f, &_projection);
    }
    
    void Renderer::begin()
    {

    }
    
    void Renderer::clear()
    {
    }
    
    void Renderer::flush()
    {
    }
    
    void Renderer::resize(const Size2& size)
    {
        _size = size;
        recalculateProjection();
    }
    
    void Renderer::preloadTexture(const std::string& filename)
    {
        std::unordered_map<std::string, Texture*>::const_iterator i = _textures.find(filename);
        
        if (i == _textures.end())
        {
            Texture* texture = loadTextureFromFile(filename);
            
            if (texture)
            {
                _textures[filename] = texture;
            }
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
            result = loadTextureFromFile(filename);
            
            if (result)
            {
                _textures[filename] = result;
            }
        }
            
        return result;
    }
    
    bool Renderer::activateTexture(Texture* texture, uint32_t layer)
    {
        _activeTextures[layer] = texture;
        
        return true;
    }
    
    Texture* Renderer::loadTextureFromFile(const std::string& filename)
    {
        Texture* texture = new Texture(this);
        
        if (!texture->initFromFile(filename))
        {
            delete texture;
            texture = nullptr;
        }
        
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
    
    Shader* Renderer::loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader)
    {
        Shader* shader = new Shader(this);
        
        if (!shader->initFromFiles(fragmentShader, vertexShader))
        {
            delete shader;
            shader = nullptr;
        }
        
        return shader;
    }
    
    Shader* Renderer::loadShaderFromStrings(const std::string& fragmentShader, const std::string& vertexShader)
    {
        Shader* shader = new Shader(this);
        
        if (!shader->initFromStrings(fragmentShader, vertexShader))
        {
            delete shader;
            shader = nullptr;
        }
        
        return shader;
    }
    
    bool Renderer::activateShader(Shader* shader)
    {
        _activeShader = shader;
        
        return true;
    }
    
    MeshBuffer* Renderer::createMeshBuffer(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices)
    {
        MeshBuffer* meshBuffer = new MeshBuffer(this);
        
        if (!meshBuffer->initFromData(indices, vertices))
        {
            delete meshBuffer;
            meshBuffer = nullptr;
        }
        
        return meshBuffer;
    }
    
    bool Renderer::drawMeshBuffer(MeshBuffer* meshBuffer, const Matrix4& transform)
    {
        if (!_activeShader)
        {
            return false;
        }
        
        return true;
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
    
    void Renderer::drawLine(const Vector2& start, const Vector2& finish, const Color& color, const Matrix4& transform)
    {
    }
    
    void Renderer::drawRectangle(const Rectangle& rectangle, const Color& color, const Matrix4& transform)
    {
    }
    
    void Renderer::drawQuad(const Rectangle& rectangle, const Color& color, const Matrix4& transform)
    {
    }
}
