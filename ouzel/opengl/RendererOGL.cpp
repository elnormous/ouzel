// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGL.h"
#include "TextureOGL.h"
#include "RenderTargetOGL.h"
#include "ShaderOGL.h"
#include "MeshBufferOGL.h"
#include "BlendStateOGL.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "core/Cache.h"
#include "utils/Utils.h"
#include "stb_image_write.h"

#if OUZEL_SUPPORTS_OPENGL
#include "ColorPSOGL2.h"
#include "ColorVSOGL2.h"
#include "TexturePSOGL2.h"
#include "TextureVSOGL2.h"
    #if OUZEL_SUPPORTS_OPENGL3
    #include "ColorPSOGL3.h"
    #include "ColorVSOGL3.h"
    #include "TexturePSOGL3.h"
    #include "TextureVSOGL3.h"
    #endif
#endif

#if OUZEL_SUPPORTS_OPENGLES
#include "ColorPSOGLES2.h"
#include "ColorVSOGLES2.h"
#include "TexturePSOGLES2.h"
#include "TextureVSOGLES2.h"
    #if OUZEL_SUPPORTS_OPENGLES3
    #include "ColorPSOGLES3.h"
    #include "ColorVSOGLES3.h"
    #include "TexturePSOGLES3.h"
    #include "TextureVSOGLES3.h"
    #endif
#endif

#if OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT = nullptr;
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT = nullptr;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT = nullptr;
#endif

namespace ouzel
{
    namespace graphics
    {
        bool RendererOGL::checkOpenGLError(bool logError)
        {
            GLenum error = glGetError();

            if (error != GL_NO_ERROR)
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

                return true;
            }
            
            return false;
        }
        
        RendererOGL::RendererOGL():
            Renderer(Driver::OPENGL)
        {
#if OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN
            glGenVertexArraysOESEXT = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
            glBindVertexArrayOESEXT = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
            glDeleteVertexArraysOESEXT = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
#endif
        }

        RendererOGL::~RendererOGL()
        {

        }

        bool RendererOGL::init(const WindowPtr& window,
                               uint32_t newSampleCount,
                               TextureFiltering newTextureFiltering,
                               float newTargetFPS,
                               bool newVerticalSync)
        {
            if (!Renderer::init(window, newSampleCount, newTextureFiltering, newTargetFPS, newVerticalSync))
            {
                return false;
            }

            if (sampleCount > 1)
            {
                log("Multisample anti-aliasing is disabled for OpenGL");
            }

            //glEnable(GL_DEPTH_TEST);
            glClearColor(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());

            ShaderPtr textureShader = createShader();

            switch (apiVersion)
            {
                case 2:
                    textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_OGL2), std::end(TEXTURE_PIXEL_SHADER_OGL2)),
                                                   std::vector<uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_OGL2), std::end(TEXTURE_VERTEX_SHADER_OGL2)),
                                                   VertexPCT::ATTRIBUTES);
                    break;
#if OUZEL_SUPPORTS_OPENGL3 || OUZEL_SUPPORTS_OPENGLES3
                case 3:
                    textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_OGL3), std::end(TEXTURE_PIXEL_SHADER_OGL3)),
                                                   std::vector<uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_OGL3), std::end(TEXTURE_VERTEX_SHADER_OGL3)),
                                                   VertexPCT::ATTRIBUTES);
                    break;
#endif
                default:
                    log("Unsupported OpenGL version");
                    return false;
            }

            textureShader->setVertexShaderConstantInfo({{"modelViewProj", sizeof(Matrix4)}});
            textureShader->setPixelShaderConstantInfo({{"color", 4 * sizeof(float)}}, 256);

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader = createShader();

            switch (apiVersion)
            {
                case 2:
                    colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(COLOR_PIXEL_SHADER_OGL2), std::end(COLOR_PIXEL_SHADER_OGL2)),
                                                 std::vector<uint8_t>(std::begin(COLOR_VERTEX_SHADER_OGL2), std::end(COLOR_VERTEX_SHADER_OGL2)),
                                                 VertexPC::ATTRIBUTES);
                    break;
