// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <set>
#include <queue>
#include <utility>
#include <mutex>
#include <atomic>
#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_MACOS
    #include <OpenGL/gl3.h>
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
    #if OUZEL_SUPPORTS_OPENGLES3
        #include <OpenGLES/ES3/gl.h>
        #include <OpenGLES/ES3/glext.h>
    #endif
#elif OUZEL_PLATFORM_ANDROID
    #include <GLES2/gl2platform.h>
    #define GL_GLEXT_PROTOTYPES 1
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif OUZEL_PLATFORM_LINUX
    #define GL_GLEXT_PROTOTYPES 1
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#elif OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
    #define GL_GLEXT_PROTOTYPES 1
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#endif

#if OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
extern PFNGLGENVERTEXARRAYSOESPROC genVertexArraysOES;
extern PFNGLBINDVERTEXARRAYOESPROC bindVertexArrayOES;
extern PFNGLDELETEVERTEXARRAYSOESPROC deleteVertexArraysOES;

#ifdef GL_OES_mapbuffer
    extern PFNGLMAPBUFFEROESPROC mapBufferOES;
    extern PFNGLUNMAPBUFFEROESPROC unmapBufferOES;
#endif

#ifdef GL_EXT_map_buffer_range
    extern PFNGLMAPBUFFERRANGEEXTPROC mapBufferRangeEXT;
#endif

#endif

