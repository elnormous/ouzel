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
#elif OUZEL_PLATFORM_RASPBIAN
    #define GL_GLEXT_PROTOTYPES 1
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#endif

#if OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT;
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT;
#endif

#include "graphics/Renderer.h"
#include "graphics/Texture.h"
#include "utils/Utils.h"

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

            virtual bool present() override;

            virtual std::vector<Size2> getSupportedResolutions() const override;

            virtual BlendStatePtr createBlendState() override;
            virtual TexturePtr createTexture() override;
            virtual RenderTargetPtr createRenderTarget() override;
            virtual ShaderPtr createShader() override;
            virtual MeshBufferPtr createMeshBuffer() override;

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

                        log(LOG_LEVEL_ERROR, "OpenGL error: %s (%x)", errorStr, error);
                    }

                    return true;
                }
                
                return false;
            }

            static inline bool bindTexture(GLuint textureId, uint32_t layer)
            {
                if (currentTextureId[layer] != textureId)
                {
                    glActiveTexture(GL_TEXTURE0 + layer);
                    glBindTexture(GL_TEXTURE_2D, textureId);
                    currentTextureId[layer] = textureId;

                    if (checkOpenGLError())
                    {
                        log(LOG_LEVEL_ERROR, "Failed to bind texture");
                        return false;
                    }
                }
                
                return true;
            }

            static inline bool useProgram(GLuint programId)
            {
                if (currentProgramId != programId)
                {
                    glUseProgram(programId);
                    currentProgramId = programId;

                    if (checkOpenGLError())
                    {
                        log(LOG_LEVEL_ERROR, "Failed to bind program");
                        return false;
                    }
                }
                
                return true;
            }

            static inline bool bindFrameBuffer(GLuint frameBufferId)
            {
                if (currentFrameBufferId != frameBufferId)
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
                    currentFrameBufferId = frameBufferId;

                    if (checkOpenGLError())
                    {
                        log(LOG_LEVEL_ERROR, "Failed to bind frame buffer");
                        return false;
                    }
                }
                
                return true;
            }

            static inline bool bindElementArrayBuffer(GLuint elementArrayBufferId)
            {
                if (currentElementArrayBufferId != elementArrayBufferId)
                {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferId);
                    currentElementArrayBufferId = elementArrayBufferId;

                    if (checkOpenGLError())
                    {
                        log(LOG_LEVEL_ERROR, "Failed to bind element array buffer");
                        return false;
                    }
                }
                
                return true;
            }

            static inline bool bindArrayBuffer(GLuint arrayBufferId)
            {
                if (currentArrayBufferId != arrayBufferId)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferId);
                    currentArrayBufferId = arrayBufferId;

                    if (checkOpenGLError())
                    {
                        log(LOG_LEVEL_ERROR, "Failed to bind array buffer");
                        return false;
                    }
                }
                
                return true;
            }

            static inline bool bindVertexArray(GLuint vertexArrayId)
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
                    currentArrayBufferId = 0;

                    if (checkOpenGLError())
                    {
                        log(LOG_LEVEL_ERROR, "Failed to bind vertex array");
                        return false;
                    }
                }
                
                return true;
            }

            static inline bool unbindTexture(GLuint textureId)
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

            static inline bool unuseProgram(GLuint programId)
            {
                if (currentProgramId == programId)
                {
                    return useProgram(0);
                }

                return true;
            }

            static inline bool unbindFrameBuffer(GLuint frameBufferId)
            {
                if (currentFrameBufferId == frameBufferId)
                {
                    return bindFrameBuffer(0);
                }

                return true;
            }

            static inline bool unbindElementArrayBuffer(GLuint elementArrayBufferId)
            {
                if (currentElementArrayBufferId == elementArrayBufferId)
                {
                    return bindElementArrayBuffer(0);
                }

                return true;
            }

            static inline bool unbindArrayBuffer(GLuint arrayBufferId)
            {
                if (currentArrayBufferId == arrayBufferId)
                {
                    return bindArrayBuffer(0);
                }

                return true;
            }

            static inline bool unbindVertexArray(GLuint vertexArrayId)
            {
                if (currentVertexArrayId == vertexArrayId)
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
                        log(LOG_LEVEL_ERROR, "Failed to set scissor test");
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
                        log(LOG_LEVEL_ERROR, "Failed to set scissor test");
                        return false;
                    }
                }
                
                return true;
            }

            static inline bool enableDepthTest(bool enable)
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
                        log(LOG_LEVEL_ERROR, "Failed to change depth test state");
                        return false;
                    }

                    currentDepthTestEnabled = enable;
                }
                
                return true;
            }

            static inline bool setViewport(GLint x,
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
                        log(LOG_LEVEL_ERROR, "Failed to set viewport");
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

                    currentBlendEnabled = blendEnabled;

                    checkError = true;
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

                        checkError = true;
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
                        
                        checkError = true;
                    }
                }
                
                if (checkError && checkOpenGLError())
                {
                    log(LOG_LEVEL_ERROR, "Failed to set blend state");
                    return false;
                }
                
                return true;
            }

#ifdef OUZEL_SUPPORTS_OPENGL
            static inline bool setPolygonFillMode(GLenum polygonFillMode)
            {
                if (currentPolygonFillMode != polygonFillMode)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, polygonFillMode);

                    currentPolygonFillMode = polygonFillMode;

                    if (checkOpenGLError())
                    {
                        log(LOG_LEVEL_ERROR, "Failed to set blend state");
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

            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              bool newVerticalSync) override;
            virtual bool update();

            virtual void setSize(const Size2& newSize) override;
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
            Rectangle viewport;

            static GLuint currentTextureId[Texture::LAYERS];
            static GLuint currentProgramId;
            static GLuint currentFrameBufferId;

            static GLuint currentElementArrayBufferId;
            static GLuint currentArrayBufferId;
            static GLuint currentVertexArrayId;

            static bool currentBlendEnabled;
            static GLenum currentBlendModeRGB;
            static GLenum currentBlendModeAlpha;
            static GLenum currentBlendSourceFactorRGB;
            static GLenum currentBlendDestFactorRGB;
            static GLenum currentBlendSourceFactorAlpha;
            static GLenum currentBlendDestFactorAlpha;

            static GLenum currentPolygonFillMode;

            static bool currentScissorTestEnabled;
            static GLint currentScissorX;
            static GLint currentScissorY;
            static GLsizei currentScissorWidth;
            static GLsizei currentScissorHeight;

            static bool currentDepthTestEnabled;

            static GLint currentViewportX;
            static GLint currentViewportY;
            static GLsizei currentViewportWidth;
            static GLsizei currentViewportHeight;
            static std::queue<std::pair<GLuint, ResourceType>> deleteQueue;
            static std::mutex deleteMutex;

            std::atomic<bool> dirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
