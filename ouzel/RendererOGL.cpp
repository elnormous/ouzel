// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGL.h"
#include "TextureOGL.h"
#include "RenderTargetOGL.h"
#include "ShaderOGL.h"
#include "MeshBufferOGL.h"
#include "Engine.h"
#include "Utils.h"

#if defined(SUPPORTS_OPENGL)
#include "ColorPSOGL.h"
#include "ColorVSOGL.h"
#include "TexturePSOGL.h"
#include "TextureVSOGL.h"
#endif

#if defined(SUPPORTS_OPENGLES)
#include "ColorPSOGLES.h"
#include "ColorVSOGLES.h"
#include "TexturePSOGLES.h"
#include "TextureVSOGLES.h"
#endif

namespace ouzel
{
    RendererOGL::RendererOGL(const Size2& size, bool resiazble, bool fullscreen):
        Renderer(size, resiazble, fullscreen, Driver::OPENGL)
    {
        recalculateProjection();
    }
    
    RendererOGL::~RendererOGL()
    {
        
    }
    
    bool RendererOGL::initOpenGL(uint32_t width, uint32_t height)
    {
        _size.width = static_cast<float>(width);
        _size.height = static_cast<float>(height);
        
        //glEnable(GL_DEPTH_TEST);
        glClearColor(_clearColor.getR(), _clearColor.getG(), _clearColor.getB(), _clearColor.getA());
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        //glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        
        // precomputed alpha
        //glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        //glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        Shader* textureShader = loadShaderFromBuffers(TEXTURE_PIXEL_SHADER_OGL, sizeof(TEXTURE_PIXEL_SHADER_OGL), TEXTURE_VERTEX_SHADER_OGL, sizeof(TEXTURE_VERTEX_SHADER_OGL), VertexPCT::ATTRIBUTES);
        if (textureShader)
        {
            _shaders[SHADER_TEXTURE] = textureShader;
        }
        
        Shader* colorShader = loadShaderFromBuffers(COLOR_PIXEL_SHADER_OGL, sizeof(COLOR_PIXEL_SHADER_OGL), COLOR_VERTEX_SHADER_OGL, sizeof(COLOR_VERTEX_SHADER_OGL), VertexPC::ATTRIBUTES);
        if (colorShader)
        {
            _shaders[SHADER_COLOR] = colorShader;
        }
        
        _ready = true;
        
        recalculateProjection();
        
        Engine::getInstance()->begin();
        
        return true;
    }
    
    bool RendererOGL::checkOpenGLErrors()
    {
        bool error = false;
        
        while (GLenum error = glGetError() != GL_NO_ERROR)
        {
            const char* errorStr = "Unknown error";
            
            switch (error)
            {
                case GL_INVALID_ENUM: errorStr = "GL_INVALID_ENUM"; break;
                case GL_INVALID_VALUE: errorStr = "GL_INVALID_VALUE"; break;
                case GL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break;
                case GL_OUT_OF_MEMORY: errorStr = "GL_OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            
            log("OpenGL error: %s (%x)", errorStr, error);
            
            error = true;
        }
        
        return error;
    }
    
    void RendererOGL::setClearColor(Color color)
    {
        Renderer::setClearColor(color);
        
        glClearColor(_clearColor.getR(), _clearColor.getG(), _clearColor.getB(), _clearColor.getA());
    }

    void RendererOGL::recalculateProjection()
    {
        Renderer::recalculateProjection();
        
        if (_ready)
        {
            glViewport(0, 0, _size.width, _size.height);
        }
    }
    
    void RendererOGL::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkOpenGLErrors();
    }
    
    void RendererOGL::flush()
    {
        glFlush();
        checkOpenGLErrors();
    }
    
    Texture* RendererOGL::loadTextureFromFile(const std::string& filename)
    {
        TextureOGL* texture = new TextureOGL();
        
        if (!texture->initFromFile(filename))
        {
            delete texture;
            texture = nullptr;
        }
        
        return texture;
    }
    
