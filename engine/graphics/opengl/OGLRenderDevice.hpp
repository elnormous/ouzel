// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICE_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include <cstring>
#include <array>
#include <atomic>
#include <memory>
#include <queue>
#include <system_error>
#include <utility>
#include <vector>

#include "OGL.h"

#if OUZEL_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#  include "GLES3/gl31.h"
#  include "GLES3/gl32.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "../RenderDevice.hpp"
#include "OGLShader.hpp"
#include "OGLStateCache.hpp"

namespace ouzel::graphics::opengl
{
    const std::error_category& getErrorCategory() noexcept;
    inline std::error_code makeErrorCode(GLenum e)
    {
        return std::error_code(static_cast<int>(e), getErrorCategory());
    }

    class RenderDevice: public graphics::RenderDevice
    {
        friend Graphics;
    public:
        PFNGLGETINTEGERVPROC glGetIntegervProc = nullptr;
        PFNGLGETSTRINGPROC glGetStringProc = nullptr;
        PFNGLGETERRORPROC glGetErrorProc = nullptr;
        PFNGLENABLEPROC glEnableProc = nullptr;
        PFNGLDISABLEPROC glDisableProc = nullptr;
        PFNGLFRONTFACEPROC glFrontFaceProc = nullptr;
        PFNGLBINDTEXTUREPROC glBindTextureProc = nullptr;
        PFNGLGENTEXTURESPROC glGenTexturesProc = nullptr;
        PFNGLDELETETEXTURESPROC glDeleteTexturesProc = nullptr;
        PFNGLTEXPARAMETERIPROC glTexParameteriProc = nullptr;
        PFNGLTEXPARAMETERIVPROC glTexParameterivProc = nullptr;
        PFNGLTEXPARAMETERFPROC glTexParameterfProc = nullptr;
        PFNGLTEXPARAMETERFVPROC glTexParameterfvProc = nullptr;
        PFNGLTEXIMAGE2DPROC glTexImage2DProc = nullptr;
        PFNGLTEXSUBIMAGE2DPROC glTexSubImage2DProc = nullptr;
        PFNGLVIEWPORTPROC glViewportProc = nullptr;
        PFNGLCLEARPROC glClearProc = nullptr;
        PFNGLCLEARCOLORPROC glClearColorProc = nullptr;
        PFNGLCLEARSTENCILPROC glClearStencilProc = nullptr;
        PFNGLCOLORMASKPROC glColorMaskProc = nullptr;
        PFNGLDEPTHMASKPROC glDepthMaskProc = nullptr;
        PFNGLDEPTHFUNCPROC glDepthFuncProc = nullptr;
        PFNGLSTENCILMASKPROC glStencilMaskProc = nullptr;
        PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparateProc = nullptr;
        PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparateProc = nullptr;
        PFNGLCULLFACEPROC glCullFaceProc = nullptr;
        PFNGLSCISSORPROC glScissorProc = nullptr;
        PFNGLDRAWELEMENTSPROC glDrawElementsProc = nullptr;
        PFNGLREADPIXELSPROC glReadPixelsProc = nullptr;

        PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparateProc = nullptr;
        PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparateProc = nullptr;

        PFNGLACTIVETEXTUREPROC glActiveTextureProc = nullptr;

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

        PFNGLMAPBUFFERRANGEPROC glMapBufferRangeProc = nullptr;
        PFNGLUNMAPBUFFERPROC glUnmapBufferProc = nullptr;

        PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubDataProc = nullptr;

#if OUZEL_OPENGLES
        PFNGLCLEARDEPTHFPROC glClearDepthfProc = nullptr;
        PFNGLMAPBUFFEROESPROC glMapBufferProc = nullptr;
        PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleProc = nullptr;
#  if OUZEL_OPENGL_INTERFACE_EAGL
        PFNGLDISCARDFRAMEBUFFEREXTPROC glDiscardFramebufferEXTProc = nullptr;
        PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC glResolveMultisampleFramebufferAPPLEProc = nullptr;
#  endif
#else
        PFNGLPOLYGONMODEPROC glPolygonModeProc = nullptr;
        PFNGLCLEARDEPTHPROC glClearDepthProc = nullptr;
        PFNGLMAPBUFFERPROC glMapBufferProc = nullptr;
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

