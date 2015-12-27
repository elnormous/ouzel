// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sprite.h"
#include "Engine.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include "Utils.h"
#include "Camera.h"
#include "Scene.h"

namespace ouzel
{
    Sprite::Sprite(const std::string& filename, Scene* scene):
        Node(scene)
    {
        _engine = _scene->getEngine();
        
        _texture = _engine->getRenderer()->getTexture(filename);
        
        if (_texture)
        {
            _size = _texture->getSize();
            _boundingBox.set(-_size.width / 2.0f, -_size.height / 2.0f, _size.width, _size.height);
        }

        _shader = _engine->getRenderer()->getShader(SHADER_TEXTURE);
        
        std::vector<uint16_t> indices = {0, 1, 2, 1, 3, 2};
        
        std::vector<Vertex> vertices = {
            Vertex(Vector4(-_size.width / 2.0f, -_size.height / 2.0f, -20.0f, 1.0f), Color(0xFF, 0xFF, 0xFF, 0xFF), Vector2(0.0f, 1.0f)),
            Vertex(Vector4(_size.width / 2.0f, -_size.height / 2.0f, -20.0f, 1.0f), Color(0xFF, 0xFF, 0xFF, 0xFF), Vector2(1.0f, 1.0f)),
            Vertex(Vector4(-_size.width / 2.0f, _size.height / 2.0f, -20.0f, 1.0f), Color(0xFF, 0xFF, 0xFF, 0xFF), Vector2(0.0f, 0.0f)),
            Vertex(Vector4(_size.width / 2.0f, _size.height / 2.0f, -20.0f, 1.0f), Color(0xFF, 0xFF, 0xFF, 0xFF), Vector2(1.0f, 0.0f))
        };
        
        _meshBuffer = _scene->getEngine()->getRenderer()->createMeshBuffer(indices, vertices);
        
    }

    Sprite::~Sprite()
    {

    }

    void Sprite::draw()
    {
        Node::draw();
        
        if (_shader && _texture)
        {
            _engine->getRenderer()->activateTexture(_texture, 0);
            _engine->getRenderer()->activateShader(_shader);
            
            Matrix4 projViewModel = _engine->getRenderer()->getProjection() * _engine->getScene()->getCamera()->getTransform() * _transform;
            
#ifdef OUZEL_PLATFORM_WINDOWS
            uint32_t uniProjViewModel = 0;
#else
            uint32_t uniProjViewModel = _shader->getVertexShaderConstantId("projViewModel");
#endif

            _shader->setVertexShaderConstant(uniProjViewModel, &projViewModel, 1);
            
            _engine->getRenderer()->drawMeshBuffer(_meshBuffer);
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
}