    Shader* RendererOGL::loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes)
    {
        ShaderOGL* shader = new ShaderOGL();
        
        if (!shader->initFromFiles(fragmentShader, vertexShader, vertexAttributes))
        {
            delete shader;
            shader = nullptr;
        }
        
        return shader;
    }
    
    Shader* RendererOGL::loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes)
    {
        ShaderOGL* shader = new ShaderOGL();
        
        if (!shader->initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize, vertexAttributes))
        {
            delete shader;
            shader = nullptr;
        }
        
        return shader;
    }
    
    MeshBuffer* RendererOGL::createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes)
    {
        MeshBufferOGL* meshBuffer = new MeshBufferOGL();
        
        if (!meshBuffer->initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexSize, vertexCount, dynamicVertexBuffer, vertexAttributes))
        {
            delete meshBuffer;
            meshBuffer = nullptr;
        }
        
        return meshBuffer;
    }
    
    bool RendererOGL::drawMeshBuffer(MeshBuffer* meshBuffer)
    {
        if (!Renderer::drawMeshBuffer(meshBuffer))
        {
            return false;
        }
        
        ShaderOGL* shaderOGL = static_cast<ShaderOGL*>(_activeShader.item);
        
        if (!shaderOGL)
        {
            return false;
        }
        
        glUseProgram(shaderOGL->getProgramId());
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        for (uint32_t layer = 0; layer < TEXTURE_LAYERS; ++layer)
        {
            glActiveTexture(GL_TEXTURE0 + layer);
            
            TextureOGL* textureOGL = static_cast<TextureOGL*>(_activeTextures[layer].item);
            
            glBindTexture(GL_TEXTURE_2D, textureOGL ? textureOGL->getTextureId() : 0);
        }
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        MeshBufferOGL* meshBufferOGL = static_cast<MeshBufferOGL*>(meshBuffer);
        
        glBindVertexArray(meshBufferOGL->getVertexArrayId());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBufferOGL->getIndexBufferId());
        glDrawElements(GL_TRIANGLES, meshBufferOGL->getIndexCount(), meshBufferOGL->getIndexFormat(), nullptr);
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        return true;
    }
    
    bool RendererOGL::drawLine(const Vector2& start, const Vector2& finish, const Color& color, const Matrix4& transform)
    {
        GLubyte indices[] = {0, 1};
        
        VertexPC vertices[] = {
            VertexPC(Vector3(start.x, start.y, 0.0f), color),
            VertexPC(Vector3(finish.x, finish.y, 0.0f), color)
        };
        
        AutoPtr<MeshBufferOGL> meshBufferOGL = static_cast<MeshBufferOGL*>(createMeshBuffer(indices, sizeof(uint8_t), 2, false, vertices, sizeof(VertexPC), 4, false, VertexPC::ATTRIBUTES));
        
        if (!meshBufferOGL)
        {
            return false;
        }
        
        ShaderOGL* colorShader = static_cast<ShaderOGL*>(getShader(SHADER_COLOR));
        
        if (!colorShader)
        {
            return false;
        }
        
        glUseProgram(colorShader->getProgramId());
        
        uint32_t uniModelViewProj = colorShader->getVertexShaderConstantId("modelViewProj");
        colorShader->setVertexShaderConstant(uniModelViewProj, { transform });
        
        glBindVertexArray(meshBufferOGL->getVertexArrayId());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBufferOGL->getIndexBufferId());
        glDrawElements(GL_LINE_STRIP, meshBufferOGL->getIndexCount(), meshBufferOGL->getIndexFormat(), nullptr);
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        return true;
    }
    
    bool RendererOGL::drawRectangle(const Rectangle& rectangle, const Color& color, const Matrix4& transform)
    {
        GLubyte indices[] = {0, 1, 3, 2, 0};
        
        VertexPC vertices[] = {
            VertexPC(Vector3(rectangle.x, rectangle.y, 0.0f), color),
            VertexPC(Vector3(rectangle.x + rectangle.width, rectangle.y, 0.0f), color),
            VertexPC(Vector3(rectangle.x, rectangle.y + rectangle.height, 0.0f), color),
            VertexPC(Vector3(rectangle.x + rectangle.width, rectangle.y + rectangle.height, 0.0f), color)
        };
        
        AutoPtr<MeshBufferOGL> meshBufferOGL = static_cast<MeshBufferOGL*>(createMeshBuffer(indices, sizeof(uint8_t), 5, false, vertices, sizeof(VertexPC), 4, false, VertexPC::ATTRIBUTES));
        
        if (!meshBufferOGL)
        {
            return false;
        }
        
        ShaderOGL* colorShader = static_cast<ShaderOGL*>(getShader(SHADER_COLOR));
        
        if (!colorShader)
        {
            return false;
        }
        
        glUseProgram(colorShader->getProgramId());
        
        uint32_t uniModelViewProj = colorShader->getVertexShaderConstantId("modelViewProj");
        colorShader->setVertexShaderConstant(uniModelViewProj, { transform });
        
        glBindVertexArray(meshBufferOGL->getVertexArrayId());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBufferOGL->getIndexBufferId());
        glDrawElements(GL_LINE_STRIP, meshBufferOGL->getIndexCount(), meshBufferOGL->getIndexFormat(), nullptr);
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        return true;
    }
    
    bool RendererOGL::drawQuad(const Rectangle& rectangle, const Color& color, const Matrix4& transform)
    {
        GLubyte indices[] = {0, 1, 2, 1, 3, 2};
        
        VertexPCT vertices[] = {
            VertexPCT(Vector3(rectangle.x, rectangle.y, 0.0f), color, Vector2(0.0f, 1.0f)),
            VertexPCT(Vector3(rectangle.x + rectangle.width, rectangle.y, 0.0f), color, Vector2(1.0f, 1.0f)),
            VertexPCT(Vector3(rectangle.x, rectangle.y + rectangle.height, 0.0f), color, Vector2(0.0f, 0.0f)),
            VertexPCT(Vector3(rectangle.x + rectangle.width, rectangle.y + rectangle.height, 0.0f), color, Vector2(1.0f, 0.0f))
        };
        
        AutoPtr<MeshBufferOGL> meshBufferOGL = static_cast<MeshBufferOGL*>(createMeshBuffer(indices, sizeof(uint8_t), 6, false, vertices, sizeof(VertexPCT), 4, false, VertexPCT::ATTRIBUTES));
        
        if (!meshBufferOGL)
        {
            return false;
        }
        
        ShaderOGL* textureShader = static_cast<ShaderOGL*>(getShader(SHADER_TEXTURE));
        
        if (!textureShader)
        {
            return false;
        }
        
        glUseProgram(textureShader->getProgramId());
        
        uint32_t uniModelViewProj = textureShader->getVertexShaderConstantId("modelViewProj");
        textureShader->setVertexShaderConstant(uniModelViewProj, { transform });
        
        glBindVertexArray(meshBufferOGL->getVertexArrayId());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBufferOGL->getIndexBufferId());
        glDrawElements(GL_TRIANGLES, meshBufferOGL->getIndexCount(), meshBufferOGL->getIndexFormat(), nullptr);
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        return true;
    }
}