#include "graphics/Renderer.h"
#include "graphics/Texture.h"
#include "utils/Log.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGL: public Renderer
        {
            friend Engine;
        public:
            virtual ~RendererOGL();

            virtual void setClearColor(Color color) override;
            virtual void setSize(const Size2& newSize) override;

            virtual bool present() override;

            virtual std::vector<Size2> getSupportedResolutions() const override;

            virtual BlendStatePtr createBlendState() override;
            virtual TexturePtr createTexture() override;
            virtual RenderTargetPtr createRenderTarget() override;
            virtual ShaderPtr createShader() override;
            virtual MeshBufferPtr createMeshBuffer() override;
            virtual IndexBufferPtr createIndexBuffer() override;
            virtual VertexBufferPtr createVertexBuffer() override;

            static inline bool checkOpenGLError(bool logError = true)
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

                        Log(Log::Level::ERR) << "OpenGL error: " << errorStr << " (" << error << ")";
                    }

                    return true;
                }

                return false;
            }

            static inline bool bindTexture(GLuint textureId, uint32_t layer)
            {
                if (stateCache.textureId[layer] != textureId)
                {
                    glActiveTexture(GL_TEXTURE0 + layer);
                    glBindTexture(GL_TEXTURE_2D, textureId);
                    stateCache.textureId[layer] = textureId;

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to bind texture";
                        return false;
                    }
                }

                return true;
            }

            static inline bool useProgram(GLuint programId)
            {
                if (stateCache.programId != programId)
                {
                    glUseProgram(programId);
                    stateCache.programId = programId;

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to bind program";
                        return false;
                    }
                }

                return true;
            }

            static inline bool bindFrameBuffer(GLuint frameBufferId)
            {
                if (stateCache.frameBufferId != frameBufferId)
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
                    stateCache.frameBufferId = frameBufferId;

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to bind frame buffer";
                        return false;
                    }
                }

                return true;
            }

            static inline bool bindElementArrayBuffer(GLuint elementArrayBufferId)
            {
                if (stateCache.elementArrayBufferId != elementArrayBufferId)
                {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferId);
                    stateCache.elementArrayBufferId = elementArrayBufferId;

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to bind element array buffer";
                        return false;
                    }
                }

                return true;
            }

            static inline bool bindArrayBuffer(GLuint arrayBufferId)
            {
                if (stateCache.arrayBufferId != arrayBufferId)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferId);
                    stateCache.arrayBufferId = arrayBufferId;

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to bind array buffer";
                        return false;
                    }
                }

                return true;
            }

            static inline bool bindVertexArray(GLuint vertexArrayId)
            {
                if (stateCache.vertexArrayId != vertexArrayId)
                {
#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                    glBindVertexArrayOES(vertexArrayId);
#elif OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
                    if (bindVertexArrayOES) bindVertexArrayOES(vertexArrayId);
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX
                    glBindVertexArray(vertexArrayId);
#endif
                    stateCache.vertexArrayId = vertexArrayId;
                    stateCache.elementArrayBufferId = 0;
                    stateCache.arrayBufferId = 0;

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to bind vertex array";
                        return false;
                    }
                }

                return true;
            }

            static inline bool unbindTexture(GLuint textureId)
            {
                for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                {
                    if (stateCache.textureId[layer] == textureId)
                    {
                        bindTexture(0, layer);
                    }
                }

                return true;
            }

            static inline bool unuseProgram(GLuint programId)
            {
                if (stateCache.programId == programId)
                {
                    return useProgram(0);
                }

                return true;
            }

            static inline bool unbindFrameBuffer(GLuint frameBufferId)
            {
                if (stateCache.frameBufferId == frameBufferId)
                {
                    return bindFrameBuffer(0);
                }

                return true;
            }

            static inline bool unbindElementArrayBuffer(GLuint elementArrayBufferId)
            {
                if (stateCache.elementArrayBufferId == elementArrayBufferId)
                {
                    return bindElementArrayBuffer(0);
                }

                return true;
            }

            static inline bool unbindArrayBuffer(GLuint arrayBufferId)
            {
                if (stateCache.arrayBufferId == arrayBufferId)
                {
                    return bindArrayBuffer(0);
                }

                return true;
            }

            static inline bool unbindVertexArray(GLuint vertexArrayId)
            {
                if (stateCache.vertexArrayId == vertexArrayId)
                {
                    return bindVertexArray(0);
                }

                return true;
            }

            static inline bool setScissorTest(bool scissorTestEnabled,
                                              GLint x,
                                              GLint y,
                                              GLsizei width,
                                              GLsizei height)
            {
                if (stateCache.scissorTestEnabled != scissorTestEnabled)
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
                        Log(Log::Level::ERR) << "Failed to set scissor test";
                        return false;
                    }

                    stateCache.scissorTestEnabled = scissorTestEnabled;
                }

                if (scissorTestEnabled)
                {
                    if (stateCache.scissorX != x ||
                        stateCache.scissorY != y ||
                        stateCache.scissorWidth != width ||
                        stateCache.scissorHeight != height)
                    {
                        glScissor(x, y, width, height);
                        stateCache.scissorX = x;
                        stateCache.scissorY = y;
                        stateCache.scissorWidth = width;
                        stateCache.scissorHeight = height;
                    }

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to set scissor test";
                        return false;
                    }
                }

                return true;
            }

            static inline bool enableDepthTest(bool enable)
            {
                if (stateCache.depthTestEnabled != enable)
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
                        Log(Log::Level::ERR) << "Failed to change depth test state";
                        return false;
                    }

                    stateCache.depthTestEnabled = enable;
                }

                return true;
            }

            static inline bool setViewport(GLint x,
                                           GLint y,
                                           GLsizei width,
                                           GLsizei height)
            {
                if (stateCache.viewportX != x ||
                    stateCache.viewportY != y ||
                    stateCache.viewportWidth != width ||
                    stateCache.viewportHeight != height)
                {
                    glViewport(x, y, width, height);
                    stateCache.viewportX = x;
                    stateCache.viewportY = y;
                    stateCache.viewportWidth = width;
                    stateCache.viewportHeight = height;

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to set viewport";
                        return false;
                    }
                }

                return true;
            }

            static inline bool setBlendState(bool blendEnabled,
                                             GLenum modeRGB,
                                             GLenum modeAlpha,
                                             GLenum sfactorRGB,
                                             GLenum dfactorRGB,
                                             GLenum sfactorAlpha,
                                             GLenum dfactorAlpha)
            {
                bool checkError = false;

                if (stateCache.blendEnabled != blendEnabled)
                {
                    if (blendEnabled)
                    {
                        glEnable(GL_BLEND);
                    }
                    else
                    {
                        glDisable(GL_BLEND);
                    }

                    stateCache.blendEnabled = blendEnabled;

                    checkError = true;
                }

                if (blendEnabled)
                {
                    if (stateCache.blendModeRGB != modeRGB ||
                        stateCache.blendModeAlpha != modeAlpha)
                    {
                        glBlendEquationSeparate(modeRGB,
                                                modeAlpha);

                        stateCache.blendModeRGB = modeRGB;
                        stateCache.blendModeAlpha = modeAlpha;

                        checkError = true;
                    }

                    if (stateCache.blendSourceFactorRGB != sfactorRGB ||
                        stateCache.blendDestFactorRGB != dfactorRGB ||
                        stateCache.blendSourceFactorAlpha != sfactorAlpha ||
                        stateCache.blendDestFactorAlpha != dfactorAlpha)
                    {
                        glBlendFuncSeparate(sfactorRGB,
                                            dfactorRGB,
                                            sfactorAlpha,
                                            dfactorAlpha);

                        stateCache.blendSourceFactorRGB = sfactorRGB;
                        stateCache.blendDestFactorRGB = dfactorRGB;
                        stateCache.blendSourceFactorAlpha = sfactorAlpha;
                        stateCache.blendDestFactorAlpha = dfactorAlpha;

                        checkError = true;
                    }
                }

                if (checkError && checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to set blend state";
                    return false;
                }

                return true;
            }

