// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#include <map>
#include <queue>
#include <utility>
#include <mutex>
#include <atomic>

#if OUZEL_SUPPORTS_OPENGLES
    #define GL_GLEXT_PROTOTYPES 1
    #include "GLES/gl.h"
    #include "GLES2/gl2.h"
    #include "GLES2/gl2ext.h"
    #include "GLES3/gl3.h"
#else
    #define GL_GLEXT_PROTOTYPES 1
    #include "GL/glcorearb.h"
    #include "GL/glext.h"
#endif

extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparateProc;
extern PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparateProc;

extern PFNGLUNIFORM1IPROC glUniform1iProc;
extern PFNGLUNIFORM1FVPROC glUniform1fvProc;
extern PFNGLUNIFORM2FVPROC glUniform2fvProc;
extern PFNGLUNIFORM3FVPROC glUniform3fvProc;
extern PFNGLUNIFORM4FVPROC glUniform4fvProc;
extern PFNGLUNIFORM1IVPROC glUniform1ivProc;
extern PFNGLUNIFORM2IVPROC glUniform2ivProc;
extern PFNGLUNIFORM3IVPROC glUniform3ivProc;
extern PFNGLUNIFORM4IVPROC glUniform4ivProc;
extern PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fvProc;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fvProc;

extern PFNGLACTIVETEXTUREPROC glActiveTextureProc;
extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffersProc;
extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffersProc;
extern PFNGLBINDRENDERBUFFERPROC glBindRenderbufferProc;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorageProc;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisampleProc;

extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffersProc;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffersProc;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebufferProc;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatusProc;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbufferProc;
extern PFNGLBLITFRAMEBUFFERPROC glBlitFramebufferProc;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2DProc;

#if OUZEL_SUPPORTS_OPENGLES
extern PFNGLCLEARDEPTHFPROC glClearDepthfProc;
#else
extern PFNGLCLEARDEPTHPROC glClearDepthProc;
#endif

extern PFNGLCREATESHADERPROC glCreateShaderProc;
extern PFNGLDELETESHADERPROC glDeleteShaderProc;
extern PFNGLATTACHSHADERPROC glAttachShaderProc;
extern PFNGLDETACHSHADERPROC glDetachShaderProc;
extern PFNGLSHADERSOURCEPROC glShaderSourceProc;
extern PFNGLCOMPILESHADERPROC glCompileShaderProc;
extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocationProc;
extern PFNGLGETSHADERIVPROC glGetShaderivProc;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLogProc;

extern PFNGLCREATEPROGRAMPROC glCreateProgramProc;
extern PFNGLDELETEPROGRAMPROC glDeleteProgramProc;
extern PFNGLUSEPROGRAMPROC glUseProgramProc;
extern PFNGLLINKPROGRAMPROC glLinkProgramProc;
extern PFNGLGETPROGRAMIVPROC glGetProgramivProc;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLogProc;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocationProc;

extern PFNGLBINDBUFFERPROC glBindBufferProc;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffersProc;
extern PFNGLGENBUFFERSPROC glGenBuffersProc;
extern PFNGLBUFFERDATAPROC glBufferDataProc;
extern PFNGLBUFFERSUBDATAPROC glBufferSubDataProc;

extern PFNGLGENVERTEXARRAYSPROC glGenVertexArraysProc;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArrayProc;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArraysProc;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArrayProc;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArrayProc;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointerProc;

extern PFNGLGETSTRINGIPROC glGetStringiProc;

