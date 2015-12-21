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

//TODO: remove
#include "RendererOGL.h"

namespace ouzel
{
    Sprite::Sprite(const std::string& filename, Scene* scene):
        Node(scene)
    {
        _engine = _scene->getEngine();
        
        _texture = _engine->getRenderer()->getTexture(filename);
        
        _size = _texture->getSize();
        
        _shader = _engine->getRenderer()->getShader(SHADER_TEXTURE);
        
        glGenVertexArrays(1, &_vertexArray);
        glBindVertexArray(_vertexArray);
        
        GLfloat width = static_cast<GLfloat>(_size.width);
        GLfloat height = static_cast<GLfloat>(_size.height);
        
        GLfloat vertices[] = {
            -width / 2.0f, -height / 2.0f, -20.0f, 1.0f,
            width / 2.0f, -height / 2.0f, -20.0f, 1.0f,
            -width / 2.0f, height / 2.0f, -20.0f, 1.0f,
            width / 2.0f, height / 2.0f, -20.0f, 1.0f};
        
        glGenBuffers(1, &_vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        
        GLfloat colors[] = {
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
        };
        
        glGenBuffers(1, &_colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
        
        GLfloat texcoords[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f};
        
        glGenBuffers(1, &_texcoordBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _texcoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
        
        GLubyte indices[] = {0, 1, 2, 1, 3, 2};
        
        glGenBuffers(1, &_indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        _boundingBox.set(-width / 2.0f, -height / 2.0f, width, height);
    }

    Sprite::~Sprite()
    {
        glDeleteVertexArrays(1, &_vertexArray);
        glDeleteBuffers(1, &_vertexBuffer);
        glDeleteBuffers(1, &_colorBuffer);
        glDeleteBuffers(1, &_texcoordBuffer);
        glDeleteBuffers(1, &_indexBuffer);
    }

    void Sprite::draw()
    {
        _engine->getRenderer()->activateShader(_shader);
        
        GLint uniProj = _shader->getVertexShaderConstantId("proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, _engine->getRenderer()->getProjection().m);
        
        GLint uniView = _shader->getVertexShaderConstantId("view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, _engine->getScene()->getCamera()->getTransform().m);
        
        GLint uniModel = _shader->getVertexShaderConstantId("model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, _transform.m);
        
        if (static_cast<RendererOGL*>(_engine->getRenderer())->checkOpenGLErrors())
        {
            return;
        }
        
        _engine->getRenderer()->activateTexture(_texture, 0);
        
        if (static_cast<RendererOGL*>(_engine->getRenderer())->checkOpenGLErrors())
        {
            return;
        }
        
        glBindVertexArray(_vertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        
        /*_engine->getRenderer()->activateTexture(_texture, 0);
        
        GLfloat width = static_cast<GLfloat>(_texture->getWidth());
        GLfloat height = static_cast<GLfloat>(_texture->getHeight());
        
        _engine->getRenderer()->drawQuad(Rectangle(-width / 2.0f, -height / 2.0f, width, height), Vector3(1.0f, 1.0f, 1.0f), _transform);*/
        
        Node::draw();
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
