// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <sstream>
#include <iterator>

#include "RendererOGL.h"
#include "TextureOGL.h"
#include "RenderTargetOGL.h"
#include "ShaderOGL.h"
#include "MeshBufferOGL.h"
#include "IndexBufferOGL.h"
#include "VertexBufferOGL.h"
#include "BlendStateOGL.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "core/Cache.h"
#include "utils/Log.h"
#include "stb_image_write.h"

#if OUZEL_SUPPORTS_OPENGL
#include "ColorPSGL2.h"
#include "ColorVSGL2.h"
#include "TexturePSGL2.h"
#include "TextureVSGL2.h"
#if OUZEL_SUPPORTS_OPENGL3
#include "ColorPSGL3.h"
#include "ColorVSGL3.h"
#include "TexturePSGL3.h"
#include "TextureVSGL3.h"
#endif
#endif

#if OUZEL_SUPPORTS_OPENGLES
#include "ColorPSGLES2.h"
#include "ColorVSGLES2.h"
#include "TexturePSGLES2.h"
#include "TextureVSGLES2.h"
#if OUZEL_SUPPORTS_OPENGLES3
#include "ColorPSGLES3.h"
#include "ColorVSGLES3.h"
#include "TexturePSGLES3.h"
#include "TextureVSGLES3.h"
#endif
#endif

#if OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
PFNGLGENVERTEXARRAYSOESPROC genVertexArraysOES = nullptr;
PFNGLBINDVERTEXARRAYOESPROC bindVertexArrayOES = nullptr;
PFNGLDELETEVERTEXARRAYSOESPROC deleteVertexArraysOES = nullptr;

#ifdef GL_OES_mapbuffer
    PFNGLMAPBUFFEROESPROC mapBufferOES;
    PFNGLUNMAPBUFFEROESPROC unmapBufferOES;
#endif

#ifdef GL_EXT_map_buffer_range
    PFNGLMAPBUFFERRANGEEXTPROC mapBufferRangeEXT;
#endif

#endif

namespace ouzel
{
    namespace graphics
    {
        RendererOGL::RendererOGL():
            Renderer(Driver::OPENGL), dirty(false)
        {
            msaaRenderBufferId = 0;

#if OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
            genVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
            bindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
            deleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");

#ifdef GL_OES_mapbuffer
            mapBufferOES = (PFNGLMAPBUFFEROESPROC)eglGetProcAddress("glMapBufferOES");
            unmapBufferOES = (PFNGLUNMAPBUFFEROESPROC)eglGetProcAddress("glUnmapBufferOES");
#endif

#ifdef GL_EXT_map_buffer_range
            mapBufferRangeEXT = (PFNGLMAPBUFFERRANGEEXTPROC)eglGetProcAddress("glMapBufferRangeEXT");
#endif

#endif
        }

        RendererOGL::~RendererOGL()
        {
            if (msaaTextureId)
            {
                glDeleteTextures(1, &msaaTextureId);
            }

            if (msaaRenderBufferId)
            {
                glDeleteRenderbuffers(1, &msaaRenderBufferId);
            }

            if (msaaFrameBufferId)
            {
                glDeleteFramebuffers(1, &msaaFrameBufferId);
            }
        }

        bool RendererOGL::init(Window* newWindow,
                               uint32_t newSampleCount,
                               TextureFilter newTextureFilter,
                               PixelFormat newBackBufferFormat,
                               bool newVerticalSync)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Renderer::init(newWindow, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync))
            {
                return false;
            }

            //const GLubyte* deviceVendor = glGetString(GL_VENDOR);
            const GLubyte* deviceName = glGetString(GL_RENDERER);

            if (checkOpenGLError() || !deviceName)
            {
                Log(Log::Level::WARN) << "Failed to get OpenGL renderer";
            }
            else
            {
                Log(Log::Level::INFO) << "Using " << reinterpret_cast<const char*>(deviceName) << " for rendering";
            }

#if OUZEL_SUPPORTS_OPENGLES
            npotTexturesSupported = (apiMajorVersion >= 3);

            const GLubyte* extensionPtr = glGetString(GL_EXTENSIONS);

