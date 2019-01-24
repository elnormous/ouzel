// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICE_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include <cstring>
#include <array>
#include <atomic>
#include <map>
#include <memory>
#include <queue>
#include <system_error>
#include <utility>
#include <vector>

#if OUZEL_SUPPORTS_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#  include "GLES3/gl31.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "graphics/RenderDevice.hpp"
#include "graphics/opengl/OGLShader.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OpenGLErrorCategory: public std::error_category
        {
        public:
            const char* name() const noexcept override
            {
                return "OpenGL";
            }

            std::string message(int condition) const override
            {
                switch (condition)
                {
                    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
                    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
                    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
                    case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
                    case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
                    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
                    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
#if !OUZEL_SUPPORTS_OPENGLES
                    case GL_CONTEXT_LOST: return "GL_CONTEXT_LOST";
#endif
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        extern const OpenGLErrorCategory openGLErrorCategory;
        std::error_code makeErrorCode(GLenum e);

        class OGLRenderDevice: public RenderDevice
        {
            friend Renderer;
        public:
            PFNGLGETINTEGERVPROC glGetIntegervProc = nullptr;
            PFNGLGETSTRINGPROC glGetStringProc = nullptr;
            PFNGLGETERRORPROC glGetErrorProc = nullptr;
            PFNGLENABLEPROC glEnableProc = nullptr;
            PFNGLDISABLEPROC glDisableProc = nullptr;
            PFNGLBINDTEXTUREPROC glBindTextureProc = nullptr;
            PFNGLGENTEXTURESPROC glGenTexturesProc = nullptr;
            PFNGLDELETETEXTURESPROC glDeleteTexturesProc = nullptr;
            PFNGLTEXPARAMETERIPROC glTexParameteriProc = nullptr;
            PFNGLTEXIMAGE2DPROC glTexImage2DProc = nullptr;
            PFNGLTEXSUBIMAGE2DPROC glTexSubImage2DProc = nullptr;
            PFNGLVIEWPORTPROC glViewportProc = nullptr;
            PFNGLCLEARPROC glClearProc = nullptr;
            PFNGLCLEARCOLORPROC glClearColorProc = nullptr;
            PFNGLCOLORMASKPROC glColorMaskProc = nullptr;
            PFNGLDEPTHMASKPROC glDepthMaskProc = nullptr;
            PFNGLDEPTHFUNCPROC glDepthFuncProc = nullptr;
            PFNGLCULLFACEPROC glCullFaceProc = nullptr;
            PFNGLSCISSORPROC glScissorProc = nullptr;
            PFNGLDRAWELEMENTSPROC glDrawElementsProc = nullptr;
            PFNGLREADPIXELSPROC glReadPixelsProc = nullptr;

            PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparateProc = nullptr;
            PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparateProc = nullptr;

            PFNGLUNIFORM1IPROC glUniform1iProc = nullptr;
            PFNGLUNIFORM1FVPROC glUniform1fvProc = nullptr;
            PFNGLUNIFORM2FVPROC glUniform2fvProc = nullptr;
            PFNGLUNIFORM3FVPROC glUniform3fvProc = nullptr;
            PFNGLUNIFORM4FVPROC glUniform4fvProc = nullptr;
            PFNGLUNIFORM1IVPROC glUniform1ivProc = nullptr;
            PFNGLUNIFORM2IVPROC glUniform2ivProc = nullptr;
            PFNGLUNIFORM3IVPROC glUniform3ivProc = nullptr;
            PFNGLUNIFORM4IVPROC glUniform4ivProc = nullptr;
            PFNGLUNIFORM1UIVPROC glUniform1uivProc = nullptr;
            PFNGLUNIFORM2UIVPROC glUniform2uivProc = nullptr;
            PFNGLUNIFORM3UIVPROC glUniform3uivProc = nullptr;
            PFNGLUNIFORM4UIVPROC glUniform4uivProc = nullptr;
            PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fvProc = nullptr;
            PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fvProc = nullptr;

            PFNGLACTIVETEXTUREPROC glActiveTextureProc = nullptr;
            PFNGLGENRENDERBUFFERSPROC glGenRenderbuffersProc = nullptr;
            PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffersProc = nullptr;
            PFNGLBINDRENDERBUFFERPROC glBindRenderbufferProc = nullptr;
            PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorageProc = nullptr;
            PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisampleProc = nullptr;
            PFNGLTEXSTORAGE2DMULTISAMPLEPROC glTexStorage2DMultisampleProc = nullptr;

            PFNGLGENFRAMEBUFFERSPROC glGenFramebuffersProc = nullptr;
            PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffersProc = nullptr;
            PFNGLBINDFRAMEBUFFERPROC glBindFramebufferProc = nullptr;
            PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatusProc = nullptr;
            PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbufferProc = nullptr;
            PFNGLBLITFRAMEBUFFERPROC glBlitFramebufferProc = nullptr;
            PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2DProc = nullptr;

#if OUZEL_SUPPORTS_OPENGLES
            PFNGLCLEARDEPTHFPROC glClearDepthfProc = nullptr;
            PFNGLMAPBUFFEROESPROC glMapBufferProc = nullptr;
            PFNGLUNMAPBUFFEROESPROC glUnmapBufferProc = nullptr;
            PFNGLMAPBUFFERRANGEEXTPROC glMapBufferRangeProc = nullptr;
            PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleProc = nullptr;
            PFNGLCOPYIMAGESUBDATAEXTPROC glCopyImageSubDataProc = nullptr;
#  if OUZEL_OPENGL_INTERFACE_EAGL
            PFNGLDISCARDFRAMEBUFFEREXTPROC glDiscardFramebufferEXTProc = nullptr;
            PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC glRenderbufferStorageMultisampleAPPLEProc = nullptr;
            PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC glResolveMultisampleFramebufferAPPLEProc = nullptr;
#  endif
#else
            PFNGLPOLYGONMODEPROC glPolygonModeProc = nullptr;
            PFNGLCLEARDEPTHPROC glClearDepthProc = nullptr;
            PFNGLMAPBUFFERPROC glMapBufferProc = nullptr;
            PFNGLUNMAPBUFFERPROC glUnmapBufferProc = nullptr;
            PFNGLMAPBUFFERRANGEPROC glMapBufferRangeProc = nullptr;
            PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubDataProc = nullptr;
#endif

            PFNGLCREATESHADERPROC glCreateShaderProc = nullptr;
            PFNGLDELETESHADERPROC glDeleteShaderProc = nullptr;
            PFNGLATTACHSHADERPROC glAttachShaderProc = nullptr;
            PFNGLDETACHSHADERPROC glDetachShaderProc = nullptr;
            PFNGLSHADERSOURCEPROC glShaderSourceProc = nullptr;
            PFNGLCOMPILESHADERPROC glCompileShaderProc = nullptr;
            PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocationProc = nullptr;
            PFNGLGETSHADERIVPROC glGetShaderivProc = nullptr;
            PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLogProc = nullptr;

            PFNGLCREATEPROGRAMPROC glCreateProgramProc = nullptr;
            PFNGLDELETEPROGRAMPROC glDeleteProgramProc = nullptr;
            PFNGLUSEPROGRAMPROC glUseProgramProc = nullptr;
            PFNGLLINKPROGRAMPROC glLinkProgramProc = nullptr;
            PFNGLGETPROGRAMIVPROC glGetProgramivProc = nullptr;
            PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLogProc = nullptr;
            PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocationProc = nullptr;

            PFNGLBINDBUFFERPROC glBindBufferProc = nullptr;
            PFNGLDELETEBUFFERSPROC glDeleteBuffersProc = nullptr;
            PFNGLGENBUFFERSPROC glGenBuffersProc = nullptr;
            PFNGLBUFFERDATAPROC glBufferDataProc = nullptr;
            PFNGLBUFFERSUBDATAPROC glBufferSubDataProc = nullptr;

            PFNGLGENVERTEXARRAYSPROC glGenVertexArraysProc = nullptr;
            PFNGLBINDVERTEXARRAYPROC glBindVertexArrayProc = nullptr;
            PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArraysProc = nullptr;
            PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArrayProc = nullptr;
            PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArrayProc = nullptr;
            PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointerProc = nullptr;

            PFNGLGETSTRINGIPROC glGetStringiProc = nullptr;
            PFNGLPUSHGROUPMARKEREXTPROC glPushGroupMarkerEXTProc = nullptr;
            PFNGLPOPGROUPMARKEREXTPROC glPopGroupMarkerEXTProc = nullptr;

            virtual ~OGLRenderDevice();

            bool isTextureBaseLevelSupported() const { return textureBaseLevelSupported; }
            bool isTextureMaxLevelSupported() const { return textureMaxLevelSupported; }

            inline void bindTexture(GLuint textureId, uint32_t layer)
            {
                if (stateCache.textureId[layer] != textureId)
                {
                    glActiveTextureProc(GL_TEXTURE0 + layer);
                    glBindTextureProc(GL_TEXTURE_2D, textureId);
                    stateCache.textureId[layer] = textureId;

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to bind texture");
                }
            }

            inline void useProgram(GLuint programId)
            {
                if (stateCache.programId != programId)
                {
                    glUseProgramProc(programId);
                    stateCache.programId = programId;

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to bind program");
                }
            }

            inline void bindFrameBuffer(GLuint bufferId)
            {
                if (stateCache.frameBufferId != bufferId)
                {
                    glBindFramebufferProc(GL_FRAMEBUFFER, bufferId);
                    stateCache.frameBufferId = bufferId;

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to bind frame buffer");
                }
            }

            inline void bindBuffer(GLuint bufferType, GLuint bufferId)
            {
                GLuint& currentBufferId = stateCache.bufferId[bufferType];

                if (currentBufferId != bufferId)
                {
                    glBindBufferProc(bufferType, bufferId);
                    currentBufferId = bufferId;

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to bind element array buffer");
                }
            }

            inline void setScissorTest(bool scissorTestEnabled,
                                       GLint x,
                                       GLint y,
                                       GLsizei width,
                                       GLsizei height)
            {
                if (stateCache.scissorTestEnabled != scissorTestEnabled)
                {
                    if (scissorTestEnabled)
                        glEnableProc(GL_SCISSOR_TEST);
                    else
                        glDisableProc(GL_SCISSOR_TEST);

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set scissor test");

                    stateCache.scissorTestEnabled = scissorTestEnabled;
                }

                if (scissorTestEnabled)
                {
                    if (stateCache.scissorX != x ||
                        stateCache.scissorY != y ||
                        stateCache.scissorWidth != width ||
                        stateCache.scissorHeight != height)
                    {
                        glScissorProc(x, y, width, height);
                        stateCache.scissorX = x;
                        stateCache.scissorY = y;
                        stateCache.scissorWidth = width;
                        stateCache.scissorHeight = height;
                    }

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set scissor test");
                }
            }

            inline void enableDepthTest(bool enable)
            {
                if (stateCache.depthTestEnabled != enable)
                {
                    if (enable)
                        glEnableProc(GL_DEPTH_TEST);
                    else
                        glDisableProc(GL_DEPTH_TEST);

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to change depth test state");

                    stateCache.depthTestEnabled = enable;
                }
            }

            inline void setDepthMask(GLboolean flag)
            {
                if (stateCache.depthMask != flag)
                {
                    glDepthMaskProc(flag);

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to change depth mask state");

                    stateCache.depthMask = flag;
                }
            }

            inline void setDepthFunc(GLenum depthFunc)
            {
                if (stateCache.depthFunc != depthFunc)
                {
                    glDepthFuncProc(depthFunc);

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to change depth test state");

                    stateCache.depthFunc = depthFunc;
                }
            }

            inline void setViewport(GLint x,
                                    GLint y,
                                    GLsizei width,
                                    GLsizei height)
            {
                if (stateCache.viewportX != x ||
                    stateCache.viewportY != y ||
                    stateCache.viewportWidth != width ||
                    stateCache.viewportHeight != height)
                {
                    glViewportProc(x, y, width, height);
                    stateCache.viewportX = x;
                    stateCache.viewportY = y;
                    stateCache.viewportWidth = width;
                    stateCache.viewportHeight = height;

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set viewport");
                }
            }

            inline void setBlendState(bool blendEnabled,
                                      GLenum modeRGB,
                                      GLenum modeAlpha,
                                      GLenum sfactorRGB,
                                      GLenum dfactorRGB,
                                      GLenum sfactorAlpha,
                                      GLenum dfactorAlpha)
            {
                if (stateCache.blendEnabled != blendEnabled)
                {
                    if (blendEnabled)
                        glEnableProc(GL_BLEND);
                    else
                        glDisableProc(GL_BLEND);

                    stateCache.blendEnabled = blendEnabled;

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to enable blend state");
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
                    }

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set blend state");
                }
            }

            inline void setColorMask(GLboolean redMask,
                                     GLboolean greenMask,
                                     GLboolean blueMask,
                                     GLboolean alphaMask)
            {
                if (stateCache.redMask != redMask ||
                    stateCache.greenMask != greenMask ||
                    stateCache.blueMask != blueMask ||
                    stateCache.alphaMask != alphaMask)
                {
                    glColorMaskProc(redMask, greenMask, blueMask, alphaMask);

                    stateCache.redMask = redMask;
                    stateCache.greenMask = greenMask;
                    stateCache.blueMask = blueMask;
                    stateCache.alphaMask = alphaMask;

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set color mask");
                }
            }

            inline void setCullFace(bool cullEnabled,
                                    GLenum cullFace)
            {
                if (stateCache.cullEnabled != cullEnabled)
                {
                    if (cullEnabled)
                        glEnableProc(GL_CULL_FACE);
                    else
                        glDisableProc(GL_CULL_FACE);

                    stateCache.cullEnabled = cullEnabled;

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to enable cull face");
                }

                if (cullEnabled)
                {
                    if (stateCache.cullFace != cullFace)
                    {
                        glCullFaceProc(cullFace);
                        stateCache.cullFace = cullFace;
                    }

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set cull face");
                }
            }

            inline void setClearDepthValue(float clearDepthValue)
            {
                if (stateCache.clearDepth != clearDepthValue)
                {
#if OUZEL_SUPPORTS_OPENGLES
                    glClearDepthfProc(clearDepthValue);
#else
                    glClearDepthProc(clearDepthValue);
#endif

                    stateCache.clearDepth = clearDepthValue;

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to enable cull face");
                }
            }

            inline void setClearColorValue(const std::array<GLfloat, 4>& clearColorValue)
            {
                if (stateCache.clearColor != clearColorValue)
                {
                    glClearColorProc(clearColorValue[0],
                                     clearColorValue[1],
                                     clearColorValue[2],
                                     clearColorValue[3]);

                    stateCache.clearColor = clearColorValue;

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to enable cull face");
                }
            }

            void deleteBuffer(GLuint bufferId)
            {
                GLuint& elementArrayBufferId = stateCache.bufferId[GL_ELEMENT_ARRAY_BUFFER];
                if (elementArrayBufferId == bufferId) elementArrayBufferId = 0;
                GLuint& arrayBufferId = stateCache.bufferId[GL_ARRAY_BUFFER];
                if (arrayBufferId == bufferId) arrayBufferId = 0;
                glDeleteBuffersProc(1, &bufferId);
            }

            void deleteRenderBuffer(GLuint renderBufferId)
            {
                glDeleteRenderbuffersProc(1, &renderBufferId);
            }

            void deleteFrameBuffer(GLuint bufferId)
            {
                if (stateCache.frameBufferId == bufferId) stateCache.frameBufferId = 0;
                glDeleteFramebuffersProc(1, &bufferId);
            }

            void deleteProgram(GLuint programId)
            {
                if (stateCache.programId == programId) stateCache.programId = 0;
                glDeleteProgramProc(programId);
            }

            void deleteTexture(GLuint textureId)
            {
                for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                {
                    if (stateCache.textureId[layer] == textureId)
                        stateCache.textureId[layer] = 0;
                }
                glDeleteTexturesProc(1, &textureId);
            }

#if !OUZEL_SUPPORTS_OPENGLES
            inline void setPolygonFillMode(GLenum polygonFillMode)
            {
                if (stateCache.polygonFillMode != polygonFillMode)
                {
                    glPolygonModeProc(GL_FRONT_AND_BACK, polygonFillMode);

                    stateCache.polygonFillMode = polygonFillMode;

                    GLenum error;

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set blend state");
                }
            }
#endif

        protected:
            OGLRenderDevice(const std::function<void(const Event&)>& initCallback);

            void init(Window* newWindow,
                      const Size2<uint32_t>& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newDebugRenderer) override;

            void setSize(const Size2<uint32_t>& newSize) override;

            void process() override;
            virtual void present();
            void generateScreenshot(const std::string& filename) override;
            void setUniform(GLint location, DataType dataType, const void* data);
            
            GLuint frameBufferId = 0;
            GLsizei frameBufferWidth = 0;
            GLsizei frameBufferHeight = 0;
            GLuint vertexArrayId = 0;

            bool textureBaseLevelSupported = false;
            bool textureMaxLevelSupported = false;

            class StateCache
            {
            public:
                StateCache()
                {
                    bufferId[GL_ELEMENT_ARRAY_BUFFER] = 0;
                    bufferId[GL_ARRAY_BUFFER] = 0;
                    std::fill(std::begin(textureId), std::end(textureId), 0);
                    std::fill(std::begin(clearColor), std::end(clearColor), 0.0F);
                }

                GLuint textureId[Texture::LAYERS];
                GLuint programId = 0;
                GLuint frameBufferId = 0;

                std::map<GLuint, GLuint> bufferId;

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

                bool depthTestEnabled = false;
                GLboolean depthMask = GL_TRUE;
                GLenum depthFunc = GL_LESS;

                GLint viewportX = 0;
                GLint viewportY = 0;
                GLsizei viewportWidth = 0;
                GLsizei viewportHeight = 0;
                bool cullEnabled = false;
                GLenum cullFace = GL_NONE;

                std::array<GLfloat, 4> clearColor{{0.0F, 0.0F, 0.0F, 0.0F}};
                float clearDepth = 1.0F;
            };

            StateCache stateCache;

            std::vector<std::unique_ptr<OGLRenderResource>> resources;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICE_HPP