#ifdef OUZEL_SUPPORTS_OPENGL
            static inline bool setPolygonFillMode(GLenum polygonFillMode)
            {
                if (stateCache.polygonFillMode != polygonFillMode)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, polygonFillMode);

                    stateCache.polygonFillMode = polygonFillMode;

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to set blend state";
                        return false;
                    }
                }

                return true;
            }
#endif

            enum class ResourceType
            {
                Buffer,
                VertexArray,
                RenderBuffer,
                FrameBuffer,
                Program,
                Shader,
                Texture
            };
            static void deleteResource(GLuint resource, ResourceType resourceType);

        protected:
            RendererOGL();

            virtual bool init(Window* newWindow,
                              uint32_t newSampleCount,
                              TextureFilter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync) override;
            virtual bool update();

            virtual bool saveScreenshots();

            bool createMSAAFrameBuffer();

            static void deleteResources();

            GLuint frameBufferId = 0;
            GLsizei frameBufferWidth = 0;
            GLsizei frameBufferHeight = 0;
            GLuint msaaTextureId = 0;
            GLuint msaaFrameBufferId = 0;
            GLuint msaaRenderBufferId = 0;

            GLbitfield clearMask = 0;
            GLfloat frameBufferClearColor[4];

            struct StateCache
            {
                GLuint textureId[Texture::LAYERS] = { 0 };
                GLuint programId = 0;
                GLuint frameBufferId = 0;

                GLuint elementArrayBufferId = 0;
                GLuint arrayBufferId = 0;
                GLuint vertexArrayId = 0;

                bool blendEnabled = false;
                GLenum blendModeRGB = 0;
                GLenum blendModeAlpha = 0;
                GLenum blendSourceFactorRGB = 0;
                GLenum blendDestFactorRGB = 0;
                GLenum blendSourceFactorAlpha = 0;
                GLenum blendDestFactorAlpha = 0;

#ifdef OUZEL_SUPPORTS_OPENGL
                GLenum polygonFillMode = GL_FILL;
#endif

                bool scissorTestEnabled = false;
                GLint scissorX = 0;
                GLint scissorY = 0;
                GLsizei scissorWidth = 0;
                GLsizei scissorHeight = 0;

                bool depthTestEnabled = false;

                GLint viewportX = 0;
                GLint viewportY = 0;
                GLsizei viewportWidth = 0;
                GLsizei viewportHeight = 0;
            };

            static StateCache stateCache;

            static std::queue<std::pair<GLuint, ResourceType>> deleteQueue;
            static std::mutex deleteMutex;

            std::atomic<bool> dirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