#if OUZEL_SUPPORTS_OPENGLES
extern PFNGLMAPBUFFEROESPROC glMapBufferProc;
extern PFNGLUNMAPBUFFEROESPROC glUnmapBufferProc;
extern PFNGLMAPBUFFERRANGEEXTPROC glMapBufferRangeProc;
extern PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleProc;
#else
extern PFNGLMAPBUFFERPROC glMapBufferProc;
extern PFNGLUNMAPBUFFERPROC glUnmapBufferProc;
extern PFNGLMAPBUFFERRANGEPROC glMapBufferRangeProc;
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

            virtual std::vector<Size2> getSupportedResolutions() const override;

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
                    glActiveTextureProc(GL_TEXTURE0 + layer);
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
                    glUseProgramProc(programId);
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
                    glBindFramebufferProc(GL_FRAMEBUFFER, frameBufferId);
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
                    glBindBufferProc(bufferType, bufferId);
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
                    if (glBindVertexArrayProc) glBindVertexArrayProc(vertexArrayId);

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
                        glBlendEquationSeparateProc(modeRGB,
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
                        glBlendFuncSeparateProc(sfactorRGB,
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

            static inline bool setColorMask(GLboolean redMask,
                                            GLboolean greenMask,
                                            GLboolean blueMask,
                                            GLboolean alphaMask)
            {
                if (stateCache.redMask != redMask ||
                    stateCache.greenMask != greenMask ||
                    stateCache.blueMask != blueMask ||
                    stateCache.alphaMask != alphaMask)
                {
                    glColorMask(redMask, greenMask, blueMask, alphaMask);

                    stateCache.redMask = redMask;
                    stateCache.greenMask = greenMask;
                    stateCache.blueMask = blueMask;
                    stateCache.alphaMask = alphaMask;

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to set color mask";
                        return false;
                    }
                }

                return true;
            }

            static void deleteBuffer(GLuint bufferId)
            {
                GLuint& elementArrayBufferId = stateCache.bufferId[GL_ELEMENT_ARRAY_BUFFER];
                if (elementArrayBufferId == bufferId) elementArrayBufferId = 0;
                GLuint& arrayBufferId = stateCache.bufferId[GL_ARRAY_BUFFER];
                if (arrayBufferId == bufferId) arrayBufferId = 0;
                glDeleteBuffersProc(1, &bufferId);
            }

            static void deleteVertexArray(GLuint vertexArrayId)
            {
#if OUZEL_PLATFORM_ANDROID
                bindVertexArray(0); // workaround for Android (current VAO's element array buffer is set to 0 if glDeleteVertexArrays is called on Android)
#else
                if (stateCache.vertexArrayId == vertexArrayId) stateCache.vertexArrayId = 0;
#endif
                if (glDeleteVertexArraysProc) glDeleteVertexArraysProc(1, &vertexArrayId);
            }

            static void deleteRenderBuffer(GLuint renderBufferId)
            {
                glDeleteRenderbuffersProc(1, &renderBufferId);
            }

            static void deleteFrameBuffer(GLuint frameBufferId)
            {
                if (stateCache.frameBufferId == frameBufferId) stateCache.frameBufferId = 0;
                glDeleteFramebuffersProc(1, &frameBufferId);
            }

            static void deleteProgram(GLuint programId)
            {
                if (stateCache.programId == programId) stateCache.programId = 0;
                glDeleteProgramProc(programId);
            }

            static void deleteShader(GLuint shaderId)
            {
                glDeleteShaderProc(shaderId);
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

#if !OUZEL_SUPPORTS_OPENGLES
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
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;
            virtual bool upload() override;
            virtual bool draw(const std::vector<DrawCommand>& drawCommands) override;
            virtual bool lockContext();
            virtual bool swapBuffers();
            virtual bool generateScreenshot(const std::string& filename) override;

            virtual BlendStateResource* createBlendState() override;
            virtual TextureResource* createTexture() override;
            virtual ShaderResource* createShader() override;
            virtual MeshBufferResource* createMeshBuffer() override;
            virtual BufferResource* createBuffer() override;

            void* getProcAddress(const std::string& name) const;

            GLuint frameBufferId = 0;
            GLsizei frameBufferWidth = 0;
            GLsizei frameBufferHeight = 0;

            GLbitfield clearMask = 0;
            GLfloat frameBufferClearColor[4];

            struct StateCache
            {
                GLuint textureId[Texture::LAYERS];
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
                GLboolean redMask = GL_TRUE;
                GLboolean greenMask = GL_TRUE;
                GLboolean blueMask = GL_TRUE;
                GLboolean alphaMask = GL_TRUE;

#if !OUZEL_SUPPORTS_OPENGLES
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

#endif
