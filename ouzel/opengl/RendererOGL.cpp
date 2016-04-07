// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGL.h"
#include "TextureOGL.h"
#include "RenderTargetOGL.h"
#include "ShaderOGL.h"
#include "MeshBufferOGL.h"
#include "BlendStateOGL.h"
#include "Engine.h"
#include "Window.h"
#include "Cache.h"
#include "Utils.h"
#include "stb_image_write.h"

#if defined(OUZEL_SUPPORTS_OPENGL)
#include "ColorPSOGL.h"
#include "ColorVSOGL.h"
#include "TexturePSOGL.h"
#include "TextureVSOGL.h"
#endif

#if defined(OUZEL_SUPPORTS_OPENGLES)
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
    namespace video
    {
        RendererOGL::RendererOGL():
            Renderer(Driver::OPENGL)
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

        bool RendererOGL::initOpenGL(GLuint framebuffer)
        {
            Size2 size = sharedEngine->getWindow()->getSize();

            _framebuffer = framebuffer;

            //glEnable(GL_DEPTH_TEST);
            glClearColor(_clearColor.getR(), _clearColor.getG(), _clearColor.getB(), _clearColor.getA());

            ShaderPtr textureShader = loadShaderFromBuffers(TEXTURE_PIXEL_SHADER_OGL, sizeof(TEXTURE_PIXEL_SHADER_OGL), TEXTURE_VERTEX_SHADER_OGL, sizeof(TEXTURE_VERTEX_SHADER_OGL), VertexPCT::ATTRIBUTES);

            if (!textureShader)
            {
                return false;
            }

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader = loadShaderFromBuffers(COLOR_PIXEL_SHADER_OGL, sizeof(COLOR_PIXEL_SHADER_OGL), COLOR_VERTEX_SHADER_OGL, sizeof(COLOR_VERTEX_SHADER_OGL), VertexPC::ATTRIBUTES);

            if (!colorShader)
            {
                return false;
            }

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            BlendStatePtr noBlendState = createBlendState(false,
                                                          BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                          BlendState::BlendOperation::ADD,
                                                          BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                          BlendState::BlendOperation::ADD);

            if (!noBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            BlendStatePtr alphaBlendState = createBlendState(true,
                                                             BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                                             BlendState::BlendOperation::ADD,
                                                             BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                             BlendState::BlendOperation::ADD);

            if (!alphaBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            _ready = true;

            setSize(size);

            sharedEngine->begin();

            return true;
        }

        bool RendererOGL::checkOpenGLErrors()
        {
            bool gotError = false;

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

                gotError = true;
            }

            return gotError;
        }

        void RendererOGL::setClearColor(Color color)
        {
            Renderer::setClearColor(color);

            glClearColor(_clearColor.getR(), _clearColor.getG(), _clearColor.getB(), _clearColor.getA());
        }

        void RendererOGL::setSize(const Size2& size)
        {
            Renderer::setSize(size);

            if (_ready)
            {
                glViewport(0, 0,
                           static_cast<GLsizei>(_size.width),
                           static_cast<GLsizei>(_size.height));
            }
        }

        void RendererOGL::clear()
        {
            Renderer::clear();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            checkOpenGLErrors();
        }

        void RendererOGL::flush()
        {
            Renderer::flush();

            glFlush();
            checkOpenGLErrors();
        }

        BlendStatePtr RendererOGL::createBlendState(bool enableBlending,
                                                    BlendState::BlendFactor colorBlendSource, BlendState::BlendFactor colorBlendDest,
                                                    BlendState::BlendOperation colorOperation,
                                                    BlendState::BlendFactor alphaBlendSource, BlendState::BlendFactor alphaBlendDest,
                                                    BlendState::BlendOperation alphaOperation)
        {
            std::shared_ptr<BlendStateOGL> blendState(new BlendStateOGL());

            if (!blendState->init(enableBlending,
                                  colorBlendSource, colorBlendDest,
                                  colorOperation,
                                  alphaBlendSource, alphaBlendDest,
                                  alphaOperation))
            {
                blendState.reset();
            }

            return blendState;
        }

        bool RendererOGL::activateBlendState(BlendStatePtr blendState)
        {
            if (_activeBlendState == blendState)
            {
                return true;
            }

            if (!Renderer::activateBlendState(blendState))
            {
                return false;
            }

            if (blendState)
            {
                std::shared_ptr<BlendStateOGL> blendStateOGL = std::static_pointer_cast<BlendStateOGL>(_activeBlendState);

                if (blendStateOGL->isBlendingEnabled())
                {
                    glEnable(GL_BLEND);
                }
                else
                {
                    glDisable(GL_BLEND);
                }

                glBlendEquationSeparate(BlendStateOGL::getBlendOperation(blendStateOGL->getColorOperation()),
                                        BlendStateOGL::getBlendOperation(blendStateOGL->getAlphaOperation()));

                glBlendFuncSeparate(BlendStateOGL::getBlendFactor(blendStateOGL->getColorBlendSource()),
                                    BlendStateOGL::getBlendFactor(blendStateOGL->getColorBlendDest()),
                                    BlendStateOGL::getBlendFactor(blendStateOGL->getAlphaBlendSource()),
                                    BlendStateOGL::getBlendFactor(blendStateOGL->getAlphaBlendDest()));
            }
            else
            {
                glDisable(GL_BLEND);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
            }

            if (checkOpenGLErrors())
            {
                return false;
            }

            return true;
        }

        TexturePtr RendererOGL::createTexture(const Size2& size, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureOGL> texture(new TextureOGL());
            texture->init(size, dynamic, mipmaps);

            return texture;
        }

        TexturePtr RendererOGL::loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureOGL> texture(new TextureOGL());

            if (!texture->initFromFile(filename, dynamic, mipmaps))
            {
                texture.reset();
            }

            return texture;
        }

        TexturePtr RendererOGL::loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureOGL> texture(new TextureOGL());

            if (!texture->initFromData(data, size, dynamic, mipmaps))
            {
                texture.reset();
            }

            return texture;
        }

        bool RendererOGL::activateTexture(const TexturePtr& texture, uint32_t layer)
        {
            if (_activeTextures[layer] == texture)
            {
                return true;
            }

            if (!Renderer::activateTexture(texture, layer))
            {
                return false;
            }

            if (_activeTextures[layer])
            {
                std::shared_ptr<TextureOGL> textureOGL = std::static_pointer_cast<TextureOGL>(_activeTextures[layer]);

                bindTexture(textureOGL->getTextureId(), layer);
            }
            else
            {
                bindTexture(0, layer);
            }

            return true;
        }

        RenderTargetPtr RendererOGL::createRenderTarget(const Size2& size, bool depthBuffer)
        {
            std::shared_ptr<RenderTargetOGL> renderTarget(new RenderTargetOGL());

            if (!renderTarget->init(size, depthBuffer))
            {
                renderTarget.reset();
            }

            return renderTarget;
        }

        bool RendererOGL::activateRenderTarget(const RenderTargetPtr& renderTarget)
        {
            if (_activeRenderTarget == renderTarget)
            {
                return true;
            }

            if (!Renderer::activateRenderTarget(renderTarget))
            {
                return false;
            }

            if (_activeRenderTarget)
            {
                std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(_activeRenderTarget);

                bindFramebuffer(renderTargetOGL->getFrameBufferId());
            }
            else
            {
                bindFramebuffer(_framebuffer);
            }

            return true;
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

        bool RendererOGL::activateShader(const ShaderPtr& shader)
        {
            if (_activeShader == shader)
            {
                return true;
            }

            if (!Renderer::activateShader(shader))
            {
                return false;
            }

            if (_activeShader)
            {
                std::shared_ptr<ShaderOGL> shaderOGL = std::static_pointer_cast<ShaderOGL>(_activeShader);

                bindProgram(shaderOGL->getProgramId());
            }
            else
            {
                bindProgram(0);
            }

            return true;
        }

        MeshBufferPtr RendererOGL::createMeshBuffer()
        {
            std::shared_ptr<MeshBufferOGL> meshBuffer(new MeshBufferOGL());

            if (!meshBuffer->init())
            {
                meshBuffer.reset();
            }

            return meshBuffer;
        }

        MeshBufferPtr RendererOGL::createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer)
        {
            std::shared_ptr<MeshBufferOGL> meshBuffer(new MeshBufferOGL());

            if (!meshBuffer->initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexAttributes, vertexCount, dynamicVertexBuffer))
            {
                meshBuffer.reset();
            }

            return meshBuffer;
        }

        bool RendererOGL::drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount, DrawMode drawMode)
        {
            if (!Renderer::drawMeshBuffer(meshBuffer))
            {
                return false;
            }

            if (_activeRenderTarget)
            {
                std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(_activeRenderTarget);

                bindFramebuffer(renderTargetOGL->getFrameBufferId());
            }
            else
            {
                bindFramebuffer(_framebuffer);
            }

            for (uint32_t layer = 0; layer < TEXTURE_LAYERS; ++layer)
            {
                if (_activeTextures[layer])
                {
                    std::shared_ptr<TextureOGL> textureOGL = std::static_pointer_cast<TextureOGL>(_activeTextures[layer]);

                    bindTexture(textureOGL->getTextureId(), layer);
                }
                else
                {
                    bindTexture(0, layer);
                }
            }

            if (_activeShader)
            {
                std::shared_ptr<ShaderOGL> shaderOGL = std::static_pointer_cast<ShaderOGL>(_activeShader);

                bindProgram(shaderOGL->getProgramId());
            }
            else
            {
                bindProgram(0);
            }

            std::shared_ptr<MeshBufferOGL> meshBufferOGL = std::static_pointer_cast<MeshBufferOGL>(meshBuffer);

            if (indexCount == 0)
            {
                indexCount = meshBufferOGL->getIndexCount();
            }

            GLenum mode;

            switch (drawMode)
            {
                case DrawMode::POINT_LIST: mode = GL_POINTS; break;
                case DrawMode::LINE_LIST: mode = GL_LINES; break;
                case DrawMode::LINE_STRIP: mode = GL_LINE_STRIP; break;
                case DrawMode::TRIANGLE_LIST: mode = GL_TRIANGLES; break;
                case DrawMode::TRIANGLE_STRIP: mode = GL_TRIANGLE_STRIP; break;
            }

            glBindVertexArray(meshBufferOGL->getVertexArrayId());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBufferOGL->getIndexBufferId());
            glDrawElements(mode, static_cast<GLsizei>(indexCount), meshBufferOGL->getIndexFormat(), nullptr);

            if (checkOpenGLErrors())
            {
                return false;
            }

            return true;
        }

        bool RendererOGL::saveScreenshot(const std::string& filename)
        {
            if (!Renderer::saveScreenshot(filename))
            {
                return false;
            }

            bindFramebuffer(_framebuffer);

            GLsizei width = static_cast<GLsizei>(_size.width);
            GLsizei height = static_cast<GLsizei>(_size.height);

            std::unique_ptr<uint8_t[]> data(new uint8_t[3 * width * height]);

            glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data.get());

            if (checkOpenGLErrors())
            {
                return false;
            }

            uint32_t w = static_cast<uint32_t>(width);
            uint32_t h = static_cast<uint32_t>(height);
            uint8_t temp;
            uint32_t depth = 3;

            for (uint32_t row = 0; row < (h>>1); row++)
            {
                for (uint32_t col = 0; col < w; col++)
                {
                    for (uint32_t z = 0; z < depth; z++)
                    {
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

        GLuint RendererOGL::_currentTextureId[TEXTURE_LAYERS] = { 0 };
        GLuint RendererOGL::_currentProgramId = 0;
        GLuint RendererOGL::_currentFrambufferId = 0;

        bool RendererOGL::bindTexture(GLuint textureId, uint32_t layer)
        {
            if (_currentTextureId[layer] != textureId)
            {
                glActiveTexture(GL_TEXTURE0 + layer);
                glBindTexture(GL_TEXTURE_2D, textureId);
                _currentTextureId[layer] = textureId;

                if (checkOpenGLErrors())
                {
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::bindProgram(GLuint programId)
        {
            if (_currentProgramId != programId)
            {
                glUseProgram(programId);
                _currentProgramId = programId;

                if (checkOpenGLErrors())
                {
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::bindFramebuffer(GLuint framebufferId)
        {
            if (_currentFrambufferId != framebufferId)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
                _currentFrambufferId = framebufferId;

                if (checkOpenGLErrors())
                {
                    return false;
                }
            }

            return true;
        }

    } // namespace video
} // namespace ouzel
