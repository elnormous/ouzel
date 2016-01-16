// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Renderer.h"
#include "Engine.h"
#include "Texture.h"
#include "Node.h"
#include "Utils.h"
#include "Shader.h"
#include "Camera.h"
#include "EventHander.h"
#include "SceneManager.h"
#include "MeshBuffer.h"
#include "EventDispatcher.h"
#include "RenderTarget.h"

namespace ouzel
{
    Renderer::Renderer()
    {
        
    }

    Renderer::~Renderer()
    {
        
    }

    bool Renderer::init(const Size2& size, bool resizable, bool fullscreen, Driver driver)
    {
        _driver = driver;
        _size = size;
        _resizable = resizable;
        _fullscreen = fullscreen;

        return true;
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
        if (_size != size)
        {
            _size = size;
            Engine::getInstance()->getSceneManager()->recalculateProjection();
            
            WindowEvent event;
            event.type = Event::Type::WINDOW_SIZE_CHANGE;
            event.size = _size;
            event.title = _title;
            
            Engine::getInstance()->getEventDispatcher()->dispatchWindowSizeChangeEvent(event, Engine::getInstance()->getRenderer());
        }
    }
    
    void Renderer::setTitle(const std::string& title)
    {
        if (_title != title)
        {
            _title = title;
            
            WindowEvent event;
            event.type = Event::Type::WINDOW_TITLE_CHANGE;
            event.size = _size;
            event.title = _title;
            
            Engine::getInstance()->getEventDispatcher()->dispatchWindowTitleChangeEvent(event, Engine::getInstance()->getRenderer());
        }
    }
    
    void Renderer::releaseTextures()
    {
        _textures.clear();
    }
    
    TexturePtr Renderer::createTexture(bool dynamic)
    {
        TexturePtr texture(new Texture());
        texture->init(dynamic);
        
        return texture;
    }
    
    void Renderer::preloadTexture(const std::string& filename, bool dynamic)
    {
        std::unordered_map<std::string, TexturePtr>::const_iterator i = _textures.find(filename);
        
        if (i == _textures.end())
        {
            TexturePtr texture = loadTextureFromFile(filename, dynamic);
            
            if (texture)
            {
                _textures[filename] = texture;
            }
        }
    }

    TexturePtr Renderer::getTexture(const std::string& filename)
    {
        TexturePtr result;
        
        std::unordered_map<std::string, TexturePtr>::const_iterator i = _textures.find(filename);
        
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
    
    bool Renderer::activateTexture(TexturePtr const& texture, uint32_t layer)
    {
        _activeTextures[layer] = texture;
        
        return true;
    }
    
    RenderTargetPtr Renderer::createRenderTarget(Size2 const& size, bool depthBuffer)
    {
        RenderTargetPtr renderTarget(new RenderTarget());
        
        if (!renderTarget->init(size, depthBuffer))
        {
            renderTarget.reset();
        }
        
        return renderTarget;
    }
    
    TexturePtr Renderer::loadTextureFromFile(const std::string& filename, bool dynamic)
    {
        TexturePtr texture(new Texture());
        
        if (!texture->initFromFile(filename, dynamic))
        {
            texture.reset();
        }
        
        return texture;
    }
    
    TexturePtr Renderer::loadTextureFromData(const void* data, const Size2& size, bool dynamic)
    {
        TexturePtr texture(new Texture());
        
        if (!texture->initFromData(data, size, dynamic))
        {
            texture.reset();
        }
        
        return texture;
    }
    
    ShaderPtr Renderer::getShader(const std::string& shaderName) const
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
    
    void Renderer::setShader(const std::string& shaderName, ShaderPtr shader)
    {
        _shaders[shaderName] = shader;
    }
    
    ShaderPtr Renderer::loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes)
    {
        ShaderPtr shader(new Shader());
        
        if (!shader->initFromFiles(fragmentShader, vertexShader, vertexAttributes))
        {
            shader.reset();
        }
        
        return shader;
    }
    
    ShaderPtr Renderer::loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes)
    {
        ShaderPtr shader(new Shader());
        
        if (!shader->initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize, vertexAttributes))
        {
            shader.reset();
        }
        
        return shader;
    }
    
    bool Renderer::activateShader(ShaderPtr const& shader)
    {
        _activeShader = shader;
        
        return true;
    }
    
    MeshBufferPtr Renderer::createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes)
    {
        MeshBufferPtr meshBuffer(new MeshBuffer());
        
        if (!meshBuffer->initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexSize, vertexCount, dynamicVertexBuffer, vertexAttributes))
        {
            meshBuffer.reset();
        }
        
        return meshBuffer;
    }
    
    bool Renderer::drawMeshBuffer(MeshBufferPtr const& meshBuffer)
    {
        if (_activeShader)
        {
            if (meshBuffer->getVertexAttributes() != _activeShader->getVertexAttributes())
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        
        return true;
    }

    Vector2 Renderer::viewToScreenLocation(const Vector2& position)
    {
        float x = 2.0f * position.x / _size.width - 1.0f;
        float y = 2.0f * (_size.height - position.y) / _size.height - 1.0f;
        
        return Vector2(x, y);
    }

    Vector2 Renderer::screenToViewLocation(const Vector2& position)
    {
        float x = (position.x + 1.0f) / 2.0f * _size.width;
        float y = _size.height - (position.y + 1.0f) / 2.0f * _size.height;
        
        return Vector2(x, y);
    }
    
    bool Renderer::drawLine(const Vector2& start, const Vector2& finish, const Color& color, const Matrix4& transform)
    {
        OUZEL_UNUSED(start);
        OUZEL_UNUSED(finish);
        OUZEL_UNUSED(color);
        OUZEL_UNUSED(transform);
        
        return true;
    }
    
    bool Renderer::drawRectangle(const Rectangle& rectangle, const Color& color, const Matrix4& transform)
    {
        OUZEL_UNUSED(rectangle);
        OUZEL_UNUSED(color);
        OUZEL_UNUSED(transform);
        
        return true;
    }
    
    bool Renderer::drawQuad(const Rectangle& rectangle, const Color& color, const Matrix4& transform)
    {
        OUZEL_UNUSED(rectangle);
        OUZEL_UNUSED(color);
        OUZEL_UNUSED(transform);
        
        return true;
    }
}
