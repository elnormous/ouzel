// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGL.h"
#include "TextureOGL.h"
#include "RenderTargetOGL.h"
#include "ShaderOGL.h"
#include "MeshBufferOGL.h"
#include "Engine.h"
#include "Utils.h"
#include "stb_image_write.h"

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

#if defined(OUZEL_PLATFORM_ANDROID)
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT = 0;
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT = 0;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT = 0;
#endif

namespace ouzel
{
    RendererOGL::RendererOGL()
    {
#if defined(OUZEL_PLATFORM_ANDROID)
        glGenVertexArraysOESEXT = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
        glBindVertexArrayOESEXT = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
        glDeleteVertexArraysOESEXT = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
#endif
    }
    
    RendererOGL::~RendererOGL()
    {
        
    }
    
    bool RendererOGL::initOpenGL(uint32_t width, uint32_t height)
    {
        Size2 size(static_cast<float>(width), static_cast<float>(height));
        
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
        
        ShaderPtr textureShader = loadShaderFromBuffers(TEXTURE_PIXEL_SHADER_OGL, sizeof(TEXTURE_PIXEL_SHADER_OGL), TEXTURE_VERTEX_SHADER_OGL, sizeof(TEXTURE_VERTEX_SHADER_OGL), VertexPCT::ATTRIBUTES);
        if (textureShader)
        {
            _shaders[SHADER_TEXTURE] = textureShader;
        }
        
        ShaderPtr colorShader = loadShaderFromBuffers(COLOR_PIXEL_SHADER_OGL, sizeof(COLOR_PIXEL_SHADER_OGL), COLOR_VERTEX_SHADER_OGL, sizeof(COLOR_VERTEX_SHADER_OGL), VertexPC::ATTRIBUTES);
        if (colorShader)
        {
            _shaders[SHADER_COLOR] = colorShader;
        }
        
        _ready = true;
        
        resize(size);
        
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

    void RendererOGL::resize(const Size2& size)
    {
        Renderer::resize(size);
        
        if (_ready)
        {
            glViewport(0, 0,
                       static_cast<GLsizei>(_size.width),
                       static_cast<GLsizei>(_size.height));
        }
    }
    
    void RendererOGL::clear()
    {
        if (_activeRenderTarget)
        {
            std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(_activeRenderTarget);
            
            glBindFramebuffer(GL_FRAMEBUFFER, renderTargetOGL->getFrameBufferId());
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkOpenGLErrors();
    }
    
    void RendererOGL::flush()
    {
        if (_activeRenderTarget)
        {
            std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(_activeRenderTarget);
            
            glBindFramebuffer(GL_FRAMEBUFFER, renderTargetOGL->getFrameBufferId());
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        glFlush();
        checkOpenGLErrors();
    }
    
    TexturePtr RendererOGL::createTexture(Size2 const& size, bool dynamic)
    {
        std::shared_ptr<TextureOGL> texture(new TextureOGL());
        texture->init(size, dynamic);
        
        return texture;
    }
    
    TexturePtr RendererOGL::loadTextureFromFile(const std::string& filename, bool dynamic)
    {
        std::shared_ptr<TextureOGL> texture(new TextureOGL());
        
        if (!texture->initFromFile(filename, dynamic))
        {
            texture.reset();
        }
        
        return texture;
    }
    
    TexturePtr RendererOGL::loadTextureFromData(const void* data, const Size2& size, bool dynamic)
    {
        std::shared_ptr<TextureOGL> texture(new TextureOGL());
        
        if (!texture->initFromData(data, size, dynamic))
        {
            texture.reset();
        }
        
        return texture;
    }
    
    ShaderPtr RendererOGL::loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes)
    {
        std::shared_ptr<ShaderOGL> shader(new ShaderOGL());
        
        if (!shader->initFromFiles(fragmentShader, vertexShader, vertexAttributes))
        {
            shader.reset();
        }
        
        return shader;
    }
    
    ShaderPtr RendererOGL::loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes)
    {
        std::shared_ptr<ShaderOGL> shader(new ShaderOGL());
        
        if (!shader->initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize, vertexAttributes))
        {
            shader.reset();
        }
        
        return shader;
    }
    
