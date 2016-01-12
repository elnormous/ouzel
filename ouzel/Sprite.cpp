// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "Sprite.h"
#include "CompileConfig.h"
#include "Engine.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include "Utils.h"
#include "Camera.h"
#include "SceneManager.h"
#include "FileSystem.h"
#include "File.h"
#include "Layer.h"

namespace ouzel
{
    Sprite::Sprite()
    {
    }

    Sprite::~Sprite()
    {

    }
    
    bool Sprite::initFromFile(const std::string& filename)
    {
        std::string extension = Engine::getInstance()->getFileSystem()->getExtension(filename);
        
        if (extension == "json")
        {
            if (!loadSpriteSheet(filename))
            {
                return false;
            }
        }
        else
        {
            _texture = Engine::getInstance()->getRenderer()->getTexture(filename);
            
            if (!_texture)
            {
                return false;
            }
            
            _size = _texture->getSize();
            _boundingBox.set(-_size.width / 2.0f, -_size.height / 2.0f, _size.width, _size.height);
            
            Rectangle rectangle(0, 0, _size.width, _size.height);
            
            addFrame(rectangle, _size, false, _size, Vector2(), Vector2(0.5f, 0.5f));
        }
        
        _shader = Engine::getInstance()->getRenderer()->getShader(SHADER_TEXTURE);
        
        if (!_shader)
        {
            return false;
        }
        
#ifdef OUZEL_PLATFORM_WINDOWS
        _uniModelViewProj = 0;
#else
        _uniModelViewProj = _shader->getVertexShaderConstantId("modelViewProj");
#endif

        return true;
    }
    
    bool Sprite::loadSpriteSheet(const std::string& filename)
    {
        File file(filename, File::Mode::READ, false);
        
        if (file.isOpen())
        {
            rapidjson::FileReadStream is(file.getFile().get(), TEMP_BUFFER, sizeof(TEMP_BUFFER));
            
            rapidjson::Document document;
            document.ParseStream<0>(is);
            
            if (document.HasParseError())
            {
                return false;
            }
            
            const rapidjson::Value& metaObject = document["meta"];
            const rapidjson::Value& sizeObject = metaObject["size"];
            
            Size2 textureSize(static_cast<float>(sizeObject["w"].GetInt()),
                              static_cast<float>(sizeObject["h"].GetInt()));
            
            _texture = Engine::getInstance()->getRenderer()->getTexture(metaObject["image"].GetString());
            
            const rapidjson::Value& framesArray = document["frames"];
            
            for (int index = 0; index < framesArray.Size(); ++index)
            {
                const rapidjson::Value& frameObject = framesArray[index];
                
                const rapidjson::Value& rectangleObject = frameObject["frame"];
                
                Rectangle rectangle(static_cast<float>(rectangleObject["x"].GetInt()),
                                    static_cast<float>(rectangleObject["y"].GetInt()),
                                    static_cast<float>(rectangleObject["w"].GetInt()),
                                    static_cast<float>(rectangleObject["h"].GetInt()));
                
                bool rotated = frameObject["rotated"].GetBool();
                
                const rapidjson::Value& sourceSizeObject = frameObject["sourceSize"];
                
                Size2 sourceSize(static_cast<float>(sourceSizeObject["w"].GetInt()),
                                 static_cast<float>(sourceSizeObject["h"].GetInt()));
                
                if (sourceSize.width > _size.width) _size.width = sourceSize.width;
                if (sourceSize.height > _size.height) _size.height = sourceSize.height;
                
                const rapidjson::Value& spriteSourceSizeObject = frameObject["spriteSourceSize"];
                
                Vector2 offset(static_cast<float>(spriteSourceSizeObject["x"].GetInt()),
                               static_cast<float>(spriteSourceSizeObject["y"].GetInt()));
                
                const rapidjson::Value& pivotObject = frameObject["pivot"];
                
                Vector2 pivot(static_cast<float>(pivotObject["x"].GetDouble()),
                              static_cast<float>(pivotObject["y"].GetDouble()));
                
                addFrame(rectangle, textureSize, rotated, sourceSize, offset, pivot);
            }
        }
        else
        {
            return false;
        }
        
        return true;
    }
    
