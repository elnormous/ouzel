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
            Renderer(Driver::OPENGL), dirty(false)
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
                               bool newVerticalSync)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Renderer::init(window, newSampleCount, newTextureFiltering, newVerticalSync))
            {
                return false;
            }

            if (sampleCount > 1)
            {
                log("Multisample anti-aliasing is disabled for OpenGL");
            }

            if (!setViewport(static_cast<GLint>(viewport.x),
                             static_cast<GLint>(viewport.y),
                             static_cast<GLsizei>(viewport.width),
                             static_cast<GLsizei>(viewport.height)))
            {
                return false;
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

            dirty = true;
            ready = true;

            return true;
        }

        bool RendererOGL::update()
        {
            if (dirty)
            {
                std::lock_guard<std::mutex> lock(dataMutex);

                clearMask = GL_COLOR_BUFFER_BIT;

                frameBufferClearColor[0] = clearColor.getR();
                frameBufferClearColor[1] = clearColor.getG();
                frameBufferClearColor[2] = clearColor.getB();
                frameBufferClearColor[3] = clearColor.getA();

                viewport = Rectangle(0.0f, 0.0f, size.width, size.height);

                dirty = false;
            }

            return true;
        }

        void RendererOGL::setClearColor(Color color)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            Renderer::setClearColor(color);

            dirty = true;
        }

        void RendererOGL::setSize(const Size2& newSize)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            Renderer::setSize(newSize);

            dirty = true;
        }

        bool RendererOGL::present()
        {
            if (!Renderer::present())
            {
                return false;
            }

            std::set<GLuint> clearedFrameBuffers;

            std::queue<ResourcePtr> resources;

            {
                std::lock_guard<std::mutex> lock(updateMutex);
                resources = std::move(updateQueue);
                updateSet.clear();
            }

            while (!resources.empty())
            {
                const ResourcePtr& resource = resources.front();

                if (!resource->update())
                {
                    return false;
                }

                resources.pop();
            }

            deleteResources();

            if (!update())
            {
                return false;
            }

            if (drawQueue.empty())
            {
                if (!bindFrameBuffer(frameBufferId))
                {
                    return false;
                }

                if (!setViewport(static_cast<GLint>(viewport.x),
                                 static_cast<GLint>(viewport.y),
                                 static_cast<GLsizei>(viewport.width),
                                 static_cast<GLsizei>(viewport.height)))
                {
                    return false;
                }

                glClearColor(frameBufferClearColor[0],
                             frameBufferClearColor[1],
                             frameBufferClearColor[2],
                             frameBufferClearColor[3]);

                glClear(clearMask);

                if (checkOpenGLError())
                {
                    log("Failed to clear frame buffer");
                    return false;
                }
            }
            else for (const DrawCommand& drawCommand : drawQueue)
            {
                // blend state
                std::shared_ptr<BlendStateOGL> blendStateOGL = std::static_pointer_cast<BlendStateOGL>(drawCommand.blendState);

                if (!blendStateOGL || !blendStateOGL->isReady())
                {
                    // don't render if invalid blend state
                    continue;
                }

                if (!setBlendState(blendStateOGL->isBlendingEnabled(),
                                   blendStateOGL->getModeRGB(),
                                   blendStateOGL->getModeAlpha(),
                                   blendStateOGL->getSourceFactorRGB(),
                                   blendStateOGL->getDestFactorRGB(),
                                   blendStateOGL->getSourceFactorAlpha(),
                                   blendStateOGL->getDestFactorAlpha()))
                {
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
                        if (!textureOGL->isReady())
                        {
                            // don't render if invalid texture
                            continue;
                        }

                        if (!bindTexture(textureOGL->getTextureId(), layer))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (!bindTexture(0, layer))
                        {
                            return false;
                        }
                    }
                }

                // shader
                std::shared_ptr<ShaderOGL> shaderOGL = std::static_pointer_cast<ShaderOGL>(drawCommand.shader);

                if (!shaderOGL || !shaderOGL->isReady())
                {
                    // don't render if invalid shader
                    continue;
                }

                bindProgram(shaderOGL->getProgramId());

                // pixel shader constants
                const std::vector<GLint>& pixelShaderConstantLocations = shaderOGL->getPixelShaderConstantLocations();
                const std::vector<Shader::ConstantInfo>& pixelShaderConstantInfos = shaderOGL->getPixelShaderConstantInfo();

                if (drawCommand.pixelShaderConstants.size() > pixelShaderConstantInfos.size())
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

                if (drawCommand.vertexShaderConstants.size() > vertexShaderConstantInfos.size())
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
                GLbitfield newClearMask = 0;
                const float* newClearColor;
                Rectangle newViewport;

                if (drawCommand.renderTarget)
                {
                    std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(drawCommand.renderTarget);

                    if (!renderTargetOGL->isReady())
                    {
                        // don't render if invalid render target
                        continue;
                    }

                    newFrameBuffer = renderTargetOGL->getFrameBufferId();
                    newClearMask = renderTargetOGL->getClearMask();
                    newClearColor = renderTargetOGL->getFrameBufferClearColor();
                    newViewport = renderTargetOGL->getViewport();

                }
                else
                {
                    newFrameBuffer = frameBufferId;
                    newClearMask = clearMask;
                    newClearColor = frameBufferClearColor;
                    newViewport = viewport;
                }

                if (!bindFrameBuffer(newFrameBuffer))
                {
                    return false;
                }

                setViewport(static_cast<GLint>(newViewport.x),
                            static_cast<GLint>(newViewport.y),
                            static_cast<GLsizei>(newViewport.width),
                            static_cast<GLsizei>(newViewport.height));

                auto clearedFrameBuffer = clearedFrameBuffers.insert(newFrameBuffer);

                if (clearedFrameBuffer.second)
                {
                    glClearColor(newClearColor[0],
                                 newClearColor[1],
                                 newClearColor[2],
                                 newClearColor[3]);

                    glClear(newClearMask);

                    if (checkOpenGLError())
                    {
                        log("Failed to clear frame buffer");
                        return false;
                    }

                }

                // scissor test
                setScissorTest(drawCommand.scissorTestEnabled,
                               static_cast<GLint>(drawCommand.scissorTest.x),
                               static_cast<GLint>(drawCommand.scissorTest.y),
                               static_cast<GLsizei>(drawCommand.scissorTest.width),
                               static_cast<GLsizei>(drawCommand.scissorTest.height));

                // mesh buffer
                std::shared_ptr<MeshBufferOGL> meshBufferOGL = std::static_pointer_cast<MeshBufferOGL>(drawCommand.meshBuffer);

                if (!meshBufferOGL || !meshBufferOGL->isReady())
                {
                    // don't render if invalid mesh buffer
                    continue;
                }

                // draw
                GLenum mode;

                switch (drawCommand.drawMode)
                {
                    case DrawMode::POINT_LIST: mode = GL_POINTS; break;
                    case DrawMode::LINE_LIST: mode = GL_LINES; break;
                    case DrawMode::LINE_STRIP: mode = GL_LINE_STRIP; break;
                    case DrawMode::TRIANGLE_LIST: mode = GL_TRIANGLES; break;
                    case DrawMode::TRIANGLE_STRIP: mode = GL_TRIANGLE_STRIP; break;
                    default: log("Invalid draw mode"); return false;
                }

                if (!meshBufferOGL->bindBuffers())
                {
                    return false;
                }

                glDrawElements(mode,
                               static_cast<GLsizei>(drawCommand.indexCount),
                               meshBufferOGL->getIndexFormat(),
                               static_cast<const char*>(nullptr) + (drawCommand.startIndex * meshBufferOGL->getIndexSize()));

                if (checkOpenGLError())
                {
                    log("Failed to draw elements");
                    return false;
                }
            }

            if (!saveScreenshots())
            {
                return false;
            }

            return true;
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

        bool RendererOGL::saveScreenshots()
        {
            for (;;)
            {
                std::string filename;

                {
                    std::lock_guard<std::mutex> lock(screenshotMutex);

                    if (screenshotQueue.empty()) break;

                    filename = screenshotQueue.front();
                    screenshotQueue.pop();
                }

                bindFrameBuffer(frameBufferId);

                const GLsizei width = static_cast<GLsizei>(size.width);
                const GLsizei height = static_cast<GLsizei>(size.height);
                const GLsizei depth = 4;

                std::vector<uint8_t> data(static_cast<size_t>(width * height * depth));

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
                            temp = data[static_cast<size_t>(((height - row - 1) * width + col) * depth + z)];
                            data[static_cast<size_t>(((height - row - 1) * width + col) * depth + z)] = data[static_cast<size_t>((row * width + col) * depth + z)];
                            data[static_cast<size_t>((row * width + col) * depth + z)] = temp;
                        }
                    }
                }

                if (!stbi_write_png(filename.c_str(), width, height, depth, data.data(), width * depth))
                {
                    log("Failed to save image to file");
                    return false;
                }
            }

            return true;
        }

        GLuint RendererOGL::currentTextureId[Texture::LAYERS] = { 0 };
        GLuint RendererOGL::currentProgramId = 0;
        GLuint RendererOGL::currentFrameBufferId = 0;

        GLuint RendererOGL::currentElementArrayBufferId = 0;
        GLuint RendererOGL::currentArrayBufferId = 0;
        GLuint RendererOGL::currentVertexArrayId = 0;

        bool RendererOGL::currentBlendEnabled = false;
        GLenum RendererOGL::currentBlendModeRGB = 0;
        GLenum RendererOGL::currentBlendModeAlpha = 0;
        GLenum RendererOGL::currentBlendSourceFactorRGB = 0;
        GLenum RendererOGL::currentBlendDestFactorRGB = 0;
        GLenum RendererOGL::currentBlendSourceFactorAlpha = 0;
        GLenum RendererOGL::currentBlendDestFactorAlpha = 0;

        bool RendererOGL::currentScissorTestEnabled = false;
        GLint RendererOGL::currentScissorX = 0;
        GLint RendererOGL::currentScissorY = 0;
        GLsizei RendererOGL::currentScissorWidth = 0;
        GLsizei RendererOGL::currentScissorHeight = 0;

        bool RendererOGL::currentDepthTestEnabled = false;

        GLint RendererOGL::currentViewportX = 0;
        GLint RendererOGL::currentViewportY = 0;
        GLsizei RendererOGL::currentViewportWidth = 0;
        GLsizei RendererOGL::currentViewportHeight = 0;
        std::queue<std::pair<GLuint, RendererOGL::ResourceType>> RendererOGL::deleteQueue;
        std::mutex RendererOGL::deleteMutex;

        bool RendererOGL::bindTexture(GLuint textureId, uint32_t layer)
        {
            if (currentTextureId[layer] != textureId)
            {
                glActiveTexture(GL_TEXTURE0 + layer);
                glBindTexture(GL_TEXTURE_2D, textureId);
                currentTextureId[layer] = textureId;

                if (checkOpenGLError())
                {
                    log("Failed to bind texture");
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
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX
                glBindVertexArray(vertexArrayId);
#endif
                currentVertexArrayId = vertexArrayId;
                currentElementArrayBufferId = 0;

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

        bool RendererOGL::setScissorTest(bool scissorTestEnabled,
                                         GLint x,
                                         GLint y,
                                         GLsizei width,
                                         GLsizei height)
        {
            if (currentScissorTestEnabled != scissorTestEnabled)
            {
                if (scissorTestEnabled)
                {
                    glEnable(GL_SCISSOR_TEST);
                }
                else
                {
                    glDisable(GL_SCISSOR_TEST);
                }

                if (checkOpenGLError())
                {
                    log("Failed to set scissor test");
                    return false;
                }

                currentScissorTestEnabled = scissorTestEnabled;
            }

            if (scissorTestEnabled)
            {
                if (x != currentScissorX ||
                    y != currentScissorY ||
                    width != currentScissorWidth ||
                    height != currentScissorHeight)
                {
                    glScissor(x, y, width, height);
                    currentScissorX = x;
                    currentScissorY = y;
                    currentScissorWidth = width;
                    currentScissorHeight = height;
                }

                if (checkOpenGLError())
                {
                    log("Failed to set scissor test");
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::enableDepthTest(bool enable)
        {
            if (currentDepthTestEnabled != enable)
            {
                if (enable)
                {
                    glEnable(GL_DEPTH_TEST);
                }
                else
                {
                    glDisable(GL_DEPTH_TEST);
                }

                if (checkOpenGLError())
                {
                    log("Failed to change depth test state");
                    return false;
                }

                currentDepthTestEnabled = enable;
            }

            return true;
        }

        bool RendererOGL::setViewport(GLint x,
                                      GLint y,
                                      GLsizei width,
                                      GLsizei height)
        {
            if (x != currentViewportX ||
                y != currentViewportY ||
                width != currentViewportWidth ||
                height != currentViewportHeight)
            {
                glViewport(x, y, width, height);
                currentViewportX = x;
                currentViewportY = y;
                currentViewportWidth = width;
                currentViewportHeight = height;

                if (checkOpenGLError())
                {
                    log("Failed to set viewport");
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::setBlendState(bool blendEnabled,
                                        GLenum modeRGB,
                                        GLenum modeAlpha,
                                        GLenum sfactorRGB,
                                        GLenum dfactorRGB,
                                        GLenum sfactorAlpha,
                                        GLenum dfactorAlpha)
        {
            if (currentBlendEnabled != blendEnabled)
            {
                if (blendEnabled)
                {
                    glEnable(GL_BLEND);
                }
                else
                {
                    glDisable(GL_BLEND);
                }

                if (checkOpenGLError())
                {
                    log("Failed to activate blend state");
                    return false;
                }

                currentBlendEnabled = blendEnabled;
            }

            if (blendEnabled)
            {
                if (currentBlendModeRGB != modeRGB ||
                    currentBlendModeAlpha != modeAlpha)
                {
                    glBlendEquationSeparate(modeRGB,
                                            modeAlpha);

                    currentBlendModeRGB = modeRGB;
                    currentBlendModeAlpha = modeAlpha;
                }

                if (currentBlendSourceFactorRGB != sfactorRGB ||
                    currentBlendDestFactorRGB != dfactorRGB ||
                    currentBlendSourceFactorAlpha != sfactorAlpha ||
                    currentBlendDestFactorAlpha != dfactorAlpha)
                {
                    glBlendFuncSeparate(sfactorRGB,
                                        dfactorRGB,
                                        sfactorAlpha,
                                        dfactorAlpha);

                    currentBlendSourceFactorRGB = sfactorRGB;
                    currentBlendDestFactorRGB = dfactorRGB;
                    currentBlendSourceFactorAlpha = sfactorAlpha;
                    currentBlendDestFactorAlpha = dfactorAlpha;
                }

                if (checkOpenGLError())
                {
                    log("Failed to set blend state");
                    return false;
                }
            }

            return true;
        }


        void RendererOGL::deleteResource(GLuint resource, ResourceType resourceType)
        {
            if (sharedEngine->isActive())
            {
                std::lock_guard<std::mutex> lock(deleteMutex);

                deleteQueue.push(std::make_pair(resource, resourceType));
            }
        }

        void RendererOGL::deleteResources()
        {
            std::pair<GLuint, ResourceType> deleteResource;

            for (;;)
            {
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
        }
    } // namespace graphics
} // namespace ouzel
