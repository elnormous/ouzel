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
#include "ColorPSOGL2.h"
#include "ColorVSOGL2.h"
#include "TexturePSOGL2.h"
#include "TextureVSOGL2.h"
    #if defined(OUZEL_SUPPORTS_OPENGL3)
    #include "ColorPSOGL3.h"
    #include "ColorVSOGL3.h"
    #include "TexturePSOGL3.h"
    #include "TextureVSOGL3.h"
    #endif
#endif

#if defined(OUZEL_SUPPORTS_OPENGLES)
#include "ColorPSOGLES2.h"
#include "ColorVSOGLES2.h"
#include "TexturePSOGLES2.h"
#include "TextureVSOGLES2.h"
    #if defined(OUZEL_SUPPORTS_OPENGLES3)
    #include "ColorPSOGLES3.h"
    #include "ColorVSOGLES3.h"
    #include "TexturePSOGLES3.h"
    #include "TextureVSOGLES3.h"
    #endif
#endif

#if defined(OUZEL_PLATFORM_ANDROID)
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT = nullptr;
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT = nullptr;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT = nullptr;
#endif

namespace ouzel
{
    namespace graphics
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

        bool RendererOGL::init(const Size2& newSize, bool newFullscreen, uint32_t newSampleCount, TextureFiltering newTextureFiltering)
        {
            if (!Renderer::init(newSize, newFullscreen, newSampleCount, newTextureFiltering))
            {
                return false;
            }

            if (sampleCount > 1)
            {
                log("Multisample anti-aliasing is disabled for OpenGL");
            }

            //glEnable(GL_DEPTH_TEST);
            glClearColor(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());

            ShaderPtr textureShader;

            switch (apiVersion)
            {
                case 2:
                    textureShader = loadShaderFromBuffers(TEXTURE_PIXEL_SHADER_OGL2, sizeof(TEXTURE_PIXEL_SHADER_OGL2), TEXTURE_VERTEX_SHADER_OGL2, sizeof(TEXTURE_VERTEX_SHADER_OGL2), VertexPCT::ATTRIBUTES);
                    break;
#if defined(OUZEL_SUPPORTS_OPENGL3) || defined(OUZEL_SUPPORTS_OPENGLES3)
                case 3:
                    textureShader = loadShaderFromBuffers(TEXTURE_PIXEL_SHADER_OGL3, sizeof(TEXTURE_PIXEL_SHADER_OGL3), TEXTURE_VERTEX_SHADER_OGL3, sizeof(TEXTURE_VERTEX_SHADER_OGL3), VertexPCT::ATTRIBUTES);
                    break;
#endif
                default:
                    log("Unsupported OpenGL version");
                    return false;
            }

            if (!textureShader)
            {
                return false;
            }

            textureShader->setVertexShaderConstantInfo({{"modelViewProj", sizeof(Matrix4)}});
            textureShader->setPixelShaderConstantInfo({{"color", 4 * sizeof(float)}}, 256);

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader;

            switch (apiVersion)
            {
                case 2:
                    colorShader = loadShaderFromBuffers(COLOR_PIXEL_SHADER_OGL2, sizeof(COLOR_PIXEL_SHADER_OGL2), COLOR_VERTEX_SHADER_OGL2, sizeof(COLOR_VERTEX_SHADER_OGL2), VertexPC::ATTRIBUTES);
                    break;
#if defined(OUZEL_SUPPORTS_OPENGL3) || defined(OUZEL_SUPPORTS_OPENGLES3)
                case 3:
                    colorShader = loadShaderFromBuffers(COLOR_PIXEL_SHADER_OGL3, sizeof(COLOR_PIXEL_SHADER_OGL3), COLOR_VERTEX_SHADER_OGL3, sizeof(COLOR_VERTEX_SHADER_OGL3), VertexPC::ATTRIBUTES);
                    break;
#endif
                default:
                    log("Unsupported OpenGL version");
                    return false;
            }

            if (!colorShader)
            {
                return false;
            }

            colorShader->setVertexShaderConstantInfo({{"modelViewProj", sizeof(Matrix4)}});
            colorShader->setPixelShaderConstantInfo({{"color", 4 * sizeof(float)}}, 256);

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

            BlendStatePtr addBlendState = createBlendState(true,
                                                           BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                                           BlendState::BlendOperation::ADD,
                                                           BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                                           BlendState::BlendOperation::ADD);

            if (!addBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            BlendStatePtr multiplyBlendState = createBlendState(true,
                                                                BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                                                BlendState::BlendOperation::ADD,
                                                                BlendState::BlendFactor::DEST_ALPHA, BlendState::BlendFactor::ZERO,
                                                                BlendState::BlendOperation::ADD);

            if (!multiplyBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

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

            ready = true;

            setSize(size);

            return true;
        }

        void RendererOGL::setFrameBuffer(GLuint newFrameBuffer)
        {
            frameBuffer = newFrameBuffer;
        }

        bool RendererOGL::checkOpenGLErrors(bool logError)
        {
            bool gotError = false;

            while (GLenum error = glGetError() != GL_NO_ERROR)
            {
                if (logError)
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
                }

                gotError = true;
            }

            return gotError;
        }

        void RendererOGL::setClearColor(Color color)
        {
            Renderer::setClearColor(color);
        }

        void RendererOGL::setSize(const Size2& newSize)
        {
            Renderer::setSize(newSize);

            viewport = Rectangle(0.0f, 0.0f, size.width, size.height);

            if (ready && !activeRenderTarget)
            {
                glViewport(static_cast<GLsizei>(viewport.x),
                           static_cast<GLsizei>(viewport.y),
                           static_cast<GLsizei>(viewport.width),
                           static_cast<GLsizei>(viewport.height));
            }
        }

        void RendererOGL::clear()
        {
            Renderer::clear();

            clearedFrameBuffers.clear();
        }

        void RendererOGL::present()
        {
            Renderer::present();
        }

        void RendererOGL::flush()
        {
            Renderer::flush();

            glFlush();
            checkOpenGLErrors();
        }

        std::vector<Size2> RendererOGL::getSupportedResolutions() const
        {
            return std::vector<Size2>();
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
            if (activeBlendState == blendState)
            {
                return true;
            }

            if (!Renderer::activateBlendState(blendState))
            {
                return false;
            }

            if (activeBlendState)
            {
                std::shared_ptr<BlendStateOGL> blendStateOGL = std::static_pointer_cast<BlendStateOGL>(activeBlendState);

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
                log("Failed to activate blend state");
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
            if (activeTextures[layer] == texture)
            {
                return true;
            }

            if (!Renderer::activateTexture(texture, layer))
            {
                return false;
            }

            if (activeTextures[layer])
            {
                std::shared_ptr<TextureOGL> textureOGL = std::static_pointer_cast<TextureOGL>(activeTextures[layer]);

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
            if (!Renderer::activateRenderTarget(renderTarget))
            {
                return false;
            }

            GLuint newFrameBuffer = 0;
            Color newClearColor;
            Rectangle newViewport;

            if (activeRenderTarget)
            {
                std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(activeRenderTarget);
                newFrameBuffer = renderTargetOGL->getFrameBufferId();
                newViewport = renderTargetOGL->getViewport();
                newClearColor = renderTargetOGL->getClearColor();

            }
            else
            {
                newFrameBuffer = frameBuffer;
                newViewport = viewport;
                newClearColor = clearColor;
            }

            glViewport(static_cast<GLsizei>(newViewport.x),
                       static_cast<GLsizei>(newViewport.y),
                       static_cast<GLsizei>(newViewport.width),
                       static_cast<GLsizei>(newViewport.height));

            bindFrameBuffer(newFrameBuffer);

            if (clearedFrameBuffers.find(newFrameBuffer) == clearedFrameBuffers.end())
            {
                glClearColor(newClearColor.getR(), newClearColor.getG(), newClearColor.getB(), newClearColor.getA());
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                if (checkOpenGLErrors())
                {
                    log("Failed to clear frame buffer");
                }

                clearedFrameBuffers.insert(newFrameBuffer);
            }

            return true;
        }

        ShaderPtr RendererOGL::loadShaderFromFiles(const std::string& pixelShader,
                                                   const std::string& vertexShader,
                                                   uint32_t vertexAttributes,
                                                   const std::string& pixelShaderFunction,
                                                   const std::string& vertexShaderFunction)
        {
            std::shared_ptr<ShaderOGL> shader(new ShaderOGL());

            if (!shader->initFromFiles(pixelShader, vertexShader, vertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                shader.reset();
            }

            return shader;
        }

        ShaderPtr RendererOGL::loadShaderFromBuffers(const uint8_t* pixelShader,
                                                     uint32_t pixelShaderSize,
                                                     const uint8_t* vertexShader,
                                                     uint32_t vertexShaderSize,
                                                     uint32_t vertexAttributes,
                                                     const std::string& pixelShaderFunction,
                                                     const std::string& vertexShaderFunction)
        {
            std::shared_ptr<ShaderOGL> shader(new ShaderOGL());

            if (!shader->initFromBuffers(pixelShader, pixelShaderSize, vertexShader, vertexShaderSize, vertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                shader.reset();
            }

            return shader;
        }

        bool RendererOGL::activateShader(const ShaderPtr& shader)
        {
            if (activeShader == shader)
            {
                return true;
            }

            if (!Renderer::activateShader(shader))
            {
                return false;
            }

            if (activeShader)
            {
                std::shared_ptr<ShaderOGL> shaderOGL = std::static_pointer_cast<ShaderOGL>(activeShader);

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

        bool RendererOGL::drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount, DrawMode drawMode, uint32_t startIndex)
        {
            if (!Renderer::drawMeshBuffer(meshBuffer))
            {
                return false;
            }

            if (!activeShader)
            {
                return false;
            }

            if (activeRenderTarget)
            {
                std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(activeRenderTarget);

                bindFrameBuffer(renderTargetOGL->getFrameBufferId());
            }
            else
            {
                bindFrameBuffer(frameBuffer);
            }

            for (uint32_t layer = 0; layer < TEXTURE_LAYERS; ++layer)
            {
                if (activeTextures[layer])
                {
                    std::shared_ptr<TextureOGL> textureOGL = std::static_pointer_cast<TextureOGL>(activeTextures[layer]);

                    bindTexture(textureOGL->getTextureId(), layer);
                }
                else
                {
                    bindTexture(0, layer);
                }
            }

            std::shared_ptr<ShaderOGL> shaderOGL = std::static_pointer_cast<ShaderOGL>(activeShader);
            bindProgram(shaderOGL->getProgramId());

            std::shared_ptr<MeshBufferOGL> meshBufferOGL = std::static_pointer_cast<MeshBufferOGL>(meshBuffer);

            if (indexCount == 0)
            {
                indexCount = meshBufferOGL->getIndexCount() - startIndex;
            }

            GLenum mode;

            switch (drawMode)
            {
                case DrawMode::POINT_LIST: mode = GL_POINTS; break;
                case DrawMode::LINE_LIST: mode = GL_LINES; break;
                case DrawMode::LINE_STRIP: mode = GL_LINE_STRIP; break;
                case DrawMode::TRIANGLE_LIST: mode = GL_TRIANGLES; break;
                case DrawMode::TRIANGLE_STRIP: mode = GL_TRIANGLE_STRIP; break;
                default: return false;
            }

            if (!meshBufferOGL->bindVertexBuffer())
            {
                return false;
            }

            if (!bindElementArrayBuffer(meshBufferOGL->getIndexBufferId()))
            {
                return false;
            }

            glDrawElements(mode, static_cast<GLsizei>(indexCount), meshBufferOGL->getIndexFormat(), static_cast<const char*>(nullptr) + (startIndex * meshBuffer->getIndexSize()));

            if (checkOpenGLErrors())
            {
                log("Failed to draw elements");
                return false;
            }

            return true;
        }

        bool RendererOGL::saveScreenshot(const std::string& filename)
        {
            bindFrameBuffer(frameBuffer);

            GLsizei width = static_cast<GLsizei>(size.width);
            GLsizei height = static_cast<GLsizei>(size.height);

            std::unique_ptr<uint8_t[]> data(new uint8_t[3 * width * height]);

            glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data.get());

            if (checkOpenGLErrors())
            {
                log("Failed to read pixels from frame buffer");
                return false;
            }

            uint32_t w = static_cast<uint32_t>(width);
            uint32_t h = static_cast<uint32_t>(height);
            uint8_t temp;
            uint32_t depth = 3;

            for (uint32_t row = 0; row < (h>>1); ++row)
            {
                for (uint32_t col = 0; col < w; ++col)
                {
                    for (uint32_t z = 0; z < depth; ++z)
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

        GLuint RendererOGL::currentTextureId[TEXTURE_LAYERS] = { 0 };
        GLuint RendererOGL::currentProgramId = 0;
        GLuint RendererOGL::currentFrameBufferId = 0;
        GLuint RendererOGL::currentElementArrayBufferId = 0;
        GLuint RendererOGL::currentArrayBufferId = 0;
        GLuint RendererOGL::currentVertexArrayId = 0;

        bool RendererOGL::bindTexture(GLuint textureId, uint32_t layer)
        {
            if (currentTextureId[layer] != textureId)
            {
                glActiveTexture(GL_TEXTURE0 + layer);
                glBindTexture(GL_TEXTURE_2D, textureId);
                currentTextureId[layer] = textureId;

                if (checkOpenGLErrors())
                {
                    log("Failed to create bind texture");
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::bindProgram(GLuint programId)
        {
            if (currentProgramId != programId)
            {
                glUseProgram(programId);
                currentProgramId = programId;

                if (checkOpenGLErrors())
                {
                    log("Failed to bind program");
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::bindFrameBuffer(GLuint frameBufferId)
        {
            if (currentFrameBufferId != frameBufferId)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
                currentFrameBufferId = frameBufferId;

                if (checkOpenGLErrors())
                {
                    log("Failed to create bind frame buffer");
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::bindElementArrayBuffer(GLuint elementArrayBufferId)
        {
            if (currentElementArrayBufferId != elementArrayBufferId)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferId);
                currentElementArrayBufferId = elementArrayBufferId;

                if (checkOpenGLErrors())
                {
                    log("Failed to bind element array buffer");
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::bindArrayBuffer(GLuint arrayBufferId)
        {
            if (currentArrayBufferId != arrayBufferId)
            {
                glBindBuffer(GL_ARRAY_BUFFER, arrayBufferId);
                currentArrayBufferId = arrayBufferId;

                if (checkOpenGLErrors())
                {
                    log("Failed to bind array buffer");
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::bindVertexArray(GLuint vertexArrayId)
        {
            if (currentVertexArrayId != vertexArrayId)
            {
#if defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
                glBindVertexArrayOES(vertexArrayId);
#elif defined(OUZEL_PLATFORM_ANDROID)
                if (glBindVertexArrayOESEXT) glBindVertexArrayOESEXT(vertexArrayId);
#else
                glBindVertexArray(vertexArrayId);
#endif
                currentVertexArrayId = vertexArrayId;

                if (checkOpenGLErrors())
                {
                    log("Failed to bind vertex array");
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::unbindTexture(GLuint textureId)
        {
            for (uint32_t layer = 0; layer < TEXTURE_LAYERS; ++layer)
            {
                if (currentTextureId[layer] == textureId)
                {
                    bindTexture(0, layer);
                }
            }

            return true;
        }

        bool RendererOGL::unbindProgram(GLuint programId)
        {
            if (currentProgramId == programId)
            {
                return bindProgram(0);
            }

            return true;
        }

        bool RendererOGL::unbindFrameBuffer(GLuint frameBufferId)
        {
            if (currentFrameBufferId == frameBufferId)
            {
                return bindFrameBuffer(0);
            }

            return true;
        }

        bool RendererOGL::unbindElementArrayBuffer(GLuint elementArrayBufferId)
        {
            if (currentElementArrayBufferId == elementArrayBufferId)
            {
                return bindElementArrayBuffer(0);
            }

            return true;
        }

        bool RendererOGL::unbindArrayBuffer(GLuint arrayBufferId)
        {
            if (currentArrayBufferId == arrayBufferId)
            {
                return bindArrayBuffer(0);
            }

            return true;
        }

        bool RendererOGL::unbindVertexArray(GLuint vertexArrayId)
        {
            if (currentVertexArrayId == vertexArrayId)
            {
                return bindVertexArray(0);
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