    void Sprite::addFrame(const Rectangle& rectangle, const Size2& textureSize, bool rotated, const Size2& sourceSize, const Vector2& offset, const Vector2& pivot)
    {
        std::vector<uint16_t> indices = {0, 1, 2, 1, 3, 2};
        
        Vector2 textCoords[4];
        Vector2 realOffset(-sourceSize.width * pivot.x + offset.x,
                           -sourceSize.height * pivot.y + (sourceSize.height - rectangle.height - offset.y));
        
        if (!rotated)
        {
            Vector2 leftTop(rectangle.x / textureSize.width,
                            rectangle.y / textureSize.height);
            
            Vector2 rightBottom((rectangle.x + rectangle.width) / textureSize.width,
                                (rectangle.y + rectangle.height) / textureSize.height);
            
            textCoords[0] = Vector2(leftTop.x, rightBottom.y);
            textCoords[1] = Vector2(rightBottom.x, rightBottom.y);
            textCoords[2] = Vector2(leftTop.x, leftTop.y);
            textCoords[3] = Vector2(rightBottom.x, leftTop.y);
        }
        else
        {
            Vector2 leftTop = Vector2(rectangle.x / textureSize.width,
                                      rectangle.y / textureSize.height);
            
            Vector2 rightBottom = Vector2((rectangle.x + rectangle.height) / textureSize.width,
                                          (rectangle.y + rectangle.width) / textureSize.height);
            
            textCoords[0] = Vector2(leftTop.x, leftTop.y);
            textCoords[1] = Vector2(leftTop.x, rightBottom.y);
            textCoords[2] = Vector2(rightBottom.x, leftTop.y);
            textCoords[3] = Vector2(rightBottom.x, rightBottom.y);
        }
        
        std::vector<VertexPCT> vertices = {
            VertexPCT(Vector3(realOffset.x, realOffset.y, 0.0f), _color, textCoords[0]),
            VertexPCT(Vector3(realOffset.x + rectangle.width, realOffset.y, 0.0f), _color, textCoords[1]),
            VertexPCT(Vector3(realOffset.x, realOffset.y + rectangle.height, 0.0f),  _color, textCoords[2]),
            VertexPCT(Vector3(realOffset.x + rectangle.width, realOffset.y + rectangle.height, 0.0f),  _color, textCoords[3])
        };
        
        _frameVertices.push_back(vertices);
        
        _frameMeshBuffers.push_back(Engine::getInstance()->getRenderer()->createMeshBuffer(indices.data(), sizeof(uint16_t), static_cast<uint32_t>(indices.size()), false,
                                                                              vertices.data(), sizeof(VertexPCT), static_cast<uint32_t>(vertices.size()), true,
                                                                              VertexPCT::ATTRIBUTES));
    }

    void Sprite::update(float delta)
    {
        Node::update(delta);
        
        if (_playing)
        {
            _timeSinceLastFrame += delta;
            
            while (_timeSinceLastFrame > _frameInterval)
            {
                _timeSinceLastFrame -= _frameInterval;
                _currentFrame++;
                
                if (_currentFrame >= _frameMeshBuffers.size())
                {
                    _currentFrame = 0;
                    
                    if (!_repeat)
                    {
                        _playing = false;
                    }
                }
            }
        }
    }
    
    void Sprite::draw()
    {
        Node::draw();
        
        LayerPtr layer = _layer.lock();
        
        if (_shader && _texture && layer)
        {
            Engine::getInstance()->getRenderer()->activateTexture(_texture, 0);
            Engine::getInstance()->getRenderer()->activateShader(_shader);
            
            Matrix4 modelViewProj = layer->getProjection() * layer->getCamera()->getTransform() * _transform;
            
            _shader->setVertexShaderConstant(_uniModelViewProj, { modelViewProj });
            
            if (_frameMeshBuffers.size() > _currentFrame)
            {
                MeshBufferPtr meshBuffer = _frameMeshBuffers[_currentFrame];
                
                Engine::getInstance()->getRenderer()->drawMeshBuffer(meshBuffer);
            }
        }
    }
    
    void Sprite::setTexture(TexturePtr const& texture)
    {
        _texture = texture;
    }
    
    void Sprite::setShader(ShaderPtr const& shader)
    {
        _shader = shader;
    }
    
    void Sprite::setColor(const Color& color)
    {
        _color = color;
        
        for (uint32_t i = 0; i < _frameMeshBuffers.size(); ++i)
        {
            for (VertexPCT& vertex : _frameVertices[i])
            {
                vertex.color = color;
            }
            
            MeshBufferPtr meshBuffer = _frameMeshBuffers[i];
            meshBuffer->uploadVertices(_frameVertices[i].data(), static_cast<uint32_t>(_frameVertices[i].size()));
        }
    }
    
    bool Sprite::checkVisibility() const
    {
        if (LayerPtr layer = _layer.lock())
        {
            Matrix4 mvp = layer->getProjection() * layer->getCamera()->getTransform() * _transform;
            
            Vector3 topRight(_size.width / 2.0f, _size.height / 2.0f, 0.0f);
            Vector3 bottomLeft(-_size.width / 2.0f, -_size.height / 2.0f, 0.0f);
            
            mvp.transformPoint(&topRight);
            mvp.transformPoint(&bottomLeft);
            
            return (topRight.x >= -1.0f && topRight.y >= -1.0f &&
                    bottomLeft.x <= 1.0f && bottomLeft.y <= 1.0f);
        }
        else
        {
            return false;
        }
    }
    
    void Sprite::play(bool repeat, float frameInterval)
    {
        _repeat = repeat;
        _frameInterval = frameInterval;
        _playing = true;
    }
    
    void Sprite::stop(bool reset)
    {
        _playing = false;
        
        if (reset)
        {
            _currentFrame = 0;
            _timeSinceLastFrame = 0.0f;
        }
    }
}
