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
#include "Scene.h"
#include "FileSystem.h"
#include "File.h"

namespace ouzel
{
    Sprite::Sprite(const std::string& filename, Scene* scene):
        Node(scene)
    {
        _engine = _scene->getEngine();
        
        std::string extension = _engine->getFileSystem()->getExtension(filename);
        
        if (extension == "json")
        {
            loadSpriteSheet(filename);
        }
        else
        {
            _texture = _engine->getRenderer()->getTexture(filename);
            
            if (_texture)
            {
                _size = _texture->getSize();
                _boundingBox.set(-_size.width / 2.0f, -_size.height / 2.0f, _size.width, _size.height);
                
                addFrame(Rectangle(0, 0, _size.width, _size.height), _size);
            }
        }
        
        _shader = _engine->getRenderer()->getShader(SHADER_TEXTURE);
        
        if (_shader)
        {
#ifdef OUZEL_PLATFORM_WINDOWS
            _uniModelViewProj = 0;
#else
            _uniModelViewProj = _shader->getVertexShaderConstantId("modelViewProj");
#endif
        }
    }

    Sprite::~Sprite()
    {

    }
    
    bool Sprite::loadSpriteSheet(const std::string& filename)
    {
        File file(filename, File::Mode::READ, false, _scene->getEngine()->getFileSystem());
        
        if (file.isOpen())
        {
            rapidjson::FileReadStream is(file.getFile(), TEMP_BUFFER, sizeof(TEMP_BUFFER));
            
            rapidjson::Document document;
            document.ParseStream<0>(is);
            
            Size2 textureSize;
            
            if (document.HasParseError())
            {
                return false;
            }
            
            if (!document.HasMember("meta")) return false;
            
            const rapidjson::Value& metaObject = document["meta"];
            
            if (!metaObject.HasMember("size")) return false;
            const rapidjson::Value& sizeObject = metaObject["size"];
            
            if (!sizeObject.HasMember("w")) return false;
            if (!sizeObject.HasMember("h")) return false;
            
            textureSize.width = static_cast<float>(sizeObject["w"].GetInt());
            textureSize.height = static_cast<float>(sizeObject["h"].GetInt());
            
            if (!metaObject.HasMember("image")) return false;
            _texture = _engine->getRenderer()->getTexture(metaObject["image"].GetString());
            
            if (!document.HasMember("frames")) return false;
            const rapidjson::Value& framesArray = document["frames"];
            
            for (int index = 0; index < framesArray.Size(); ++index)
            {
                const rapidjson::Value& frameObject = framesArray[index];
                
                if (!frameObject.HasMember("frame")) return false;
                
                const rapidjson::Value& rectangleObject = frameObject["frame"];
                
                Rectangle rectangle(static_cast<float>(rectangleObject["x"].GetInt()),
                                    static_cast<float>(rectangleObject["y"].GetInt()),
                                    static_cast<float>(rectangleObject["w"].GetInt()),
                                    static_cast<float>(rectangleObject["h"].GetInt()));
                
                if (rectangle.width > _size.width) _size.width = rectangle.width;
                if (rectangle.height > _size.height) _size.height = rectangle.height;
                
                addFrame(rectangle, textureSize);
            }
        }
        else
        {
            return false;
        }
        
        return true;
    }
    
    void Sprite::addFrame(const Rectangle& rectangle, Size2 textureSize)
    {
        std::vector<uint16_t> indices = {0, 1, 2, 1, 3, 2};
        
        Vector2 leftBottom(rectangle.x / textureSize.width,
                           rectangle.y / textureSize.height);
        
        Vector2 rightTop((rectangle.x + rectangle.width) / textureSize.width,
                         (rectangle.y + rectangle.height) / textureSize.height);
        
        std::vector<Vertex> vertices = {
            Vertex(Vector3(-rectangle.width / 2.0f, -rectangle.height / 2.0f, 0.0f), _color, Vector2(leftBottom.x, rightTop.y)),
            Vertex(Vector3(rectangle.width / 2.0f, -rectangle.height / 2.0f, 0.0f), _color, Vector2(rightTop.x, rightTop.y)),
            Vertex(Vector3(-rectangle.width / 2.0f, rectangle.height / 2.0f, 0.0f),  _color, Vector2(leftBottom.x, leftBottom.y)),
            Vertex(Vector3(rectangle.width / 2.0f, rectangle.height / 2.0f, 0.0f),  _color, Vector2(rightTop.x, leftBottom.y))
        };
        
        _frameVertices.push_back(vertices);
        
        _frameMeshBuffers.push_back(_scene->getEngine()->getRenderer()->createMeshBuffer(indices, vertices, false, true));
    }

    void Sprite::draw()
    {
        Node::draw();
        
        if (_shader && _texture)
        {
            _engine->getRenderer()->activateTexture(_texture, 0);
            _engine->getRenderer()->activateShader(_shader);
            
            Matrix4 modelViewProj = _engine->getRenderer()->getProjection() * _engine->getScene()->getCamera()->getTransform() * _transform;
            
            _shader->setVertexShaderConstant(_uniModelViewProj, &modelViewProj, 1);
            
            if (_frameMeshBuffers.size() > _currentFrame)
            {
                MeshBuffer* meshBuffer = _frameMeshBuffers[_currentFrame];
                
                _engine->getRenderer()->drawMeshBuffer(meshBuffer);
            }
        }
    }
    
    void Sprite::update(float delta)
    {
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
    
    void Sprite::setTexture(Texture* texture)
    {
        _texture = texture;
    }
    
    void Sprite::setShader(Shader* shader)
    {
        _shader = shader;
    }
    
    void Sprite::setColor(const Color& color)
    {
        _color = color;
        
        for (uint32_t i = 0; i < _frameMeshBuffers.size(); ++i)
        {
            for (Vertex& vertex : _frameVertices[i])
            {
                vertex.color = color;
            }
            
            MeshBuffer* meshBuffer = _frameMeshBuffers[i];
            meshBuffer->uploadVertices(_frameVertices[i]);
        }
    }
    
    bool Sprite::checkVisibility() const
    {
        Matrix4 mvp = _engine->getRenderer()->getProjection() * _engine->getScene()->getCamera()->getTransform() * _transform;
        
        Vector3 topRight(_size.width / 2.0f, _size.height / 2.0f, 0.0f);
        Vector3 bottomLeft(-_size.width / 2.0f, -_size.height / 2.0f, 0.0f);
        
        mvp.transformPoint(&topRight);
        mvp.transformPoint(&bottomLeft);
        
        return (topRight.x >= -1.0f && topRight.y >= -1.0f &&
                bottomLeft.x <= 1.0f && bottomLeft.y <= 1.0f);
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