            if (checkOpenGLError() || !extensionPtr)
            {
                Log(Log::Level::WARN) << "Failed to get OpenGL extensions";
            }
            else
            {
                std::string extensions(reinterpret_cast<const char*>(extensionPtr));

                std::istringstream extensionStringStream(extensions);

                for (std::string extension; extensionStringStream >> extensions;)
                {
                    if (extension == "GL_OES_texture_npot")
                    {
                        npotTexturesSupported = true;
                    }
                }
            }
#endif
            renderTargetsFlipped = true;

            frameBufferWidth = static_cast<GLsizei>(size.width);
            frameBufferHeight = static_cast<GLsizei>(size.height);

            if (sampleCount > 1)
            {
#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                if (!createMSAAFrameBuffer())
                {
                    return false;
                }
#else
                Log(Log::Level::WARN) << "Multisample anti-aliasing is disabled for OpenGL";
#endif
            }

            //glEnable(GL_DEPTH_TEST);

            ShaderPtr textureShader = createShader();

            switch (apiMajorVersion)
            {
                case 2:
#if OUZEL_SUPPORTS_OPENGL
                    textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TextureGL2_ps), std::end(TextureGL2_ps)),
                                                   std::vector<uint8_t>(std::begin(TextureGL2_vs), std::end(TextureGL2_vs)),
                                                   VertexPCT::ATTRIBUTES,
                                                   {{"color", 4 * sizeof(float)}},
                                                   {{"modelViewProj", sizeof(Matrix4)}});
#elif OUZEL_SUPPORTS_OPENGLES
                    textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TextureGLES2_ps), std::end(TextureGLES2_ps)),
                                                   std::vector<uint8_t>(std::begin(TextureGLES2_vs), std::end(TextureGLES2_vs)),
                                                   VertexPCT::ATTRIBUTES,
                                                   {{"color", 4 * sizeof(float)}},
                                                   {{"modelViewProj", sizeof(Matrix4)}});
#endif
                    break;
                case 3:
#if OUZEL_SUPPORTS_OPENGL3
                    textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TextureGL3_ps), std::end(TextureGL3_ps)),
                                                   std::vector<uint8_t>(std::begin(TextureGL3_vs), std::end(TextureGL3_vs)),
                                                   VertexPCT::ATTRIBUTES,
                                                   {{"color", 4 * sizeof(float)}},
                                                   {{"modelViewProj", sizeof(Matrix4)}});
#elif OUZEL_SUPPORTS_OPENGLES3
                    textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TextureGLES3_ps), std::end(TextureGLES3_ps)),
                                                   std::vector<uint8_t>(std::begin(TextureGLES3_vs), std::end(TextureGLES3_vs)),
                                                   VertexPCT::ATTRIBUTES,
                                                   {{"color", 4 * sizeof(float)}},
                                                   {{"modelViewProj", sizeof(Matrix4)}});
#endif
                    break;
                default:
                    Log(Log::Level::ERR) << "Unsupported OpenGL version";
                    return false;
            }

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader = createShader();

            switch (apiMajorVersion)
            {
                case 2:
#if OUZEL_SUPPORTS_OPENGL
                    colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(ColorGL2_ps), std::end(ColorGL2_ps)),
                                                 std::vector<uint8_t>(std::begin(ColorGL2_vs), std::end(ColorGL2_vs)),
                                                 VertexPC::ATTRIBUTES,
                                                 {{"color", 4 * sizeof(float)}},
                                                 {{"modelViewProj", sizeof(Matrix4)}});
#elif OUZEL_SUPPORTS_OPENGLES
                    colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(ColorGLES2_ps), std::end(ColorGLES2_ps)),
                                                 std::vector<uint8_t>(std::begin(ColorGLES2_vs), std::end(ColorGLES2_vs)),
                                                 VertexPC::ATTRIBUTES,
                                                 {{"color", 4 * sizeof(float)}},
                                                 {{"modelViewProj", sizeof(Matrix4)}});
#endif
                    break;
                case 3:
#if OUZEL_SUPPORTS_OPENGL3
                    colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(ColorGL3_ps), std::end(ColorGL3_ps)),
                                                 std::vector<uint8_t>(std::begin(ColorGL3_vs), std::end(ColorGL3_vs)),
                                                 VertexPC::ATTRIBUTES,
                                                 {{"color", 4 * sizeof(float)}},
                                                 {{"modelViewProj", sizeof(Matrix4)}});