#if OUZEL_SUPPORTS_OPENGL3 || OUZEL_SUPPORTS_OPENGLES3
                case 3:
                    colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(COLOR_PIXEL_SHADER_OGL3), std::end(COLOR_PIXEL_SHADER_OGL3)),
                                                 std::vector<uint8_t>(std::begin(COLOR_VERTEX_SHADER_OGL3), std::end(COLOR_VERTEX_SHADER_OGL3)),
                                                 VertexPC::ATTRIBUTES);
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

            BlendStatePtr noBlendState = createBlendState();

            noBlendState->init(false,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            BlendStatePtr addBlendState = createBlendState();

            addBlendState->init(true,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            BlendStatePtr multiplyBlendState = createBlendState();

            multiplyBlendState->init(true,
                                     BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                     BlendState::BlendOperation::ADD,
                                     BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                     BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            BlendStatePtr alphaBlendState = createBlendState();

            alphaBlendState->init(true,
                                  BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                  BlendState::BlendOperation::ADD,
                                  BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                  BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            ready = true;

            setSize(size);

            return true;
        }

        void RendererOGL::setClearColor(Color color)
        {
            Renderer::setClearColor(color);
        }

        void RendererOGL::setSize(const Size2& newSize)
        {
            Renderer::setSize(newSize);

            viewport = Rectangle(0.0f, 0.0f, size.width, size.height);
        }

        void RendererOGL::clear()
        {
            std::function<void(void)> func;

            for (;;)
            {
                std::pair<GLuint, ResourceType> deleteResource;

                {
                    std::lock_guard<std::mutex> lock(deleteMutex);
                    if (deleteQueue.empty())
                    {
                        break;
                    }

                    deleteResource = deleteQueue.front();
                    deleteQueue.pop();
                }

                switch (deleteResource.second)
                {
                    case ResourceType::Buffer:
                        unbindArrayBuffer(deleteResource.first);
                        unbindElementArrayBuffer(deleteResource.first);
                        glDeleteBuffers(1, &deleteResource.first);
                        break;
                    case ResourceType::VertexArray:
                        unbindVertexArray(deleteResource.first);
#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                        glDeleteVertexArraysOES(1, &deleteResource.first);
#elif OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN
                        if (glDeleteVertexArraysOESEXT) glDeleteVertexArraysOESEXT(1, &deleteResource.first);
#else
                        glDeleteVertexArrays(1, &deleteResource.first);
#endif
                        break;
                    case ResourceType::RenderBuffer:
                        glDeleteRenderbuffers(1, &deleteResource.first);
                        break;
                    case ResourceType::FrameBuffer:
                        unbindFrameBuffer(deleteResource.first);
                        glDeleteFramebuffers(1, &deleteResource.first);
                        break;
                    case ResourceType::Program:
                        unbindProgram(deleteResource.first);
                        glDeleteProgram(deleteResource.first);
                        break;
                    case ResourceType::Shader:
                        glDeleteShader(deleteResource.first);
                        break;
                    case ResourceType::Texture:
                        unbindTexture(deleteResource.first);
                        glDeleteTextures(1, &deleteResource.first);
                        break;
                }
            }

            Renderer::clear();
        }

        bool RendererOGL::present()
        {
            if (!Renderer::present())
            {
                return false;
            }

            std::set<GLuint> clearedFrameBuffers;

            std::queue<DrawCommand> drawCommands;

            {
                std::lock_guard<std::mutex> lock(drawQueueMutex);
                drawCommands = drawQueue;
            }

            while (!drawCommands.empty())
            {
                const DrawCommand drawCommand = drawCommands.front();
                drawCommands.pop();

                // blend state
                std::shared_ptr<BlendStateOGL> blendStateOGL = std::static_pointer_cast<BlendStateOGL>(drawCommand.blendState);

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

                if (checkOpenGLError())
                {
                    log("Failed to activate blend state");
                    return false;
                }

                // textures
                for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                {
                    std::shared_ptr<TextureOGL> textureOGL;

                    if (drawCommand.textures.size() > layer)
                    {
                        textureOGL = std::static_pointer_cast<TextureOGL>(drawCommand.textures[layer]);
                    }

                    if (textureOGL)
                    {
                        if (!textureOGL->update())
                        {
                            return false;
                        }

                        bindTexture(textureOGL->getTextureId(), layer);
                    }
                    else
                    {
                        bindTexture(0, layer);
                    }
                }

                if (checkOpenGLError())
                {
                    log("Failed to bind texture");
                    return false;
                }

                // shader
                std::shared_ptr<ShaderOGL> shaderOGL = std::static_pointer_cast<ShaderOGL>(drawCommand.shader);
                if (!shaderOGL->update())
                {
                    return false;
                }
                bindProgram(shaderOGL->getProgramId());

                if (checkOpenGLError())
                {
                    log("Failed to bind shader");
                    return false;
                }

                // pixel shader constants
                const std::vector<GLint>& pixelShaderConstantLocations = shaderOGL->getPixelShaderConstantLocations();
                const std::vector<Shader::ConstantInfo>& pixelShaderConstantInfos = shaderOGL->getPixelShaderConstantInfo();

                if (drawCommand.pixelShaderConstants.size() > pixelShaderConstantLocations.size())
                {
                    log("Invalid pixel shader constant size");
                    return false;
                }

                for (size_t i = 0; i < drawCommand.pixelShaderConstants.size(); ++i)
                {
                    GLint location = pixelShaderConstantLocations[i];
                    const Shader::ConstantInfo& pixelShaderConstantInfo = pixelShaderConstantInfos[i];
                    const std::vector<float>& pixelShaderConstant = drawCommand.pixelShaderConstants[i];

                    uint32_t components = pixelShaderConstantInfo.size / 4;

                    switch (components)
                    {
                        case 1:
                            glUniform1fv(location, static_cast<GLsizei>(pixelShaderConstant.size() / components), pixelShaderConstant.data());
                            break;
                        case 2:
                            glUniform2fv(location, static_cast<GLsizei>(pixelShaderConstant.size() / components), pixelShaderConstant.data());
                            break;
                        case 3:
                            glUniform3fv(location, static_cast<GLsizei>(pixelShaderConstant.size() / components), pixelShaderConstant.data());
                            break;
                        case 4:
                            glUniform4fv(location, static_cast<GLsizei>(pixelShaderConstant.size() / components), pixelShaderConstant.data());
                            break;
                        case 9:
                            glUniformMatrix3fv(location, static_cast<GLsizei>(pixelShaderConstant.size() / components), GL_FALSE, pixelShaderConstant.data());
                            break;
                        case 16:
                            glUniformMatrix4fv(location, static_cast<GLsizei>(pixelShaderConstant.size() / components), GL_FALSE, pixelShaderConstant.data());
                            break;
                        default:
                            log("Unsupported uniform size");
                            return false;
                    }
                }

                // vertex shader constants
                const std::vector<GLint>& vertexShaderConstantLocations = shaderOGL->getVertexShaderConstantLocations();
                const std::vector<Shader::ConstantInfo>& vertexShaderConstantInfos = shaderOGL->getVertexShaderConstantInfo();

                if (drawCommand.vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                {
                    log("Invalid vertex shader constant size");
                    return false;
                }

                for (size_t i = 0; i < drawCommand.vertexShaderConstants.size(); ++i)
                {
                    GLint location = vertexShaderConstantLocations[i];
                    const Shader::ConstantInfo& vertexShaderConstantInfo = vertexShaderConstantInfos[i];
                    const std::vector<float>& vertexShaderConstant = drawCommand.vertexShaderConstants[i];

                    uint32_t components = vertexShaderConstantInfo.size / 4;

                    switch (components)
                    {
                        case 1:
                            glUniform1fv(location, static_cast<GLsizei>(vertexShaderConstant.size() / components), vertexShaderConstant.data());
                            break;
                        case 2:
                            glUniform2fv(location, static_cast<GLsizei>(vertexShaderConstant.size() / components), vertexShaderConstant.data());
                            break;
                        case 3:
                            glUniform3fv(location, static_cast<GLsizei>(vertexShaderConstant.size() / components), vertexShaderConstant.data());
                            break;
                        case 4:
                            glUniform4fv(location, static_cast<GLsizei>(vertexShaderConstant.size() / components), vertexShaderConstant.data());
                            break;
                        case 9:
                            glUniformMatrix3fv(location, static_cast<GLsizei>(vertexShaderConstant.size() / components), GL_FALSE, vertexShaderConstant.data());
                            break;
                        case 16:
                            glUniformMatrix4fv(location, static_cast<GLsizei>(vertexShaderConstant.size() / components), GL_FALSE, vertexShaderConstant.data());
                            break;
                        default:
                            log("Unsupported uniform size");
                            return false;
                    }
                }

                // render target
                GLuint newFrameBuffer = 0;
                Color newClearColor;
                Rectangle newViewport;

                if (drawCommand.renderTarget)
                {
                    std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(drawCommand.renderTarget);
                    newFrameBuffer = renderTargetOGL->getFrameBufferId();
                    newViewport = renderTargetOGL->getViewport();
                    newClearColor = renderTargetOGL->getClearColor();

                }
                else
                {
                    newFrameBuffer = frameBufferId;
                    newViewport = viewport;
                    newClearColor = clearColor;
                }

                bindFrameBuffer(newFrameBuffer);

                if (checkOpenGLError())
                {
                    log("Failed to bind frame buffer");
                    return false;
                }

                glViewport(static_cast<GLsizei>(newViewport.x),
                           static_cast<GLsizei>(newViewport.y),
                           static_cast<GLsizei>(newViewport.width),
                           static_cast<GLsizei>(newViewport.height));

                if (clearedFrameBuffers.find(newFrameBuffer) == clearedFrameBuffers.end())
                {
                    glClearColor(newClearColor.getR(), newClearColor.getG(), newClearColor.getB(), newClearColor.getA());
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    if (checkOpenGLError())
                    {
                        log("Failed to clear frame buffer");
                    }
                    
                    clearedFrameBuffers.insert(newFrameBuffer);
                }

                // scissor test
                if (drawCommand.scissorTestEnabled)
                {
                    glEnable(GL_SCISSOR_TEST);

                    glScissor(static_cast<GLint>(drawCommand.scissorTest.x),
                              static_cast<GLint>(drawCommand.scissorTest.y),
                              static_cast<GLsizei>(drawCommand.scissorTest.width),
                              static_cast<GLsizei>(drawCommand.scissorTest.height));
                }
                else
                {
                    glDisable(GL_SCISSOR_TEST);
                }

                // mesh buffer
                std::shared_ptr<MeshBufferOGL> meshBufferOGL = std::static_pointer_cast<MeshBufferOGL>(drawCommand.meshBuffer);
                if (!meshBufferOGL->update())
                {
                    return false;
                }

                uint32_t indexCount = drawCommand.indexCount;

                if (indexCount == 0)
                {
                    indexCount = meshBufferOGL->getIndexCount() - drawCommand.startIndex;
                }

                GLenum mode;

                switch (drawCommand.drawMode)
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

                glDrawElements(mode, static_cast<GLsizei>(indexCount), meshBufferOGL->getIndexFormat(), static_cast<const char*>(nullptr) + (drawCommand.startIndex * meshBufferOGL->getIndexSize()));
                
                if (checkOpenGLError())
                {
                    log("Failed to draw elements");
                    return false;
                }
            }

            return true;
        }

        void RendererOGL::flush()
        {
            Renderer::flush();

            glFlush();
            if (checkOpenGLError())
            {
                log("Failed to flush OpenGL buffer");
            }
        }

        std::vector<Size2> RendererOGL::getSupportedResolutions() const
        {
            return std::vector<Size2>();
        }

        BlendStatePtr RendererOGL::createBlendState()
        {
            std::shared_ptr<BlendStateOGL> blendState(new BlendStateOGL());
            return blendState;
        }

        TexturePtr RendererOGL::createTexture()
        {
            std::shared_ptr<TextureOGL> texture(new TextureOGL());
            return texture;
        }

        RenderTargetPtr RendererOGL::createRenderTarget()
        {
            std::shared_ptr<RenderTargetOGL> renderTarget(new RenderTargetOGL());
            return renderTarget;
        }

        ShaderPtr RendererOGL::createShader()
        {
            std::shared_ptr<ShaderOGL> shader(new ShaderOGL());
            return shader;
        }

        MeshBufferPtr RendererOGL::createMeshBuffer()
        {
            std::shared_ptr<MeshBufferOGL> meshBuffer(new MeshBufferOGL());
            return meshBuffer;
        }

        bool RendererOGL::saveScreenshot(const std::string& filename)
        {
            bindFrameBuffer(frameBufferId);

            const GLsizei width = static_cast<GLsizei>(size.width);
            const GLsizei height = static_cast<GLsizei>(size.height);
            const GLsizei depth = 4;

            std::vector<uint8_t> data(width * height * depth);

            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

            if (checkOpenGLError())
            {
                log("Failed to read pixels from frame buffer");
                return false;
            }

            uint8_t temp;
            for (GLsizei row = 0; row < height / 2; ++row)
            {
                for (GLsizei col = 0; col < width; ++col)
                {
                    for (GLsizei z = 0; z < depth; ++z)
                    {
                        temp = data[((height - row - 1) * width + col) * depth + z];
                        data[((height - row - 1) * width + col) * depth + z] = data[(row * width + col) * depth + z];
                        data[(row * width + col) * depth + z] = temp;
                    }
                }
            }

            if (!stbi_write_png(filename.c_str(), width, height, depth, data.data(), width * depth))
            {
                log("Failed to save image to file");
                return false;
            }
            
            return true;
        }

        GLuint RendererOGL::currentTextureId[Texture::LAYERS] = { 0 };
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

                if (checkOpenGLError())
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

                if (checkOpenGLError())
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

                if (checkOpenGLError())
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

                if (checkOpenGLError())
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

                if (checkOpenGLError())
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
#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                glBindVertexArrayOES(vertexArrayId);
#elif OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN
                if (glBindVertexArrayOESEXT) glBindVertexArrayOESEXT(vertexArrayId);
#else
                glBindVertexArray(vertexArrayId);
#endif
                currentVertexArrayId = vertexArrayId;

                if (checkOpenGLError())
                {
                    log("Failed to bind vertex array");
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::unbindTexture(GLuint textureId)
        {
            for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
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

        void RendererOGL::deleteResource(GLuint resource, ResourceType resourceType)
        {
            std::lock_guard<std::mutex> lock(deleteMutex);

            deleteQueue.push(std::make_pair(resource, resourceType));
        }
    } // namespace graphics
} // namespace ouzel