        RenderDevice(const Settings& settings,
                     core::Window& newWindow,
                     const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        auto isTextureBaseLevelSupported() const noexcept { return textureBaseLevelSupported; }
        auto isTextureMaxLevelSupported() const noexcept { return textureMaxLevelSupported; }

        void setFrontFace(GLenum mode)
        {
            if (stateCache.frontFace != mode)
            {
                glFrontFaceProc(mode);
                stateCache.frontFace = mode;

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set front face mode");
            }
        }

        void bindTexture(GLenum target, GLenum layer, GLuint textureId)
        {
            if (stateCache.textures[target][layer] != textureId)
            {
                glActiveTextureProc(GL_TEXTURE0 + layer);
                glBindTextureProc(target, textureId);
                stateCache.textures[target][layer] = textureId;

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to bind texture");
            }
        }

        void useProgram(GLuint programId)
        {
            if (stateCache.programId != programId)
            {
                glUseProgramProc(programId);
                stateCache.programId = programId;

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to bind program");
            }
        }

        void bindFrameBuffer(GLuint bufferId)
        {
            if (stateCache.frameBufferId != bufferId)
            {
                glBindFramebufferProc(GL_FRAMEBUFFER, bufferId);
                stateCache.frameBufferId = bufferId;

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to bind frame buffer");
            }
        }

        void bindBuffer(GLuint target, GLuint bufferId)
        {
            GLuint& currentBufferId = stateCache.bufferId[target];

            if (currentBufferId != bufferId)
            {
                glBindBufferProc(target, bufferId);
                currentBufferId = bufferId;

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to bind element array buffer");
            }
        }

        void setScissorTest(bool scissorTestEnabled,
                            GLint x, GLint y,
                            GLsizei width, GLsizei height)
        {
            if (stateCache.scissorTestEnabled != scissorTestEnabled)
            {
                if (scissorTestEnabled)
                    glEnableProc(GL_SCISSOR_TEST);
                else
                    glDisableProc(GL_SCISSOR_TEST);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
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

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set scissor test");
            }
        }

        void enableDepthTest(bool enable)
        {
            if (stateCache.depthTestEnabled != enable)
            {
                if (enable)
                    glEnableProc(GL_DEPTH_TEST);
                else
                    glDisableProc(GL_DEPTH_TEST);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to change depth test state");

                stateCache.depthTestEnabled = enable;
            }
        }

        void enableStencilTest(bool enable)
        {
            if (stateCache.stencilTestEnabled != enable)
            {
                if (enable)
                    glEnableProc(GL_STENCIL_TEST);
                else
                    glDisableProc(GL_STENCIL_TEST);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to change stencil test state");

                stateCache.stencilTestEnabled = enable;
            }
        }

        void setViewport(GLint x, GLint y,
                         GLsizei width, GLsizei height)
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

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set viewport");
            }
        }

        void setBlendState(bool blendEnabled,
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

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
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

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set blend state");
            }
        }

        void setColorMask(GLboolean redMask,
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

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set color mask");
            }
        }

        void setDepthMask(GLboolean flag)
        {
            if (stateCache.depthMask != flag)
            {
                glDepthMaskProc(flag);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to change depth mask state");

                stateCache.depthMask = flag;
            }
        }

        void setDepthFunc(GLenum depthFunc)
        {
            if (stateCache.depthFunc != depthFunc)
            {
                glDepthFuncProc(depthFunc);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to change depth test state");

                stateCache.depthFunc = depthFunc;
            }
        }

        void setStencilMask(GLuint stencilMask)
        {
            if (stateCache.stencilMask != stencilMask)
            {
                glStencilMaskProc(stencilMask);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to change stencil mask");

                stateCache.stencilMask = stencilMask;
            }
        }

        void setCullFace(bool cullEnabled, GLenum cullFace)
        {
            if (stateCache.cullEnabled != cullEnabled)
            {
                if (cullEnabled)
                    glEnableProc(GL_CULL_FACE);
                else
                    glDisableProc(GL_CULL_FACE);

                stateCache.cullEnabled = cullEnabled;

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to enable cull face");
            }

            if (cullEnabled)
            {
                if (stateCache.cullFace != cullFace)
                {
                    glCullFaceProc(cullFace);
                    stateCache.cullFace = cullFace;
                }

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set cull face");
            }
        }

        void setClearColorValue(const std::array<GLfloat, 4>& clearColorValue)
        {
            if (stateCache.clearColor != clearColorValue)
            {
                glClearColorProc(clearColorValue[0],
                                 clearColorValue[1],
                                 clearColorValue[2],
                                 clearColorValue[3]);

                stateCache.clearColor = clearColorValue;

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set clear color");
            }
        }

        void setClearDepthValue(GLfloat clearDepthValue)
        {
            if (stateCache.clearDepth != clearDepthValue)
            {
#if OUZEL_OPENGLES
                glClearDepthfProc(clearDepthValue);
#else
                glClearDepthProc(static_cast<GLdouble>(clearDepthValue));
#endif

                stateCache.clearDepth = clearDepthValue;

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set clear depth");
            }
        }

        void setClearStencilValue(GLint clearStencilValue)
        {
            if (stateCache.clearStencil != clearStencilValue)
            {
                glClearStencilProc(clearStencilValue);

                stateCache.clearStencil = clearStencilValue;

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set clear stencil");
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
            for (auto& texture : stateCache.textures)
                for (auto& boundTextureId : texture.second)
                    if (boundTextureId == textureId) boundTextureId = 0;

            glDeleteTexturesProc(1, &textureId);
        }

#if !OUZEL_OPENGLES
        void setPolygonFillMode(GLenum polygonFillMode)
        {
            if (stateCache.polygonFillMode != polygonFillMode)
            {
                glPolygonModeProc(GL_FRONT_AND_BACK, polygonFillMode);

                stateCache.polygonFillMode = polygonFillMode;

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set blend state");
            }
        }
#endif

        template <class T>
        auto getResource(std::size_t id) const
        {
            return id ? static_cast<T*>(resources[id - 1].get()) : nullptr;
        }

    protected:
        void init(GLsizei newWidth, GLsizei newHeight);
        virtual void resizeFrameBuffer() {}

        void process() override;
        virtual void present();
        void generateScreenshot(const std::string& filename) override;
        void setUniform(GLint location, DataType dataType, const void* data);

        bool embedded = false;

        GLuint frameBufferId = 0;
        GLsizei frameBufferWidth = 0;
        GLsizei frameBufferHeight = 0;
        GLuint vertexArrayId = 0;

        bool textureBaseLevelSupported:1;
        bool textureMaxLevelSupported:1;

        StateCache stateCache;

        std::vector<std::unique_ptr<RenderResource>> resources;
    };
}

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICE_HPP