#elif OUZEL_SUPPORTS_OPENGLES3
                    colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(ColorGLES3_ps), std::end(ColorGLES3_ps)),
                                                 std::vector<uint8_t>(std::begin(ColorGLES3_vs), std::end(ColorGLES3_vs)),
                                                 VertexPC::ATTRIBUTES,
                                                 {{"color", 4 * sizeof(float)}},
                                                 {{"modelViewProj", sizeof(Matrix4)}});
#endif
                    break;
                default:
                    Log(Log::Level::ERR) << "Unsupported OpenGL version";
                    return false;
            }

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

            TexturePtr whitePixelTexture = createTexture();
            whitePixelTexture->initFromBuffer( { 255, 255, 255, 255 }, Size2(1.0f, 1.0f), false, false);
            sharedEngine->getCache()->setTexture(TEXTURE_WHITE_PIXEL, whitePixelTexture);

            dirty = true;
            ready = true;

            return true;
        }

        bool RendererOGL::update()
        {
            if (dirty)
            {
                Size2 newSize;

                {
                    std::lock_guard<std::mutex> lock(dataMutex);

                    newSize = size;

                    clearMask = GL_COLOR_BUFFER_BIT;

                    frameBufferClearColor[0] = clearColor.getR();
                    frameBufferClearColor[1] = clearColor.getG();
                    frameBufferClearColor[2] = clearColor.getB();
                    frameBufferClearColor[3] = clearColor.getA();

                    dirty = false;
                }

                if (frameBufferWidth != static_cast<GLsizei>(newSize.width) ||
                    frameBufferHeight != static_cast<GLsizei>(newSize.height))
                {
                    frameBufferWidth = static_cast<GLsizei>(newSize.width);
                    frameBufferHeight = static_cast<GLsizei>(newSize.height);

                    if (sampleCount > 1)
                    {
#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                        if (!createMSAAFrameBuffer())
                        {
                            return false;
                        }
#endif
                    }
                }
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

            deleteResources();

            if (!update())
            {
                return false;
            }

            if (drawQueue.empty())
            {
                frameBufferClearedFrame = currentFrame;

                if (clear)
                {
                    GLuint clearFrameBufferId = 0;

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                    if (sampleCount > 1)
                    {
                        clearFrameBufferId = msaaFrameBufferId;
                    }
                    else
#endif
                    {
                        clearFrameBufferId = frameBufferId;
                    }

                    if (!bindFrameBuffer(clearFrameBufferId))
                    {
                        return false;
                    }

                    if (!setViewport(0, 0,
                                     frameBufferWidth,
                                     frameBufferHeight))
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
                        Log(Log::Level::ERR) << "Failed to clear frame buffer";
                        return false;
                    }
                }
            }
            else for (const DrawCommand& drawCommand : drawQueue)
            {
#ifndef OUZEL_SUPPORTS_OPENGL
                if (drawCommand.wireframe)
                {
                    continue;
                }
#endif

                // blend state
                std::shared_ptr<BlendStateOGL> blendStateOGL = std::static_pointer_cast<BlendStateOGL>(drawCommand.blendState);

                if (!blendStateOGL)
                {
                    // don't render if invalid blend state
                    continue;
                }

                if (!setBlendState(blendStateOGL->isGLBlendEnabled(),
                                   blendStateOGL->getModeRGB(),
                                   blendStateOGL->getModeAlpha(),
                                   blendStateOGL->getSourceFactorRGB(),
                                   blendStateOGL->getDestFactorRGB(),
                                   blendStateOGL->getSourceFactorAlpha(),
                                   blendStateOGL->getDestFactorAlpha()))
                {
                    return false;
                }

                bool texturesValid = true;

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
                        if (!textureOGL->getTextureId())
                        {
                            texturesValid = false;
                            break;
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

                if (!texturesValid)
                {
                    continue;
                }

                // shader
                std::shared_ptr<ShaderOGL> shaderOGL = std::static_pointer_cast<ShaderOGL>(drawCommand.shader);

                if (!shaderOGL || !shaderOGL->getProgramId())
                {
                    // don't render if invalid shader
                    continue;
                }

                useProgram(shaderOGL->getProgramId());

                // pixel shader constants
                const std::vector<ShaderOGL::Location>& pixelShaderConstantLocations = shaderOGL->getPixelShaderConstantLocations();

                if (drawCommand.pixelShaderConstants.size() > pixelShaderConstantLocations.size())
                {
                    Log(Log::Level::ERR) << "Invalid pixel shader constant size";
                    return false;
                }

                for (size_t i = 0; i < drawCommand.pixelShaderConstants.size(); ++i)
                {
                    const ShaderOGL::Location& pixelShaderConstantLocation = pixelShaderConstantLocations[i];
                    const std::vector<float>& pixelShaderConstant = drawCommand.pixelShaderConstants[i];

                    uint32_t components = pixelShaderConstantLocation.size / 4;

                    switch (components)
                    {
                        case 1:
                            glUniform1fv(pixelShaderConstantLocation.location, static_cast<GLsizei>(pixelShaderConstant.size() / components), pixelShaderConstant.data());
                            break;
                        case 2:
                            glUniform2fv(pixelShaderConstantLocation.location, static_cast<GLsizei>(pixelShaderConstant.size() / components), pixelShaderConstant.data());
                            break;
                        case 3:
                            glUniform3fv(pixelShaderConstantLocation.location, static_cast<GLsizei>(pixelShaderConstant.size() / components), pixelShaderConstant.data());
                            break;
                        case 4:
                            glUniform4fv(pixelShaderConstantLocation.location, static_cast<GLsizei>(pixelShaderConstant.size() / components), pixelShaderConstant.data());
                            break;
                        case 9:
                            glUniformMatrix3fv(pixelShaderConstantLocation.location, static_cast<GLsizei>(pixelShaderConstant.size() / components), GL_FALSE, pixelShaderConstant.data());
                            break;
                        case 16:
                            glUniformMatrix4fv(pixelShaderConstantLocation.location, static_cast<GLsizei>(pixelShaderConstant.size() / components), GL_FALSE, pixelShaderConstant.data());
                            break;
                        default:
                            Log(Log::Level::ERR) << "Unsupported uniform size";
                            return false;
                    }
                }

                // vertex shader constants
                const std::vector<ShaderOGL::Location>& vertexShaderConstantLocations = shaderOGL->getVertexShaderConstantLocations();

                if (drawCommand.vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                {
                    Log(Log::Level::ERR) << "Invalid vertex shader constant size";
                    return false;
                }

                for (size_t i = 0; i < drawCommand.vertexShaderConstants.size(); ++i)
                {
                    const ShaderOGL::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                    const std::vector<float>& vertexShaderConstant = drawCommand.vertexShaderConstants[i];

                    uint32_t components = vertexShaderConstantLocation.size / 4;

                    switch (components)
                    {
                        case 1:
                            glUniform1fv(vertexShaderConstantLocation.location, static_cast<GLsizei>(vertexShaderConstant.size() / components), vertexShaderConstant.data());
                            break;
                        case 2:
                            glUniform2fv(vertexShaderConstantLocation.location, static_cast<GLsizei>(vertexShaderConstant.size() / components), vertexShaderConstant.data());
                            break;
                        case 3:
                            glUniform3fv(vertexShaderConstantLocation.location, static_cast<GLsizei>(vertexShaderConstant.size() / components), vertexShaderConstant.data());
                            break;
                        case 4:
                            glUniform4fv(vertexShaderConstantLocation.location, static_cast<GLsizei>(vertexShaderConstant.size() / components), vertexShaderConstant.data());
                            break;
                        case 9:
                            glUniformMatrix3fv(vertexShaderConstantLocation.location, static_cast<GLsizei>(vertexShaderConstant.size() / components), GL_FALSE, vertexShaderConstant.data());
                            break;
                        case 16:
                            glUniformMatrix4fv(vertexShaderConstantLocation.location, static_cast<GLsizei>(vertexShaderConstant.size() / components), GL_FALSE, vertexShaderConstant.data());
                            break;
                        default:
                            Log(Log::Level::ERR) << "Unsupported uniform size";
                            return false;
                    }
                }

                // render target
                GLuint newFrameBufferId = 0;
                GLbitfield newClearMask = 0;
                const float* newClearColor;
                bool clearBuffer = false;

                if (drawCommand.renderTarget)
                {
                    std::shared_ptr<RenderTargetOGL> renderTargetOGL = std::static_pointer_cast<RenderTargetOGL>(drawCommand.renderTarget);

                    if (!renderTargetOGL->getFrameBufferId())
                    {
                        continue;
                    }

                    newFrameBufferId = renderTargetOGL->getFrameBufferId();
                    newClearMask = renderTargetOGL->getClearMask();
                    newClearColor = renderTargetOGL->getFrameBufferClearColor();

                    if (renderTargetOGL->getFrameBufferClearedFrame() != currentFrame)
                    {
                        renderTargetOGL->setFrameBufferClearedFrame(currentFrame);
                        clearBuffer = renderTargetOGL->getClear();
                    }
                }
                else
                {
#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                    if (sampleCount > 1)
                    {
                        newFrameBufferId = msaaFrameBufferId;
                    }
                    else
#endif
                    {
                        newFrameBufferId = frameBufferId;
                    }

                    newClearMask = clearMask;
                    newClearColor = frameBufferClearColor;

                    if (frameBufferClearedFrame != currentFrame)
                    {
                        frameBufferClearedFrame = currentFrame;
                        clearBuffer = clear;
                    }
                }

                if (!bindFrameBuffer(newFrameBufferId))
                {
                    return false;
                }

                setViewport(static_cast<GLint>(drawCommand.viewport.x),
                            static_cast<GLint>(drawCommand.viewport.y),
                            static_cast<GLsizei>(drawCommand.viewport.width),
                            static_cast<GLsizei>(drawCommand.viewport.height));

                if (clearBuffer)
                {
                    glClearColor(newClearColor[0],
                                 newClearColor[1],
                                 newClearColor[2],
                                 newClearColor[3]);

                    glClear(newClearMask);

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to clear frame buffer";
                        return false;
                    }
                }

#ifdef OUZEL_SUPPORTS_OPENGL
                setPolygonFillMode(drawCommand.wireframe ? GL_LINE : GL_FILL);
#endif

                // scissor test
                setScissorTest(drawCommand.scissorTestEnabled,
                               static_cast<GLint>(drawCommand.scissorTest.x),
                               static_cast<GLint>(drawCommand.scissorTest.y),
                               static_cast<GLsizei>(drawCommand.scissorTest.width),
                               static_cast<GLsizei>(drawCommand.scissorTest.height));

                // mesh buffer
                std::shared_ptr<MeshBufferOGL> meshBufferOGL = std::static_pointer_cast<MeshBufferOGL>(drawCommand.meshBuffer);

                if (!meshBufferOGL)
                {
                    // don't render if invalid mesh buffer
                    continue;
                }

                std::shared_ptr<IndexBufferOGL> indexBufferOGL = std::static_pointer_cast<IndexBufferOGL>(meshBufferOGL->getIndexBuffer());
                std::shared_ptr<VertexBufferOGL> vertexBufferOGL = std::static_pointer_cast<VertexBufferOGL>(meshBufferOGL->getVertexBuffer());

                if (!indexBufferOGL || !indexBufferOGL->getBufferId() ||
                    !vertexBufferOGL || !vertexBufferOGL->getBufferId())
                {
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
                    default: Log(Log::Level::ERR) << "Invalid draw mode"; return false;
                }

                if (!meshBufferOGL->bindBuffers())
                {
                    return false;
                }

                glDrawElements(mode,
                               static_cast<GLsizei>(drawCommand.indexCount),
                               indexBufferOGL->getType(),
                               static_cast<const char*>(nullptr) + (drawCommand.startIndex * indexBufferOGL->getBytesPerIndex()));

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to draw elements";
                    return false;
                }
            }

            if (sampleCount > 1)
            {
#if OUZEL_PLATFORM_MACOS
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId); // draw to frame buffer
                glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFrameBufferId); // read from multisampled FBO
                glDrawBuffer(GL_BACK); // set the back buffer as the draw buffer

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to bind MSAA frame buffer";
                    return false;
                }

                glBlitFramebuffer(0, 0, frameBufferWidth, frameBufferHeight,
                                  0, 0, frameBufferWidth, frameBufferHeight,
                                  GL_COLOR_BUFFER_BIT, GL_NEAREST);

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to blit MSAA texture";
                    return false;
                }
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, frameBufferId); // draw to frame buffer
                glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, msaaFrameBufferId); // read from multisampled FBO

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to bind MSAA frame buffer";
                    return false;
                }

                if (apiMajorVersion >= 3)
                {
                    glBlitFramebuffer(0, 0, frameBufferWidth, frameBufferHeight,
                                      0, 0, frameBufferWidth, frameBufferHeight,
                                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
                }
                else
                {
                    glResolveMultisampleFramebufferAPPLE();
                }

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to blit MSAA texture";
                    return false;
                }
