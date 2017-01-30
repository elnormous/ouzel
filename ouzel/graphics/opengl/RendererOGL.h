// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
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

#if OUZEL_PLATFORM_EMSCRIPTEN || OUZEL_PLATFORM_RASPBIAN // workaround for a typo in Emscripten's and Raspbian's gl2ext.h
    typedef void (GL_APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMG) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples);
#endif

#if OUZEL_SUPPORTS_OPENGL
#ifdef GL_ARB_vertex_array_object
    extern PFNGLGENVERTEXARRAYSPROC genVertexArraysProc;
    extern PFNGLBINDVERTEXARRAYPROC bindVertexArrayProc;
    extern PFNGLDELETEVERTEXARRAYSPROC deleteVertexArraysProc;
#endif

#ifdef GL3_PROTOTYPES
    extern PFNGLMAPBUFFERPROC mapBufferProc;
    extern PFNGLUNMAPBUFFERPROC unmapBufferProc;
#endif

#ifdef GL_ARB_map_buffer_range
    extern PFNGLMAPBUFFERRANGEPROC mapBufferRangeProc;
#endif

#ifdef GL_ARB_framebuffer_object
    extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC renderbufferStorageMultisampleProc;
#endif

#elif OUZEL_OPENGL_INTERFACE_EGL
#ifdef GL_OES_vertex_array_object
    extern PFNGLGENVERTEXARRAYSOESPROC genVertexArraysProc;
    extern PFNGLBINDVERTEXARRAYOESPROC bindVertexArrayProc;
    extern PFNGLDELETEVERTEXARRAYSOESPROC deleteVertexArraysProc;
#endif

#ifdef GL_OES_mapbuffer
    extern PFNGLMAPBUFFEROESPROC mapBufferProc;
    extern PFNGLUNMAPBUFFEROESPROC unmapBufferProc;
#endif

#ifdef GL_EXT_map_buffer_range
    extern PFNGLMAPBUFFERRANGEEXTPROC mapBufferRangeProc;
#endif

#ifdef GL_IMG_multisampled_render_to_texture
    extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMG renderbufferStorageMultisampleProc;
    extern PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMG framebufferTexture2DMultisampleProc;
#endif

#endif