    MeshBufferPtr RendererOGL::createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes)
    {
        std::shared_ptr<MeshBufferOGL> meshBuffer(new MeshBufferOGL());
        
        if (!meshBuffer->initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexSize, vertexCount, dynamicVertexBuffer, vertexAttributes))
        {
            meshBuffer.reset();
        }
        
        return meshBuffer;
    }
    
    bool RendererOGL::drawMeshBuffer(MeshBufferPtr const& meshBuffer)
    {
        if (!Renderer::drawMeshBuffer(meshBuffer))
        {
            return false;
        }
        
        if (_activeShader)
        {
            if (_activeRenderTarget)
            {
                std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(_activeRenderTarget);
                
                glBindFramebuffer(GL_FRAMEBUFFER, renderTargetOGL->getFrameBufferId());
            }
            else
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            
            std::shared_ptr<ShaderOGL> shaderOGL = std::static_pointer_cast<ShaderOGL>(_activeShader);
            
            glUseProgram(shaderOGL->getProgramId());
            
            if (checkOpenGLErrors())
            {
                return false;
            }
            
            for (uint32_t layer = 0; layer < TEXTURE_LAYERS; ++layer)
            {
                glActiveTexture(GL_TEXTURE0 + layer);
                
                if (_activeTextures[layer])
                {
                    std::shared_ptr<TextureOGL> textureOGL = std::static_pointer_cast<TextureOGL>(_activeTextures[layer]);
                
                    glBindTexture(GL_TEXTURE_2D, textureOGL->getTextureId());
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            }
            
            if (checkOpenGLErrors())
            {
                return false;
            }
            
            std::shared_ptr<MeshBufferOGL> meshBufferOGL = std::static_pointer_cast<MeshBufferOGL>(meshBuffer);
            
            glBindVertexArray(meshBufferOGL->getVertexArrayId());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBufferOGL->getIndexBufferId());
            glDrawElements(GL_TRIANGLES, meshBufferOGL->getIndexCount(), meshBufferOGL->getIndexFormat(), nullptr);
            
            if (checkOpenGLErrors())
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
    
    bool RendererOGL::drawLine(const Vector2& start, const Vector2& finish, const Color& color, const Matrix4& transform)
    {
        if (!Renderer::drawLine(start, finish, color, transform))
        {
            return false;
        }
        
        if (_activeRenderTarget)
        {
            std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(_activeRenderTarget);
            
            glBindFramebuffer(GL_FRAMEBUFFER, renderTargetOGL->getFrameBufferId());
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        GLubyte indices[] = {0, 1};
        
        VertexPC vertices[] = {
            VertexPC(Vector3(start.x, start.y, 0.0f), color),
            VertexPC(Vector3(finish.x, finish.y, 0.0f), color)
        };
        
        std::shared_ptr<MeshBufferOGL> meshBufferOGL = std::static_pointer_cast<MeshBufferOGL>(createMeshBuffer(indices, sizeof(uint8_t), 2, false, vertices, sizeof(VertexPC), 4, false, VertexPC::ATTRIBUTES));
        
        if (!meshBufferOGL)
        {
            return false;
        }
        
        std::shared_ptr<ShaderOGL> colorShader = std::static_pointer_cast<ShaderOGL>(getShader(SHADER_COLOR));
        
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
        if (!Renderer::drawRectangle(rectangle, color, transform))
        {
            return false;
        }
        
        if (_activeRenderTarget)
        {
            std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(_activeRenderTarget);
            
            glBindFramebuffer(GL_FRAMEBUFFER, renderTargetOGL->getFrameBufferId());
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        GLubyte indices[] = {0, 1, 3, 2, 0};
        
        VertexPC vertices[] = {
            VertexPC(Vector3(rectangle.x, rectangle.y, 0.0f), color),
            VertexPC(Vector3(rectangle.x + rectangle.width, rectangle.y, 0.0f), color),
            VertexPC(Vector3(rectangle.x, rectangle.y + rectangle.height, 0.0f), color),
            VertexPC(Vector3(rectangle.x + rectangle.width, rectangle.y + rectangle.height, 0.0f), color)
        };
        
        std::shared_ptr<MeshBufferOGL> meshBufferOGL = std::static_pointer_cast<MeshBufferOGL>(createMeshBuffer(indices, sizeof(uint8_t), 5, false, vertices, sizeof(VertexPC), 4, false, VertexPC::ATTRIBUTES));
        
        if (!meshBufferOGL)
        {
            return false;
        }
        
        std::shared_ptr<ShaderOGL> colorShader = std::static_pointer_cast<ShaderOGL>(getShader(SHADER_COLOR));
        
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
        if (!Renderer::drawQuad(rectangle, color, transform))
        {
            return false;
        }
        
        if (_activeRenderTarget)
        {
            std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(_activeRenderTarget);
            
            glBindFramebuffer(GL_FRAMEBUFFER, renderTargetOGL->getFrameBufferId());
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        GLubyte indices[] = {0, 1, 2, 1, 3, 2};
        
        VertexPCT vertices[] = {
            VertexPCT(Vector3(rectangle.x, rectangle.y, 0.0f), color, Vector2(0.0f, 1.0f)),
            VertexPCT(Vector3(rectangle.x + rectangle.width, rectangle.y, 0.0f), color, Vector2(1.0f, 1.0f)),
            VertexPCT(Vector3(rectangle.x, rectangle.y + rectangle.height, 0.0f), color, Vector2(0.0f, 0.0f)),
            VertexPCT(Vector3(rectangle.x + rectangle.width, rectangle.y + rectangle.height, 0.0f), color, Vector2(1.0f, 0.0f))
        };
        
        std::shared_ptr<MeshBufferOGL> meshBufferOGL = std::static_pointer_cast<MeshBufferOGL>(createMeshBuffer(indices, sizeof(uint8_t), 6, false, vertices, sizeof(VertexPCT), 4, false, VertexPCT::ATTRIBUTES));
        
        if (!meshBufferOGL)
        {
            return false;
        }
        
        std::shared_ptr<ShaderOGL> textureShader = std::static_pointer_cast<ShaderOGL>(getShader(SHADER_TEXTURE));
        
        if (!textureShader)
        {
            return false;
        }
        
        glUseProgram(textureShader->getProgramId());
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        for (uint32_t layer = 0; layer < TEXTURE_LAYERS; ++layer)
        {
            glActiveTexture(GL_TEXTURE0 + layer);
            
            if (_activeTextures[layer])
            {
                std::shared_ptr<TextureOGL> textureOGL = std::static_pointer_cast<TextureOGL>(_activeTextures[layer]);
                
                glBindTexture(GL_TEXTURE_2D, textureOGL->getTextureId());
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
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
    
    bool RendererOGL::saveScreenshot(std::string const& filename)
    {
        if (!Renderer::saveScreenshot(filename))
        {
            return false;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        GLsizei width = static_cast<GLsizei>(_size.width);
        GLsizei height = static_cast<GLsizei>(_size.height);
        
        std::unique_ptr<uint8_t[]> data(new uint8_t[3 * width * height]);
        
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data.get());
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        int32_t w = width;
        int32_t h = height;
        uint8_t temp;
        int32_t depth = 3;
        
        for (int32_t row = 0; row < (h>>1); row++)
        {
            for (int32_t col = 0; col < w; col++)
            {
                for (int32_t z = 0; z < depth; z++) {
                    temp = data[(row * w + col) * depth + z];
                    data[(row * w + col) * depth + z] = data[((h - row - 1) * w + col) * depth + z];
                    data[((h - row - 1) * w + col) * depth + z] = temp;
                }
            }
        }
        
        if (!stbi_write_png(filename.c_str(), width, height, 3, data.get(), width * 3))
        {
            log("Failed to save image to file");
            return false;
        }
        
        return true;
    }
}