#endif

                // reset framebuffer
#if OUZEL_PLATFORM_MACOS
                glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
                stateCache.frameBufferId = frameBufferId;
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS

                const GLenum discard[] = { GL_COLOR_ATTACHMENT0 /*, GL_DEPTH_ATTACHMENT */ };
                glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, discard);

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to discard render buffers";
                    return false;
                }

                stateCache.frameBufferId = 0;

                glBindRenderbuffer(GL_RENDERBUFFER, frameBufferId);

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to set render buffer";
                    return false;
                }
#endif
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
            std::shared_ptr<BlendStateOGL> blendState = std::make_shared<BlendStateOGL>();
            return blendState;
        }

        TexturePtr RendererOGL::createTexture()
        {
            std::shared_ptr<TextureOGL> texture = std::make_shared<TextureOGL>();
            return texture;
        }

        RenderTargetPtr RendererOGL::createRenderTarget()
        {
            std::shared_ptr<RenderTargetOGL> renderTarget = std::make_shared<RenderTargetOGL>();
            return renderTarget;
        }

        ShaderPtr RendererOGL::createShader()
        {
            std::shared_ptr<ShaderOGL> shader(new ShaderOGL());
            return shader;
        }

        MeshBufferPtr RendererOGL::createMeshBuffer()
        {
            std::shared_ptr<MeshBufferOGL> meshBuffer = std::make_shared<MeshBufferOGL>();
            return meshBuffer;
        }

        IndexBufferPtr RendererOGL::createIndexBuffer()
        {
            std::shared_ptr<IndexBufferOGL> meshBuffer = std::make_shared<IndexBufferOGL>();
            return meshBuffer;
        }

        VertexBufferPtr RendererOGL::createVertexBuffer()
        {
            std::shared_ptr<VertexBufferOGL> meshBuffer = std::make_shared<VertexBufferOGL>();
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

                const GLsizei width = frameBufferWidth;
                const GLsizei height = frameBufferHeight;
                const GLsizei depth = 4;

                std::vector<uint8_t> data(static_cast<size_t>(width * height * depth));

                glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to read pixels from frame buffer";
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
                    Log(Log::Level::ERR) << "Failed to save image to file";
                    return false;
                }
            }

            return true;
        }

        bool RendererOGL::createMSAAFrameBuffer()
        {
#if OUZEL_PLATFORM_MACOS
            glEnable(GL_MULTISAMPLE);

            if (!msaaTextureId)
            {
                glGenTextures(1, &msaaTextureId);
            }

            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaaTextureId);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, static_cast<GLsizei>(sampleCount), GL_RGBA,
                                    frameBufferWidth, frameBufferHeight, false);

            if (checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to initialize MSAA color buffer";
                return false;
            }

            if (!msaaFrameBufferId)
            {
                glGenFramebuffers(1, &msaaFrameBufferId);
            }

            graphics::RendererOGL::bindFrameBuffer(msaaFrameBufferId);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaaTextureId, 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                Log(Log::Level::ERR) << "Failed to create framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER);
                return false;
            }
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
            if (!msaaRenderBufferId)
            {
                glGenRenderbuffers(1, &msaaRenderBufferId);
            }

            glBindRenderbuffer(GL_RENDERBUFFER, msaaRenderBufferId);

            glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, static_cast<GLsizei>(sampleCount), GL_RGBA8_OES, frameBufferWidth, frameBufferHeight);

            if (checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to initialize MSAA color buffer";
                return false;
            }

            if (!msaaFrameBufferId)
            {
                glGenFramebuffers(1, &msaaFrameBufferId);
            }

            graphics::RendererOGL::bindFrameBuffer(msaaFrameBufferId);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaRenderBufferId);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                Log(Log::Level::ERR) << "Failed to create framebuffer object " << glCheckFramebufferStatus(GL_FRAMEBUFFER);
                return false;
            }
#endif
            return true;
        }

        RendererOGL::StateCache RendererOGL::stateCache;
        std::queue<std::pair<GLuint, RendererOGL::ResourceType>> RendererOGL::deleteQueue;
        std::mutex RendererOGL::deleteMutex;

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
#elif OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
                        if (deleteVertexArraysOES) deleteVertexArraysOES(1, &deleteResource.first);
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
                        unuseProgram(deleteResource.first);
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