#include "graphics/Renderer.h"
#include "graphics/TextureResource.h"
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

            virtual bool process() override;
            virtual bool draw() override;

            virtual std::vector<Size2> getSupportedResolutions() const override;

            virtual BlendStateResource* createBlendState() override;
            virtual TextureResource* createTexture() override;
            virtual ShaderResource* createShader() override;
            virtual MeshBufferResource* createMeshBuffer() override;
            virtual BufferResource* createBuffer() override;

            static inline bool checkOpenGLError(bool logError = true)
            {
                GLenum error = glGetError();

                if (error != GL_NO_ERROR)
                {
                    if (logError)
                    {
                        const char* errorStr;

                        switch (error)
                        {
                            case GL_INVALID_ENUM: errorStr = "GL_INVALID_ENUM"; break;
                            case GL_INVALID_VALUE: errorStr = "GL_INVALID_VALUE"; break;
                            case GL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break;
                            case GL_OUT_OF_MEMORY: errorStr = "GL_OUT_OF_MEMORY"; break;
                            case GL_INVALID_FRAMEBUFFER_OPERATION: errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
                            default: errorStr = "Unknown error"; break;
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

            static inline bool bindBuffer(GLuint bufferType, GLuint bufferId)
            {
                GLuint& currentBufferId = stateCache.bufferId[bufferType];

                if (currentBufferId != bufferId)
                {
                    glBindBuffer(bufferType, bufferId);
                    currentBufferId = bufferId;

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to bind element array buffer";
                        return false;
                    }
                }

                return true;
            }

            static inline bool bindVertexArray(GLuint vertexArrayId)
            {
                if (stateCache.vertexArrayId != vertexArrayId)
                {
#if OUZEL_OPENGL_INTERFACE_EAGL
                    glBindVertexArrayOES(vertexArrayId);
#elif OUZEL_OPENGL_INTERFACE_EGL
                    if (bindVertexArrayProc) bindVertexArrayProc(vertexArrayId);
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX
                    glBindVertexArray(vertexArrayId);
#endif
                    stateCache.vertexArrayId = vertexArrayId;
                    stateCache.bufferId[GL_ELEMENT_ARRAY_BUFFER] = 0;
                    stateCache.bufferId[GL_ARRAY_BUFFER] = 0;

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to bind vertex array";
                        return false;
                    }
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

            static inline bool depthMask(bool flag)
            {
                if (stateCache.depthMask != flag)
                {
                    glDepthMask(flag ? GL_TRUE : GL_FALSE);

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to change depth mask state";
                        return false;
                    }

                    stateCache.depthMask = flag;
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

            static void deleteBuffer(GLuint bufferId)
            {
                GLuint& elementArrayBufferId = stateCache.bufferId[GL_ELEMENT_ARRAY_BUFFER];
                if (elementArrayBufferId == bufferId) elementArrayBufferId = 0;
                GLuint& arrayBufferId = stateCache.bufferId[GL_ARRAY_BUFFER];
                if (arrayBufferId == bufferId) arrayBufferId = 0;
                glDeleteBuffers(1, &bufferId);
            }

            static void deleteVertexArray(GLuint vertexArrayId)
            {
#if OUZEL_PLATFORM_ANDROID
                bindVertexArray(0); // workaround for Android (current VAO's element array buffer is set to 0 if glDeleteVertexArrays is called on Android)
#else
                if (stateCache.vertexArrayId == vertexArrayId) stateCache.vertexArrayId = 0;
#endif

#if OUZEL_OPENGL_INTERFACE_EAGL
                glDeleteVertexArraysOES(1, &vertexArrayId);
#elif OUZEL_OPENGL_INTERFACE_EGL
                if (deleteVertexArraysProc) deleteVertexArraysProc(1, &vertexArrayId);
#else
                glDeleteVertexArrays(1, &vertexArrayId);
#endif
            }

            static void deleteRenderBuffer(GLuint renderBufferId)
            {
                glDeleteRenderbuffers(1, &renderBufferId);
            }

            static void deleteFrameBuffer(GLuint frameBufferId)
            {
                if (stateCache.frameBufferId == frameBufferId) stateCache.frameBufferId = 0;
                glDeleteFramebuffers(1, &frameBufferId);
            }

            static void deleteProgram(GLuint programId)
            {
                if (stateCache.programId == programId) stateCache.programId = 0;
                glDeleteProgram(programId);
            }

            static void deleteShader(GLuint shaderId)
            {
                glDeleteShader(shaderId);
            }

            static void deleteTexture(GLuint textureId)
            {
                for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                {
                    if (stateCache.textureId[layer] == textureId)
                    {
                        stateCache.textureId[layer] = 0;
                    }
                }
                glDeleteTextures(1, &textureId);
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

        protected:
            RendererOGL();

            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync,
                              bool newDepth) override;
            virtual bool update() override;
            virtual bool lockContext();
            virtual bool swapBuffers();

            virtual bool generateScreenshot(const std::string& filename) override;

            virtual bool createFrameBuffer();

            void* getProcAddress(const std::string& name) const;

            GLuint systemFrameBufferId = 0;

            GLsizei frameBufferWidth = 0;
            GLsizei frameBufferHeight = 0;
            GLuint frameBufferId = 0;
            GLuint colorRenderBufferId = 0;
            GLuint depthRenderBufferId = 0;

            GLbitfield clearMask = 0;
            GLfloat frameBufferClearColor[4];

            struct StateCache
            {
                GLuint textureId[Texture::LAYERS] = {0};
                GLuint programId = 0;
                GLuint frameBufferId = 0;

                std::map<GLuint, GLuint> bufferId;
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

                bool depthMask = true;
                bool depthTestEnabled = false;

                GLint viewportX = 0;
                GLint viewportY = 0;
                GLsizei viewportWidth = 0;
                GLsizei viewportHeight = 0;
            };

            static StateCache stateCache;
        };
    } // namespace graphics
} // namespace ouzel
