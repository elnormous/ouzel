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
    
    bool Renderer::initOpenGL(uint32_t width, uint32_t height)
    {
        //glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        
        glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        //glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        
        // precomputed alpha
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        Shader* textureShader = new Shader("texture.fsh", "texture.vsh");
        _shaders[SHADER_TEXTURE] = textureShader;
        
        Shader* colorShader = new Shader("color.fsh", "color.vsh");
        _shaders[SHADER_COLOR] = colorShader;
        
        resize(width, height);
        
        return true;
    }

    void Renderer::resize(uint32_t width, uint32_t height)
    {
        _width = width;
        _height = height;
        
        glViewport(0, 0, _width, _height);
        
        Matrix4::createOrthographic(_width, _height, 1.0f, 1000.0f, &_projection);
    }
    
    void Renderer::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkOpenGLErrors();
    }
    
    void Renderer::flush()
    {
        glFlush();
        checkOpenGLErrors();
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
    
    void Renderer::drawLine(const Vector2& start, const Vector2& finish, const Vector3& color, const Matrix4& transform)
    {
        GLuint vertexArray = 0;
        GLuint vertexBuffer = 0;
        GLuint colorBuffer = 0;
        GLuint indexBuffer = 0;
        
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        
        GLfloat vertices[] = {
            start.x, start.y, -10.0f, 1.0f,
            finish.x, finish.y, -10.0f, 1.0f};
        
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        
        GLfloat colors[] = {
            color.x, color.y, color.z, 1.0f,
            color.x, color.y, color.z, 1.0f
        };
        
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
        
        GLubyte indices[] = {0, 1};
        
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        Shader* colorShader = getShader(SHADER_COLOR);
        
        glUseProgram(colorShader->getProgram());
        
        GLint uniProj = glGetUniformLocation(colorShader->getProgram(), "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, _engine->getRenderer()->getProjection().m);
        
        GLint uniView = glGetUniformLocation(colorShader->getProgram(), "view");
        
        if (_engine->getScene()->getCamera())
        {
            glUniformMatrix4fv(uniView, 1, GL_FALSE, _engine->getScene()->getCamera()->getTransform().m);
        }
        else
        {
            Matrix4 temp;
            glUniformMatrix4fv(uniView, 1, GL_FALSE, temp.m);
        }
        
        GLint uniModel = glGetUniformLocation(colorShader->getProgram(), "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, transform.m);
        
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_BYTE, nullptr);
        
        // delete buffers
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &colorBuffer);
        glDeleteBuffers(1, &indexBuffer);
    }
    
    void Renderer::drawRectangle(const Rectangle& rectangle, const Vector3& color, const Matrix4& transform)
    {
        GLuint vertexArray = 0;
        GLuint vertexBuffer = 0;
        GLuint colorBuffer = 0;
        GLuint indexBuffer = 0;
        
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        
        GLfloat vertices[] = {
            rectangle.x, rectangle.y, -10.0f, 1.0f,
            rectangle.x + rectangle.width, rectangle.y, -10.0f, 1.0f,
            rectangle.x, rectangle.y + rectangle.height, -10.0f, 1.0f,
            rectangle.x + rectangle.width, rectangle.y + rectangle.height, -10.0f, 1.0f};
        
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        
        GLfloat colors[] = {
            color.x, color.y, color.z, 1.0f,
            color.x, color.y, color.z, 1.0f,
            color.x, color.y, color.z, 1.0f,
            color.x, color.y, color.z, 1.0f,
        };
        
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
        
        GLubyte indices[] = {0, 1, 3, 2, 0};
        
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        Shader* colorShader = getShader(SHADER_COLOR);
        
        glUseProgram(colorShader->getProgram());
        
        GLint uniProj = glGetUniformLocation(colorShader->getProgram(), "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, _engine->getRenderer()->getProjection().m);
        
        GLint uniView = glGetUniformLocation(colorShader->getProgram(), "view");
        
        if (_engine->getScene()->getCamera())
        {
            glUniformMatrix4fv(uniView, 1, GL_FALSE, _engine->getScene()->getCamera()->getTransform().m);
        }
        else
        {
            Matrix4 temp;
            glUniformMatrix4fv(uniView, 1, GL_FALSE, temp.m);
        }
        
        GLint uniModel = glGetUniformLocation(colorShader->getProgram(), "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, transform.m);
        
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_BYTE, nullptr);
        
        // delete buffers
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &colorBuffer);
        glDeleteBuffers(1, &indexBuffer);
    }
    
    void Renderer::drawQuad(const Rectangle& rectangle, const Vector3& color, const Matrix4& transform)
    {
        GLuint vertexArray = 0;
        GLuint vertexBuffer = 0;
        GLuint colorBuffer = 0;
        GLuint texcoordBuffer = 0;
        GLuint indexBuffer = 0;
        
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        
        GLfloat vertices[] = {
            rectangle.x, rectangle.y, -10.0f, 1.0f,
            rectangle.x + rectangle.width, rectangle.y, -10.0f, 1.0f,
            rectangle.x, rectangle.y + rectangle.height, -10.0f, 1.0f,
            rectangle.x + rectangle.width, rectangle.y + rectangle.height, -10.0f, 1.0f};
        
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        
        GLfloat colors[] = {
            color.x, color.y, color.z, 1.0f,
            color.x, color.y, color.z, 1.0f,
            color.x, color.y, color.z, 1.0f,
            color.x, color.y, color.z, 1.0f,
        };
        
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
        
        GLfloat texcoords[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f};
        
        glGenBuffers(1, &texcoordBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, texcoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
        
        GLubyte indices[] = {0, 1, 2, 1, 3, 2};
        
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        Shader* colorShader = getShader(SHADER_TEXTURE);
        
        glUseProgram(colorShader->getProgram());
        
        GLint uniProj = glGetUniformLocation(colorShader->getProgram(), "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, _engine->getRenderer()->getProjection().m);
        
        GLint uniView = glGetUniformLocation(colorShader->getProgram(), "view");
        
        if (_engine->getScene()->getCamera())
        {
            glUniformMatrix4fv(uniView, 1, GL_FALSE, _engine->getScene()->getCamera()->getTransform().m);
        }
        else
        {
            Matrix4 temp;
            glUniformMatrix4fv(uniView, 1, GL_FALSE, temp.m);
        }
        
        GLint uniModel = glGetUniformLocation(colorShader->getProgram(), "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, transform.m);
        
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        
        // delete buffers
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &colorBuffer);
        glDeleteBuffers(1, &texcoordBuffer);
        glDeleteBuffers(1, &indexBuffer);
    }
}
