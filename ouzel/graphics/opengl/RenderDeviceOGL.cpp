// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if !OUZEL_PLATFORM_WINDOWS
#  include <dlfcn.h>
#endif

#include <cassert>
#include <sstream>

#if OUZEL_SUPPORTS_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#if OUZEL_OPENGL_INTERFACE_EGL
#  include "EGL/egl.h"
#elif OUZEL_OPENGL_INTERFACE_GLX
#  include <GL/glx.h>
#  include "GL/glxext.h"
#elif OUZEL_OPENGL_INTERFACE_WGL
#  include "GL/wglext.h"
#endif

#include "RenderDeviceOGL.hpp"
#include "BlendStateResourceOGL.hpp"
#include "BufferResourceOGL.hpp"
#include "DepthStencilStateResourceOGL.hpp"
#include "RenderTargetResourceOGL.hpp"
#include "ShaderResourceOGL.hpp"
#include "TextureResourceOGL.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"
#include "stb_image_write.h"

PFNGLGETINTEGERVPROC glGetIntegervProc;
PFNGLGETSTRINGPROC glGetStringProc;
PFNGLGETERRORPROC glGetErrorProc;
PFNGLENABLEPROC glEnableProc;
PFNGLDISABLEPROC glDisableProc;
PFNGLBINDTEXTUREPROC glBindTextureProc;
PFNGLGENTEXTURESPROC glGenTexturesProc;
PFNGLDELETETEXTURESPROC glDeleteTexturesProc;
PFNGLTEXPARAMETERIPROC glTexParameteriProc;
PFNGLTEXIMAGE2DPROC glTexImage2DProc;
PFNGLTEXSUBIMAGE2DPROC glTexSubImage2DProc;
PFNGLVIEWPORTPROC glViewportProc;
PFNGLCLEARPROC glClearProc;
PFNGLCLEARCOLORPROC glClearColorProc;
PFNGLCOLORMASKPROC glColorMaskProc;
PFNGLDEPTHMASKPROC glDepthMaskProc;
PFNGLDEPTHFUNCPROC glDepthFuncProc;
PFNGLCULLFACEPROC glCullFaceProc;
PFNGLSCISSORPROC glScissorProc;
PFNGLDRAWELEMENTSPROC glDrawElementsProc;
PFNGLREADPIXELSPROC glReadPixelsProc;

PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparateProc;
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparateProc;

PFNGLUNIFORM1IPROC glUniform1iProc;
PFNGLUNIFORM1FVPROC glUniform1fvProc;
PFNGLUNIFORM2FVPROC glUniform2fvProc;
PFNGLUNIFORM3FVPROC glUniform3fvProc;
PFNGLUNIFORM4FVPROC glUniform4fvProc;
PFNGLUNIFORM1IVPROC glUniform1ivProc;
PFNGLUNIFORM2IVPROC glUniform2ivProc;
PFNGLUNIFORM3IVPROC glUniform3ivProc;
PFNGLUNIFORM4IVPROC glUniform4ivProc;
PFNGLUNIFORM1UIVPROC glUniform1uivProc;
PFNGLUNIFORM2UIVPROC glUniform2uivProc;
PFNGLUNIFORM3UIVPROC glUniform3uivProc;
PFNGLUNIFORM4UIVPROC glUniform4uivProc;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fvProc;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fvProc;

PFNGLACTIVETEXTUREPROC glActiveTextureProc;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffersProc;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffersProc;
PFNGLBINDRENDERBUFFERPROC glBindRenderbufferProc;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorageProc;

PFNGLGENFRAMEBUFFERSPROC glGenFramebuffersProc;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffersProc;
PFNGLBINDFRAMEBUFFERPROC glBindFramebufferProc;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatusProc;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbufferProc;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebufferProc;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2DProc;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisampleProc;

#if OUZEL_SUPPORTS_OPENGLES
PFNGLCLEARDEPTHFPROC glClearDepthfProc;
PFNGLMAPBUFFEROESPROC glMapBufferProc;
PFNGLUNMAPBUFFEROESPROC glUnmapBufferProc;
PFNGLMAPBUFFERRANGEEXTPROC glMapBufferRangeProc;
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleProc;
PFNGLCOPYIMAGESUBDATAEXTPROC glCopyImageSubDataProc;
#  if OUZEL_OPENGL_INTERFACE_EAGL
PFNGLDISCARDFRAMEBUFFEREXTPROC glDiscardFramebufferEXTProc;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC glRenderbufferStorageMultisampleAPPLEProc;
PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC glResolveMultisampleFramebufferAPPLEProc;
#  endif
#else
PFNGLPOLYGONMODEPROC glPolygonModeProc;
PFNGLCLEARDEPTHPROC glClearDepthProc;
PFNGLMAPBUFFERPROC glMapBufferProc;
PFNGLUNMAPBUFFERPROC glUnmapBufferProc;
PFNGLMAPBUFFERRANGEPROC glMapBufferRangeProc;
PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubDataProc;
#endif

PFNGLCREATESHADERPROC glCreateShaderProc;
PFNGLDELETESHADERPROC glDeleteShaderProc;
PFNGLATTACHSHADERPROC glAttachShaderProc;
PFNGLDETACHSHADERPROC glDetachShaderProc;
PFNGLSHADERSOURCEPROC glShaderSourceProc;
PFNGLCOMPILESHADERPROC glCompileShaderProc;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocationProc;
PFNGLGETSHADERIVPROC glGetShaderivProc;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLogProc;

PFNGLCREATEPROGRAMPROC glCreateProgramProc;
PFNGLDELETEPROGRAMPROC glDeleteProgramProc;
PFNGLUSEPROGRAMPROC glUseProgramProc;
PFNGLLINKPROGRAMPROC glLinkProgramProc;
PFNGLGETPROGRAMIVPROC glGetProgramivProc;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLogProc;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocationProc;

PFNGLBINDBUFFERPROC glBindBufferProc;
PFNGLDELETEBUFFERSPROC glDeleteBuffersProc;
PFNGLGENBUFFERSPROC glGenBuffersProc;
PFNGLBUFFERDATAPROC glBufferDataProc;
PFNGLBUFFERSUBDATAPROC glBufferSubDataProc;

PFNGLGENVERTEXARRAYSPROC glGenVertexArraysProc;
PFNGLBINDVERTEXARRAYPROC glBindVertexArrayProc;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArraysProc;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArrayProc;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArrayProc;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointerProc;

PFNGLGETSTRINGIPROC glGetStringiProc;
PFNGLPUSHGROUPMARKEREXTPROC glPushGroupMarkerEXTProc;
PFNGLPOPGROUPMARKEREXTPROC glPopGroupMarkerEXTProc;

template<typename T>
static inline T getCoreProcAddress(const char* name)
{
#if OUZEL_OPENGL_INTERFACE_GLX
    return reinterpret_cast<T>(glXGetProcAddress(reinterpret_cast<const GLubyte*>(name)));
#elif OUZEL_OPENGL_INTERFACE_WGL
    return reinterpret_cast<T>(wglGetProcAddress(name));
#else
    T result;
    *reinterpret_cast<void**>(&result) = dlsym(RTLD_DEFAULT, name);
    return result;
#endif
}

template<typename T>
static inline T getExtProcAddress(const char* name)
{
#if OUZEL_OPENGL_INTERFACE_EGL
    return reinterpret_cast<T>(eglGetProcAddress(name));
#elif OUZEL_OPENGL_INTERFACE_GLX
    return reinterpret_cast<T>(glXGetProcAddress(reinterpret_cast<const GLubyte*>(name)));
#elif OUZEL_OPENGL_INTERFACE_WGL
    return reinterpret_cast<T>(wglGetProcAddress(name));
#else
    T result;
    *reinterpret_cast<void**>(&result) = dlsym(RTLD_DEFAULT, name);
    return result;
#endif
}

namespace ouzel
{
    namespace graphics
    {
        static GLenum getVertexFormat(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::BYTE:
                case DataType::BYTE_NORM:
                case DataType::BYTE_VECTOR2:
                case DataType::BYTE_VECTOR2_NORM:
                case DataType::BYTE_VECTOR3:
                case DataType::BYTE_VECTOR3_NORM:
                case DataType::BYTE_VECTOR4:
                case DataType::BYTE_VECTOR4_NORM:
                    return GL_BYTE;

                case DataType::UNSIGNED_BYTE:
                case DataType::UNSIGNED_BYTE_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR2:
                case DataType::UNSIGNED_BYTE_VECTOR2_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR3:
                case DataType::UNSIGNED_BYTE_VECTOR3_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR4:
                case DataType::UNSIGNED_BYTE_VECTOR4_NORM:
                    return GL_UNSIGNED_BYTE;

                case DataType::SHORT:
                case DataType::SHORT_NORM:
                case DataType::SHORT_VECTOR2:
                case DataType::SHORT_VECTOR2_NORM:
                case DataType::SHORT_VECTOR3:
                case DataType::SHORT_VECTOR3_NORM:
                case DataType::SHORT_VECTOR4:
                case DataType::SHORT_VECTOR4_NORM:
                    return GL_SHORT;

                case DataType::UNSIGNED_SHORT:
                case DataType::UNSIGNED_SHORT_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR2:
                case DataType::UNSIGNED_SHORT_VECTOR2_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR3:
                case DataType::UNSIGNED_SHORT_VECTOR3_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR4:
                case DataType::UNSIGNED_SHORT_VECTOR4_NORM:
                    return GL_UNSIGNED_SHORT;

                case DataType::INTEGER:
                case DataType::INTEGER_VECTOR2:
                case DataType::INTEGER_VECTOR3:
                case DataType::INTEGER_VECTOR4:
                    return GL_INT;

                case DataType::UNSIGNED_INTEGER:
                case DataType::UNSIGNED_INTEGER_VECTOR2:
                case DataType::UNSIGNED_INTEGER_VECTOR3:
                case DataType::UNSIGNED_INTEGER_VECTOR4:
                    return GL_UNSIGNED_INT;

                case DataType::FLOAT:
                case DataType::FLOAT_VECTOR2:
                case DataType::FLOAT_VECTOR3:
                case DataType::FLOAT_VECTOR4:
                case DataType::FLOAT_MATRIX3:
                case DataType::FLOAT_MATRIX4:
                    return GL_FLOAT;

                default:
                    return 0;
            }
        }

        static GLint getArraySize(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::BYTE:
                case DataType::BYTE_NORM:
                case DataType::UNSIGNED_BYTE:
                case DataType::UNSIGNED_BYTE_NORM:
                case DataType::SHORT:
                case DataType::SHORT_NORM:
                case DataType::UNSIGNED_SHORT:
                case DataType::UNSIGNED_SHORT_NORM:
                case DataType::INTEGER:
                case DataType::UNSIGNED_INTEGER:
                case DataType::FLOAT:
                    return 1;

                case DataType::BYTE_VECTOR2:
                case DataType::BYTE_VECTOR2_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR2:
                case DataType::UNSIGNED_BYTE_VECTOR2_NORM:
                case DataType::SHORT_VECTOR2:
                case DataType::SHORT_VECTOR2_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR2:
                case DataType::UNSIGNED_SHORT_VECTOR2_NORM:
                case DataType::INTEGER_VECTOR2:
                case DataType::UNSIGNED_INTEGER_VECTOR2:
                case DataType::FLOAT_VECTOR2:
                    return 2;

                case DataType::BYTE_VECTOR3:
                case DataType::BYTE_VECTOR3_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR3:
                case DataType::UNSIGNED_BYTE_VECTOR3_NORM:
                case DataType::SHORT_VECTOR3:
                case DataType::SHORT_VECTOR3_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR3:
                case DataType::UNSIGNED_SHORT_VECTOR3_NORM:
                case DataType::INTEGER_VECTOR3:
                case DataType::UNSIGNED_INTEGER_VECTOR3:
                case DataType::FLOAT_VECTOR3:
                    return 3;

                case DataType::BYTE_VECTOR4:
                case DataType::BYTE_VECTOR4_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR4:
                case DataType::UNSIGNED_BYTE_VECTOR4_NORM:
                case DataType::SHORT_VECTOR4:
                case DataType::SHORT_VECTOR4_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR4:
                case DataType::UNSIGNED_SHORT_VECTOR4_NORM:
                case DataType::INTEGER_VECTOR4:
                case DataType::UNSIGNED_INTEGER_VECTOR4:
                case DataType::FLOAT_VECTOR4:
                    return 4;

                case DataType::FLOAT_MATRIX3:
                    return 3 * 3;

                case DataType::FLOAT_MATRIX4:
                    return 4 * 4;

                default:
                    return 0;
            }
        }

        static GLboolean isNormalized(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::BYTE_NORM:
                case DataType::BYTE_VECTOR2_NORM:
                case DataType::BYTE_VECTOR3_NORM:
                case DataType::BYTE_VECTOR4_NORM:
                case DataType::UNSIGNED_BYTE_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR2_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR3_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR4_NORM:
                case DataType::SHORT_NORM:
                case DataType::SHORT_VECTOR2_NORM:
                case DataType::SHORT_VECTOR3_NORM:
                case DataType::SHORT_VECTOR4_NORM:
                case DataType::UNSIGNED_SHORT_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR2_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR3_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR4_NORM:
                    return GL_TRUE;
                default:
                    return GL_FALSE;
            }
        }

        RenderDeviceOGL::RenderDeviceOGL(const std::function<void(const Event&)>& initCallback):
            RenderDevice(Driver::OPENGL, initCallback)
        {
            projectionTransform = Matrix4(1.0F, 0.0F, 0.0F, 0.0F,
                                          0.0F, 1.0F, 0.0F, 0.0F,
                                          0.0F, 0.0F, 2.0F, -1.0F,
                                          0.0F, 0.0F, 0.0F, 1.0F);

            renderTargetProjectionTransform = Matrix4(1.0F, 0.0F, 0.0F, 0.0F,
                                                      0.0F, -1.0F, 0.0F, 0.0F,
                                                      0.0F, 0.0F, 2.0F, -1.0F,
                                                      0.0F, 0.0F, 0.0F, 1.0F);
        }

        RenderDeviceOGL::~RenderDeviceOGL()
        {
            if (vertexArrayId) glDeleteVertexArraysProc(1, &vertexArrayId);

            resources.clear();
        }

        void RenderDeviceOGL::init(Window* newWindow,
                                   const Size2& newSize,
                                   uint32_t newSampleCount,
                                   Texture::Filter newTextureFilter,
                                   uint32_t newMaxAnisotropy,
                                   bool newVerticalSync,
                                   bool newDepth,
                                   bool newDebugRenderer)
        {
            RenderDevice::init(newWindow,
                               newSize,
                               newSampleCount,
                               newTextureFilter,
                               newMaxAnisotropy,
                               newVerticalSync,
                               newDepth,
                               newDebugRenderer);

            frameBufferWidth = static_cast<GLsizei>(size.width);
            frameBufferHeight = static_cast<GLsizei>(size.height);

            glGetStringProc = getCoreProcAddress<PFNGLGETSTRINGPROC>("glGetString");
            glGetIntegervProc = getCoreProcAddress<PFNGLGETINTEGERVPROC>("glGetIntegerv");
            glGetErrorProc = getCoreProcAddress<PFNGLGETERRORPROC>("glGetError");

#if OUZEL_OPENGL_INTERFACE_WGL
            const GLubyte* versionPtr = glGetStringProc(GL_VERSION);

            if (!versionPtr)
                throw SystemError("Failed to get OpenGL version");

            std::string versionStr(reinterpret_cast<const char*>(versionPtr));
            std::string versionParts[2];
            uint32_t part = 0;

            for (char c : versionStr)
            {
                if (c == '.')
                {
                    if (++part > 1) break;
                }
                else
                    versionParts[part] += c;
            }

            apiMajorVersion = static_cast<uint16_t>(std::stoi(versionParts[0]));
            apiMinorVersion = static_cast<uint16_t>(std::stoi(versionParts[1]));

            if (apiMajorVersion < 2 ||
                apiMajorVersion > 4)
                throw SystemError("Unsupported OpenGL version");
#endif

            const GLubyte* deviceName = glGetStringProc(GL_RENDERER);

            GLenum error;

            if ((error = glGetErrorProc()) != GL_NO_ERROR || !deviceName)
                engine->log(Log::Level::WARN) << "Failed to get OpenGL renderer, error: " + std::to_string(error);
            else
                engine->log(Log::Level::INFO) << "Using " << reinterpret_cast<const char*>(deviceName) << " for rendering";

            glEnableProc = getCoreProcAddress<PFNGLENABLEPROC>("glEnable");
            glDisableProc = getCoreProcAddress<PFNGLDISABLEPROC>("glDisable");
            glBindTextureProc = getCoreProcAddress<PFNGLBINDTEXTUREPROC>("glBindTexture");
            glGenTexturesProc = getCoreProcAddress<PFNGLGENTEXTURESPROC>("glGenTextures");
            glDeleteTexturesProc = getCoreProcAddress<PFNGLDELETETEXTURESPROC>("glDeleteTextures");
            glTexParameteriProc = getCoreProcAddress<PFNGLTEXPARAMETERIPROC>("glTexParameteri");
            glTexImage2DProc = getCoreProcAddress<PFNGLTEXIMAGE2DPROC>("glTexImage2D");
            glTexSubImage2DProc = getCoreProcAddress<PFNGLTEXSUBIMAGE2DPROC>("glTexSubImage2D");
            glViewportProc = getCoreProcAddress<PFNGLVIEWPORTPROC>("glViewport");
            glClearProc = getCoreProcAddress<PFNGLCLEARPROC>("glClear");
            glClearColorProc = getCoreProcAddress<PFNGLCLEARCOLORPROC>("glClearColor");
            glColorMaskProc = getCoreProcAddress<PFNGLCOLORMASKPROC>("glColorMask");
            glDepthMaskProc = getCoreProcAddress<PFNGLDEPTHMASKPROC>("glDepthMask");
            glDepthFuncProc = getCoreProcAddress<PFNGLDEPTHFUNCPROC>("glDepthFunc");
            glCullFaceProc = getCoreProcAddress<PFNGLCULLFACEPROC>("glCullFace");
            glScissorProc = getCoreProcAddress<PFNGLSCISSORPROC>("glScissor");
            glDrawElementsProc = getCoreProcAddress<PFNGLDRAWELEMENTSPROC>("glDrawElements");
            glReadPixelsProc = getCoreProcAddress<PFNGLREADPIXELSPROC>("glReadPixels");

            glBlendFuncSeparateProc = getCoreProcAddress<PFNGLBLENDFUNCSEPARATEPROC>("glBlendFuncSeparate");
            glBlendEquationSeparateProc = getCoreProcAddress<PFNGLBLENDEQUATIONSEPARATEPROC>("glBlendEquationSeparate");

            glUniform1iProc = getCoreProcAddress<PFNGLUNIFORM1IPROC>("glUniform1i");
            glUniform1fvProc = getCoreProcAddress<PFNGLUNIFORM1FVPROC>("glUniform1fv");
            glUniform2fvProc = getCoreProcAddress<PFNGLUNIFORM2FVPROC>("glUniform2fv");
            glUniform3fvProc = getCoreProcAddress<PFNGLUNIFORM3FVPROC>("glUniform3fv");
            glUniform4fvProc = getCoreProcAddress<PFNGLUNIFORM4FVPROC>("glUniform4fv");
            glUniform1ivProc = getCoreProcAddress<PFNGLUNIFORM1IVPROC>("glUniform1iv");
            glUniform2ivProc = getCoreProcAddress<PFNGLUNIFORM2IVPROC>("glUniform2iv");
            glUniform3ivProc = getCoreProcAddress<PFNGLUNIFORM3IVPROC>("glUniform3iv");
            glUniform4ivProc = getCoreProcAddress<PFNGLUNIFORM4IVPROC>("glUniform4iv");
            glUniformMatrix3fvProc = getCoreProcAddress<PFNGLUNIFORMMATRIX3FVPROC>("glUniformMatrix3fv");
            glUniformMatrix4fvProc = getCoreProcAddress<PFNGLUNIFORMMATRIX4FVPROC>("glUniformMatrix4fv");

            glActiveTextureProc = getCoreProcAddress<PFNGLACTIVETEXTUREPROC>("glActiveTexture");

#if OUZEL_SUPPORTS_OPENGLES
            glClearDepthfProc = getCoreProcAddress<PFNGLCLEARDEPTHFPROC>("glClearDepthf");
#else
            glPolygonModeProc = getCoreProcAddress<PFNGLPOLYGONMODEPROC>("glPolygonMode");
            glClearDepthProc = getCoreProcAddress<PFNGLCLEARDEPTHPROC>("glClearDepth");
#endif

            glCreateShaderProc = getCoreProcAddress<PFNGLCREATESHADERPROC>("glCreateShader");
            glDeleteShaderProc = getCoreProcAddress<PFNGLDELETESHADERPROC>("glDeleteShader");
            glAttachShaderProc = getCoreProcAddress<PFNGLATTACHSHADERPROC>("glAttachShader");
            glDetachShaderProc = getCoreProcAddress<PFNGLDETACHSHADERPROC>("glDetachShader");
            glShaderSourceProc = getCoreProcAddress<PFNGLSHADERSOURCEPROC>("glShaderSource");
            glCompileShaderProc = getCoreProcAddress<PFNGLCOMPILESHADERPROC>("glCompileShader");
            glBindAttribLocationProc = getCoreProcAddress<PFNGLBINDATTRIBLOCATIONPROC>("glBindAttribLocation");
            glGetShaderivProc = getCoreProcAddress<PFNGLGETSHADERIVPROC>("glGetShaderiv");
            glGetShaderInfoLogProc = getCoreProcAddress<PFNGLGETSHADERINFOLOGPROC>("glGetShaderInfoLog");

            glCreateProgramProc = getCoreProcAddress<PFNGLCREATEPROGRAMPROC>("glCreateProgram");
            glDeleteProgramProc = getCoreProcAddress<PFNGLDELETEPROGRAMPROC>("glDeleteProgram");
            glUseProgramProc = getCoreProcAddress<PFNGLUSEPROGRAMPROC>("glUseProgram");
            glLinkProgramProc = getCoreProcAddress<PFNGLLINKPROGRAMPROC>("glLinkProgram");
            glGetProgramivProc = getCoreProcAddress<PFNGLGETPROGRAMIVPROC>("glGetProgramiv");
            glGetProgramInfoLogProc = getCoreProcAddress<PFNGLGETPROGRAMINFOLOGPROC>("glGetProgramInfoLog");
            glGetUniformLocationProc = getCoreProcAddress<PFNGLGETUNIFORMLOCATIONPROC>("glGetUniformLocation");

            glBindBufferProc = getCoreProcAddress<PFNGLBINDBUFFERPROC>("glBindBuffer");
            glDeleteBuffersProc = getCoreProcAddress<PFNGLDELETEBUFFERSPROC>("glDeleteBuffers");
            glGenBuffersProc = getCoreProcAddress<PFNGLGENBUFFERSPROC>("glGenBuffers");
            glBufferDataProc = getCoreProcAddress<PFNGLBUFFERDATAPROC>("glBufferData");
            glBufferSubDataProc = getCoreProcAddress<PFNGLBUFFERSUBDATAPROC>("glBufferSubData");

            glEnableVertexAttribArrayProc = getCoreProcAddress<PFNGLENABLEVERTEXATTRIBARRAYPROC>("glEnableVertexAttribArray");
            glDisableVertexAttribArrayProc = getCoreProcAddress<PFNGLDISABLEVERTEXATTRIBARRAYPROC>("glDisableVertexAttribArray");
            glVertexAttribPointerProc = getCoreProcAddress<PFNGLVERTEXATTRIBPOINTERPROC>("glVertexAttribPointer");

#if OUZEL_SUPPORTS_OPENGLES
            glGenFramebuffersProc = getCoreProcAddress<PFNGLGENFRAMEBUFFERSPROC>("glGenFramebuffers");
            glDeleteFramebuffersProc = getCoreProcAddress<PFNGLDELETEFRAMEBUFFERSPROC>("glDeleteFramebuffers");
            glBindFramebufferProc = getCoreProcAddress<PFNGLBINDFRAMEBUFFERPROC>("glBindFramebuffer");
            glCheckFramebufferStatusProc = getCoreProcAddress<PFNGLCHECKFRAMEBUFFERSTATUSPROC>("glCheckFramebufferStatus");
            glFramebufferRenderbufferProc = getCoreProcAddress<PFNGLFRAMEBUFFERRENDERBUFFERPROC>("glFramebufferRenderbuffer");
            glFramebufferTexture2DProc = getCoreProcAddress<PFNGLFRAMEBUFFERTEXTURE2DPROC>("glFramebufferTexture2D");

            glGenRenderbuffersProc = getCoreProcAddress<PFNGLGENRENDERBUFFERSPROC>("glGenRenderbuffers");
            glDeleteRenderbuffersProc = getCoreProcAddress<PFNGLDELETERENDERBUFFERSPROC>("glDeleteRenderbuffers");
            glBindRenderbufferProc = getCoreProcAddress<PFNGLBINDRENDERBUFFERPROC>("glBindRenderbuffer");
            glRenderbufferStorageProc = getCoreProcAddress<PFNGLRENDERBUFFERSTORAGEPROC>("glRenderbufferStorage");

#  if OUZEL_OPENGL_INTERFACE_EAGL
            glBlitFramebufferProc = getCoreProcAddress<PFNGLBLITFRAMEBUFFERPROC>("glBlitFramebuffer");

            glDiscardFramebufferEXTProc = getCoreProcAddress<PFNGLDISCARDFRAMEBUFFEREXTPROC>("glDiscardFramebufferEXT");
            glRenderbufferStorageMultisampleAPPLEProc = getCoreProcAddress<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC>("glRenderbufferStorageMultisampleAPPLE");
            glResolveMultisampleFramebufferAPPLEProc = getCoreProcAddress<PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC>("glResolveMultisampleFramebufferAPPLE");

#  endif
#endif

            std::vector<std::string> extensions;

            if (apiMajorVersion >= 3)
            {
                glGetStringiProc = getExtProcAddress<PFNGLGETSTRINGIPROC>("glGetStringi");

                GLint extensionCount;
                glGetIntegervProc(GL_NUM_EXTENSIONS, &extensionCount);

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    engine->log(Log::Level::WARN) << "Failed to get OpenGL extension count, error: " + std::to_string(error);
                else
                {
                    for (GLuint i = 0; i < static_cast<GLuint>(extensionCount); ++i)
                    {
                        std::string extension(reinterpret_cast<const char*>(glGetStringiProc(GL_EXTENSIONS, i)));

                        extensions.push_back(extension);
                    }
                }
            }
            else
            {
                const GLubyte* extensionPtr = glGetStringProc(GL_EXTENSIONS);

                if ((error = glGetErrorProc()) != GL_NO_ERROR || !extensionPtr)
                    engine->log(Log::Level::WARN) << "Failed to get OpenGL extensions";
                else
                {
                    std::istringstream extensionStringStream(reinterpret_cast<const char*>(extensionPtr));

                    for (std::string extension; extensionStringStream >> extension;)
                        extensions.push_back(extension);
                }
            }

            engine->log(Log::Level::ALL) << "Supported OpenGL extensions: " << extensions;

            anisotropicFilteringSupported = false;
            npotTexturesSupported = false;
            multisamplingSupported = false;
            textureBaseLevelSupported = false;
            textureMaxLevelSupported = false;
            renderTargetsSupported = false;

            if (apiMajorVersion >= 4)
            {
#if !OUZEL_SUPPORTS_OPENGLES
                if ((apiMajorVersion == 4 && apiMinorVersion >= 6) || // at least OpenGL 4.6
                    apiMajorVersion >= 5)
                    anisotropicFilteringSupported = true;

                if ((apiMajorVersion == 4 && apiMinorVersion >= 3) || // at least OpenGL 4.3
                    apiMajorVersion >= 5)
                    glCopyImageSubDataProc = getExtProcAddress<PFNGLCOPYIMAGESUBDATAPROC>("glCopyImageSubData");
#endif
            }

            if (apiMajorVersion >= 3)
            {
                npotTexturesSupported = true;
                multisamplingSupported = true;
                textureBaseLevelSupported = true;
                textureMaxLevelSupported = true;
                renderTargetsSupported = true;

                glUniform1uivProc = getExtProcAddress<PFNGLUNIFORM1UIVPROC>("glUniform1uiv");
                glUniform2uivProc = getExtProcAddress<PFNGLUNIFORM2UIVPROC>("glUniform2uiv");
                glUniform3uivProc = getExtProcAddress<PFNGLUNIFORM3UIVPROC>("glUniform3uiv");
                glUniform4uivProc = getExtProcAddress<PFNGLUNIFORM4UIVPROC>("glUniform4uiv");

#if OUZEL_OPENGL_INTERFACE_EAGL
                glGenVertexArraysProc = getExtProcAddress<PFNGLGENVERTEXARRAYSPROC>("glGenVertexArraysOES");
                glBindVertexArrayProc = getExtProcAddress<PFNGLBINDVERTEXARRAYPROC>("glBindVertexArrayOES");
                glDeleteVertexArraysProc = getExtProcAddress<PFNGLDELETEVERTEXARRAYSPROC>("glDeleteVertexArraysOES");

                glMapBufferProc = getExtProcAddress<PFNGLMAPBUFFEROESPROC>("glMapBufferOES");
                glUnmapBufferProc = getExtProcAddress<PFNGLUNMAPBUFFEROESPROC>("glUnmapBufferOES");
                glMapBufferRangeProc = getExtProcAddress<PFNGLMAPBUFFERRANGEPROC>("glMapBufferRangeEXT");

                glRenderbufferStorageMultisampleProc = getExtProcAddress<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>("glRenderbufferStorageMultisampleAPPLE");
#else
                glGenVertexArraysProc = getExtProcAddress<PFNGLGENVERTEXARRAYSPROC>("glGenVertexArrays");
                glBindVertexArrayProc = getExtProcAddress<PFNGLBINDVERTEXARRAYPROC>("glBindVertexArray");
                glDeleteVertexArraysProc = getExtProcAddress<PFNGLDELETEVERTEXARRAYSPROC>("glDeleteVertexArrays");
#  if OUZEL_OPENGL_INTERFACE_EGL
                glMapBufferProc = getExtProcAddress<PFNGLMAPBUFFEROESPROC>("glMapBuffer");
                glUnmapBufferProc = getExtProcAddress<PFNGLUNMAPBUFFEROESPROC>("glUnmapBuffer");
#  else
                glMapBufferProc = getExtProcAddress<PFNGLMAPBUFFERPROC>("glMapBuffer");
                glUnmapBufferProc = getExtProcAddress<PFNGLUNMAPBUFFERPROC>("glUnmapBuffer");

                glGenFramebuffersProc = getExtProcAddress<PFNGLGENFRAMEBUFFERSPROC>("glGenFramebuffers");
                glDeleteFramebuffersProc = getExtProcAddress<PFNGLDELETEFRAMEBUFFERSPROC>("glDeleteFramebuffers");
                glBindFramebufferProc = getExtProcAddress<PFNGLBINDFRAMEBUFFERPROC>("glBindFramebuffer");
                glCheckFramebufferStatusProc = getExtProcAddress<PFNGLCHECKFRAMEBUFFERSTATUSPROC>("glCheckFramebufferStatus");
                glFramebufferRenderbufferProc = getExtProcAddress<PFNGLFRAMEBUFFERRENDERBUFFERPROC>("glFramebufferRenderbuffer");
                glBlitFramebufferProc = getExtProcAddress<PFNGLBLITFRAMEBUFFERPROC>("glBlitFramebuffer");
                glFramebufferTexture2DProc = getExtProcAddress<PFNGLFRAMEBUFFERTEXTURE2DPROC>("glFramebufferTexture2D");

                glGenRenderbuffersProc = getExtProcAddress<PFNGLGENRENDERBUFFERSPROC>("glGenRenderbuffers");
                glDeleteRenderbuffersProc = getExtProcAddress<PFNGLDELETERENDERBUFFERSPROC>("glDeleteRenderbuffers");
                glBindRenderbufferProc = getExtProcAddress<PFNGLBINDRENDERBUFFERPROC>("glBindRenderbuffer");
                glRenderbufferStorageProc = getExtProcAddress<PFNGLRENDERBUFFERSTORAGEPROC>("glRenderbufferStorage");
#  endif
                glMapBufferRangeProc = getExtProcAddress<PFNGLMAPBUFFERRANGEPROC>("glMapBufferRange");
                glRenderbufferStorageMultisampleProc = getExtProcAddress<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>("glRenderbufferStorageMultisample");

#  if OUZEL_SUPPORTS_OPENGLES
                glFramebufferTexture2DMultisampleProc = getExtProcAddress<PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC>("glFramebufferTexture2DMultisample");
#  endif
#endif
            }

            if (apiMajorVersion >= 2)
            {
#if !OUZEL_SUPPORTS_OPENGLES
                textureBaseLevelSupported = true;
                textureMaxLevelSupported = true;
                renderTargetsSupported = true;
#endif
            }

            for (const std::string& extension : extensions)
            {
                if (extension == "GL_OES_texture_npot" ||
                    extension == "GL_ARB_texture_non_power_of_two")
                    npotTexturesSupported = true;
                else if (extension == "GL_EXT_debug_marker")
                {
                    glPushGroupMarkerEXTProc = getExtProcAddress<PFNGLPUSHGROUPMARKEREXTPROC>("glPushGroupMarkerEXT");
                    glPopGroupMarkerEXTProc = getExtProcAddress<PFNGLPOPGROUPMARKEREXTPROC>("glPopGroupMarkerEXT");
                }
                else if (extension == "GL_EXT_texture_filter_anisotropic")
                    anisotropicFilteringSupported = true;
                else if (extension == "GL_EXT_map_buffer_range")
                {
#if OUZEL_SUPPORTS_OPENGLES
                    glMapBufferRangeProc = getExtProcAddress<PFNGLMAPBUFFERRANGEEXTPROC>("glMapBufferRangeEXT");
#else
                    glMapBufferRangeProc = getExtProcAddress<PFNGLMAPBUFFERRANGEPROC>("glMapBufferRangeEXT");
#endif
                }
#if OUZEL_SUPPORTS_OPENGLES
                else if (extension == "GL_APPLE_framebuffer_multisample")
                {
                    multisamplingSupported = true;
                    glRenderbufferStorageMultisampleProc = getExtProcAddress<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>("glRenderbufferStorageMultisampleAPPLE");
                }
                else if (extension == "GL_APPLE_texture_max_level")
                    textureMaxLevelSupported = true;
                else if (extension == "GL_OES_vertex_array_object")
                {
                    glGenVertexArraysProc = getExtProcAddress<PFNGLGENVERTEXARRAYSOESPROC>("glGenVertexArraysOES");
                    glBindVertexArrayProc = getExtProcAddress<PFNGLBINDVERTEXARRAYOESPROC>("glBindVertexArrayOES");
                    glDeleteVertexArraysProc = getExtProcAddress<PFNGLDELETEVERTEXARRAYSOESPROC>("glDeleteVertexArraysOES");
                }
                else if (extension == "GL_OES_mapbuffer")
                {
                    glMapBufferProc = getExtProcAddress<PFNGLMAPBUFFEROESPROC>("glMapBufferOES");
                    glUnmapBufferProc = getExtProcAddress<PFNGLUNMAPBUFFEROESPROC>("glUnmapBufferOES");
                }
#  if !OUZEL_OPENGL_INTERFACE_EAGL
                else if (extension == "GL_EXT_copy_image")
                    glCopyImageSubDataProc = getExtProcAddress<PFNGLCOPYIMAGESUBDATAEXTPROC>("glCopyImageSubDataEXT");
                else if (extension == "GL_EXT_multisampled_render_to_texture")
                {
                    multisamplingSupported = true;
                    glRenderbufferStorageMultisampleProc = getExtProcAddress<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC>("glRenderbufferStorageMultisampleEXT");
                }
                else if (extension == "GL_IMG_multisampled_render_to_texture")
                {
                    multisamplingSupported = true;
                    glRenderbufferStorageMultisampleProc = getExtProcAddress<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC>("glRenderbufferStorageMultisampleIMG");
                    glFramebufferTexture2DMultisampleProc = getExtProcAddress<PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC>("glFramebufferTexture2DMultisampleIMG");
                }
#  endif
#else
                else if (extension == "GL_ARB_copy_image")
                    glCopyImageSubDataProc = getExtProcAddress<PFNGLCOPYIMAGESUBDATAPROC>("glCopyImageSubData");
                else if (extension == "GL_ARB_vertex_array_object")
                {
                    glGenVertexArraysProc = getExtProcAddress<PFNGLGENVERTEXARRAYSPROC>("glGenVertexArrays");
                    glBindVertexArrayProc = getExtProcAddress<PFNGLBINDVERTEXARRAYPROC>("glBindVertexArray");
                    glDeleteVertexArraysProc = getExtProcAddress<PFNGLDELETEVERTEXARRAYSPROC>("glDeleteVertexArrays");
                }
                else if (extension == "GL_EXT_framebuffer_object")
                {
                    renderTargetsSupported = true;

                    glGenFramebuffersProc = getExtProcAddress<PFNGLGENFRAMEBUFFERSPROC>("glGenFramebuffers");
                    glDeleteFramebuffersProc = getExtProcAddress<PFNGLDELETEFRAMEBUFFERSPROC>("glDeleteFramebuffers");
                    glBindFramebufferProc = getExtProcAddress<PFNGLBINDFRAMEBUFFERPROC>("glBindFramebuffer");
                    glCheckFramebufferStatusProc = getExtProcAddress<PFNGLCHECKFRAMEBUFFERSTATUSPROC>("glCheckFramebufferStatus");
                    glFramebufferRenderbufferProc = getExtProcAddress<PFNGLFRAMEBUFFERRENDERBUFFERPROC>("glFramebufferRenderbuffer");
                    glFramebufferTexture2DProc = getExtProcAddress<PFNGLFRAMEBUFFERTEXTURE2DPROC>("glFramebufferTexture2D");

                    glGenRenderbuffersProc = getExtProcAddress<PFNGLGENRENDERBUFFERSPROC>("glGenRenderbuffers");
                    glDeleteRenderbuffersProc = getExtProcAddress<PFNGLDELETERENDERBUFFERSPROC>("glDeleteRenderbuffers");
                    glBindRenderbufferProc = getExtProcAddress<PFNGLBINDRENDERBUFFERPROC>("glBindRenderbuffer");
                    glRenderbufferStorageProc = getExtProcAddress<PFNGLRENDERBUFFERSTORAGEPROC>("glRenderbufferStorage");
                }
                else if (extension == "GL_EXT_framebuffer_blit")
                    glBlitFramebufferProc = getExtProcAddress<PFNGLBLITFRAMEBUFFERPROC>("glBlitFramebuffer");
                else if (extension == "GL_EXT_framebuffer_multisample")
                {
                    multisamplingSupported = true;
                    glRenderbufferStorageMultisampleProc = getExtProcAddress<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>("glRenderbufferStorageMultisample");
                }
#endif
            }

            if (!multisamplingSupported) sampleCount = 1;

            glDisableProc(GL_DITHER);

            if ((error = glGetErrorProc()) != GL_NO_ERROR)
                throw SystemError("Failed to set depth function, error: " + std::to_string(error));

#if !OUZEL_SUPPORTS_OPENGLES
            if (sampleCount > 1)
            {
                glEnableProc(GL_MULTISAMPLE);

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw SystemError("Failed to enable multi-sampling, error: " + std::to_string(error));
            }
#endif

            clearMask = 0;
            if (clearColorBuffer) clearMask |= GL_COLOR_BUFFER_BIT;
            if (clearDepthBuffer) clearMask |= GL_DEPTH_BUFFER_BIT;

            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();

            if (glGenVertexArraysProc)
            {
                glGenVertexArraysProc(1, &vertexArrayId);

                glBindVertexArrayProc(vertexArrayId);

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw DataError("Failed to bind vertex array, error: " + std::to_string(error));
            }
        }

        void RenderDeviceOGL::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            if (clearColorBuffer)
                clearMask |= GL_COLOR_BUFFER_BIT;
            else
                clearMask &= ~static_cast<GLbitfield>(GL_COLOR_BUFFER_BIT);

        }

        void RenderDeviceOGL::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            if (clearDepthBuffer)
                clearMask |= GL_DEPTH_BUFFER_BIT;
            else
                clearMask &= ~static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT);
        }

        void RenderDeviceOGL::setClearColor(Color newClearColor)
        {
            clearColor = newClearColor;

            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();
        }

        void RenderDeviceOGL::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;
        }

        void RenderDeviceOGL::setSize(const Size2& newSize)
        {
            RenderDevice::setSize(newSize);

            frameBufferWidth = static_cast<GLsizei>(size.width);
            frameBufferHeight = static_cast<GLsizei>(size.height);
        }

        static void setUniform(GLint location, DataType dataType, const void* data)
        {
            switch (dataType)
            {
                case DataType::INTEGER:
                    glUniform1ivProc(location, 1, reinterpret_cast<const GLint*>(data));
                    break;
                case DataType::UNSIGNED_INTEGER:
                    if (!glUniform1uivProc) throw DataError("Unsupported uniform size");
                    glUniform1uivProc(location, 1, reinterpret_cast<const GLuint*>(data));
                    break;
                case DataType::INTEGER_VECTOR2:
                    glUniform2ivProc(location, 1, reinterpret_cast<const GLint*>(data));
                    break;
                case DataType::UNSIGNED_INTEGER_VECTOR2:
                    if (!glUniform2uivProc) throw DataError("Unsupported uniform size");
                    glUniform2uivProc(location, 1, reinterpret_cast<const GLuint*>(data));
                    break;
                case DataType::INTEGER_VECTOR3:
                    glUniform3ivProc(location, 1, reinterpret_cast<const GLint*>(data));
                    break;
                case DataType::UNSIGNED_INTEGER_VECTOR3:
                    if (!glUniform3uivProc) throw DataError("Unsupported uniform size");
                    glUniform3uivProc(location, 1, reinterpret_cast<const GLuint*>(data));
                    break;
                case DataType::INTEGER_VECTOR4:
                    glUniform4ivProc(location, 1, reinterpret_cast<const GLint*>(data));
                    break;
                case DataType::UNSIGNED_INTEGER_VECTOR4:
                    if (!glUniform4uivProc) throw DataError("Unsupported uniform size");
                    glUniform4uivProc(location, 1, reinterpret_cast<const GLuint*>(data));
                    break;
                case DataType::FLOAT:
                    glUniform1fvProc(location, 1, reinterpret_cast<const GLfloat*>(data));
                    break;
                case DataType::FLOAT_VECTOR2:
                    glUniform2fvProc(location, 1, reinterpret_cast<const GLfloat*>(data));
                    break;
                case DataType::FLOAT_VECTOR3:
                    glUniform3fvProc(location, 1, reinterpret_cast<const GLfloat*>(data));
                    break;
                case DataType::FLOAT_VECTOR4:
                    glUniform4fvProc(location, 1, reinterpret_cast<const GLfloat*>(data));
                    break;
                case DataType::FLOAT_MATRIX3:
                    glUniformMatrix3fvProc(location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(data));
                    break;
                case DataType::FLOAT_MATRIX4:
                    glUniformMatrix4fvProc(location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(data));
                    break;
                default:
                    throw DataError("Unsupported uniform size");
            }
        }

        void RenderDeviceOGL::process()
        {
            RenderDevice::process();
            executeAll();

            ShaderResourceOGL* currentShader = nullptr;

            CommandBuffer commandBuffer;

            for (;;)
            {
                std::unique_lock<std::mutex> lock(commandQueueMutex);
                while (commandQueue.empty()) commandQueueCondition.wait(lock);
                commandBuffer = std::move(commandQueue.front());
                commandQueue.pop();
                lock.unlock();

                std::unique_ptr<Command> command;

                while (!commandBuffer.commands.empty())
                {
                    command = std::move(commandBuffer.commands.front());
                    commandBuffer.commands.pop();

                    switch (command->type)
                    {
                        case Command::Type::PRESENT:
                        {
                            present();
                            break;
                        }

                        case Command::Type::DELETE_RESOURCE:
                        {
                            const DeleteResourceCommand* deleteResourceCommand = static_cast<const DeleteResourceCommand*>(command.get());
                            resources[deleteResourceCommand->resource - 1].reset();
                            break;
                        }

                        case Command::Type::SET_RENDER_TARGET:
                        {
                            const SetRenderTargetCommand* setRenderTargetCommand = static_cast<const SetRenderTargetCommand*>(command.get());

                            GLuint newFrameBufferId = 0;

                            if (setRenderTargetCommand->renderTarget)
                            {
                                TextureResourceOGL* renderTargetOGL = static_cast<TextureResourceOGL*>(resources[setRenderTargetCommand->renderTarget - 1].get());

                                if (!renderTargetOGL->getFrameBufferId()) break;
                                newFrameBufferId = renderTargetOGL->getFrameBufferId();
                            }
                            else
                                newFrameBufferId = frameBufferId;

                            bindFrameBuffer(newFrameBufferId);

                            // TODO: update cull mode

                            break;
                        }

                        case Command::Type::SET_RENDER_TARGET_PARAMETERS:
                        {
                            const SetRenderTargetParametersCommand* setRenderTargetParametersCommand = static_cast<const SetRenderTargetParametersCommand*>(command.get());

                            if (setRenderTargetParametersCommand->renderTarget)
                            {
                                TextureResourceOGL* renderTargetOGL = static_cast<TextureResourceOGL*>(resources[setRenderTargetParametersCommand->renderTarget - 1].get());
                                renderTargetOGL->setClearColorBuffer(setRenderTargetParametersCommand->clearColorBuffer);
                                renderTargetOGL->setClearDepthBuffer(setRenderTargetParametersCommand->clearDepthBuffer);
                                renderTargetOGL->setClearColor(setRenderTargetParametersCommand->clearColor);
                                renderTargetOGL->setClearDepth(setRenderTargetParametersCommand->clearDepth);
                            }
                            else
                            {
                                setClearColorBuffer(setRenderTargetParametersCommand->clearColorBuffer);
                                setClearDepthBuffer(setRenderTargetParametersCommand->clearDepthBuffer);
                                setClearColor(setRenderTargetParametersCommand->clearColor);
                                setClearDepth(setRenderTargetParametersCommand->clearDepth);
                            }

                            break;
                        }

                        case Command::Type::CLEAR_RENDER_TARGET:
                        {
                            const ClearRenderTargetCommand* clearCommand = static_cast<const ClearRenderTargetCommand*>(command.get());

                            GLuint newFrameBufferId = 0;
                            GLbitfield newClearMask = 0;
                            const std::array<float, 4>* newClearColor;
                            GLfloat newClearDepth;
                            GLsizei renderTargetWidth = 0;
                            GLsizei renderTargetHeight = 0;

                            if (clearCommand->renderTarget)
                            {
                                TextureResourceOGL* renderTargetOGL = static_cast<TextureResourceOGL*>(resources[clearCommand->renderTarget - 1].get());

                                if (!renderTargetOGL->getFrameBufferId()) break;

                                renderTargetWidth = renderTargetOGL->getWidth();
                                renderTargetHeight = renderTargetOGL->getHeight();
                                newFrameBufferId = renderTargetOGL->getFrameBufferId();
                                newClearColor = &renderTargetOGL->getFrameBufferClearColor();
                                newClearDepth = renderTargetOGL->getClearDepth();
                                newClearMask = renderTargetOGL->getClearMask();
                            }
                            else
                            {
                                renderTargetWidth = frameBufferWidth;
                                renderTargetHeight = frameBufferHeight;
                                newFrameBufferId = frameBufferId;
                                newClearColor = &frameBufferClearColor;
                                newClearDepth = clearDepth;
                                newClearMask = clearMask;
                            }

                            if (newClearMask)
                            {
                                bindFrameBuffer(newFrameBufferId);

                                setViewport(0, 0,
                                            renderTargetWidth,
                                            renderTargetHeight);

                                setScissorTest(false, 0, 0, renderTargetWidth, renderTargetHeight);

                                if (newClearMask & GL_DEPTH_BUFFER_BIT)
                                {
                                    // allow clearing the depth buffer
                                    setDepthMask(true);
                                    setClearDepthValue(newClearDepth);
                                }

                                if (newClearMask & GL_COLOR_BUFFER_BIT)
                                    setClearColorValue(*newClearColor);

                                glClearProc(newClearMask);

                                GLenum error;

                                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                                    throw DataError("Failed to clear frame buffer, error: " + std::to_string(error));
                            }

                            break;
                        }

                        case Command::Type::BLIT:
                        {
#if !OUZEL_SUPPORTS_OPENGLES
                            const BlitCommand* blitCommand = static_cast<const BlitCommand*>(command.get());

                            TextureResourceOGL* sourceTextureOGL = static_cast<TextureResourceOGL*>(resources[blitCommand->sourceTexture - 1].get());
                            TextureResourceOGL* destinationTextureOGL = static_cast<TextureResourceOGL*>(resources[blitCommand->destinationTexture - 1].get());

                            if (glCopyImageSubDataProc)
                                glCopyImageSubDataProc(sourceTextureOGL->getTextureId(),
                                                       GL_TEXTURE_2D,
                                                       static_cast<GLint>(blitCommand->sourceLevel),
                                                       static_cast<GLint>(blitCommand->sourceX),
                                                       static_cast<GLint>(blitCommand->sourceY),
                                                       0,
                                                       destinationTextureOGL->getTextureId(),
                                                       GL_TEXTURE_2D,
                                                       static_cast<GLint>(blitCommand->destinationLevel),
                                                       static_cast<GLint>(blitCommand->destinationX),
                                                       static_cast<GLint>(blitCommand->destinationY),
                                                       0,
                                                       static_cast<GLsizei>(blitCommand->sourceWidth),
                                                       static_cast<GLsizei>(blitCommand->sourceHeight),
                                                       0);
#endif
                            // TODO: copy data if glCopyImageSubData is not available
                            break;
                        }

                        case Command::Type::SET_CULL_MODE:
                        {
                            const SetCullModeCommad* setCullModeCommad = static_cast<const SetCullModeCommad*>(command.get());

                            GLenum cullFace = GL_NONE;

                            switch (setCullModeCommad->cullMode)
                            {
                                case CullMode::NONE: cullFace = GL_NONE; break;
                                case CullMode::FRONT: cullFace = ((stateCache.frameBufferId != frameBufferId) ? GL_FRONT : GL_BACK); break; // flip the faces, because of the flipped y-axis
                                case CullMode::BACK: cullFace = ((stateCache.frameBufferId != frameBufferId) ? GL_BACK : GL_FRONT); break;
                                default: throw DataError("Invalid cull mode");
                            }

                            setCullFace(cullFace != GL_NONE, cullFace);

                            break;
                        }

                        case Command::Type::SET_FILL_MODE:
                        {
                            const SetFillModeCommad* setFillModeCommad = static_cast<const SetFillModeCommad*>(command.get());

#if OUZEL_SUPPORTS_OPENGLES
                            if (setFillModeCommad->fillMode != FillMode::SOLID)
                                engine->log(Log::Level::WARN) << "Unsupported fill mode";
#else
                            GLenum fillMode = GL_NONE;

                            switch (setFillModeCommad->fillMode)
                            {
                                case FillMode::SOLID: fillMode = GL_FILL; break;
                                case FillMode::WIREFRAME: fillMode = GL_LINE; break;
                                default: throw DataError("Invalid fill mode");
                            }

                            setPolygonFillMode(fillMode);
#endif
                            break;
                        }

                        case Command::Type::SET_SCISSOR_TEST:
                        {
                            const SetScissorTestCommand* setScissorTestCommand = static_cast<const SetScissorTestCommand*>(command.get());

                            setScissorTest(setScissorTestCommand->enabled,
                                           static_cast<GLint>(setScissorTestCommand->rectangle.position.x),
                                           static_cast<GLint>(setScissorTestCommand->rectangle.position.y),
                                           static_cast<GLsizei>(setScissorTestCommand->rectangle.size.width),
                                           static_cast<GLsizei>(setScissorTestCommand->rectangle.size.height));

                            break;
                        }

                        case Command::Type::SET_VIEWPORT:
                        {
                            const SetViewportCommand* setViewportCommand = static_cast<const SetViewportCommand*>(command.get());

                            setViewport(static_cast<GLint>(setViewportCommand->viewport.position.x),
                                        static_cast<GLint>(setViewportCommand->viewport.position.y),
                                        static_cast<GLsizei>(setViewportCommand->viewport.size.width),
                                        static_cast<GLsizei>(setViewportCommand->viewport.size.height));

                            break;
                        }

                        case Command::Type::INIT_DEPTH_STENCIL_STATE:
                        {
                            const InitDepthStencilStateCommand* initDepthStencilStateCommand = static_cast<const InitDepthStencilStateCommand*>(command.get());
                            std::unique_ptr<DepthStencilStateResourceOGL> depthStencilStateResourceOGL(new DepthStencilStateResourceOGL(*this,
                                                                                                                                        initDepthStencilStateCommand->depthTest,
                                                                                                                                        initDepthStencilStateCommand->depthWrite,
                                                                                                                                        initDepthStencilStateCommand->compareFunction));

                            if (initDepthStencilStateCommand->depthStencilState > resources.size())
                                resources.resize(initDepthStencilStateCommand->depthStencilState);
                            resources[initDepthStencilStateCommand->depthStencilState - 1] = std::move(depthStencilStateResourceOGL);
                            break;
                        }

                        case Command::Type::SET_DEPTH_STENCIL_STATE:
                        {
                            const SetDepthStencilStateCommand* setDepthStencilStateCommand = static_cast<const SetDepthStencilStateCommand*>(command.get());

                            if (setDepthStencilStateCommand->depthStencilState)
                            {
                                DepthStencilStateResourceOGL* depthStencilStateResourceOGL = static_cast<DepthStencilStateResourceOGL*>(resources[setDepthStencilStateCommand->depthStencilState - 1].get());

                                enableDepthTest(depthStencilStateResourceOGL->getDepthTest());
                                setDepthMask(depthStencilStateResourceOGL->getDepthMask());
                                glDepthFuncProc(depthStencilStateResourceOGL->getCompareFunction());
                            }
                            else
                            {
                                enableDepthTest(false);
                                setDepthMask(GL_FALSE);
                                setDepthFunc(GL_LESS);
                            }

                            break;
                        }

                        case Command::Type::SET_PIPELINE_STATE:
                        {
                            const SetPipelineStateCommand* setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command.get());

                            BlendStateResourceOGL* blendStateOGL = static_cast<BlendStateResourceOGL*>(resources[setPipelineStateCommand->blendState - 1].get());
                            ShaderResourceOGL* shaderOGL = static_cast<ShaderResourceOGL*>(resources[setPipelineStateCommand->shader - 1].get());
                            currentShader = shaderOGL;

                            if (blendStateOGL)
                            {
                                setBlendState(blendStateOGL->isBlendEnabled(),
                                              blendStateOGL->getModeRGB(),
                                              blendStateOGL->getModeAlpha(),
                                              blendStateOGL->getSourceFactorRGB(),
                                              blendStateOGL->getDestFactorRGB(),
                                              blendStateOGL->getSourceFactorAlpha(),
                                              blendStateOGL->getDestFactorAlpha());

                                setColorMask(blendStateOGL->getRedMask(),
                                             blendStateOGL->getGreenMask(),
                                             blendStateOGL->getBlueMask(),
                                             blendStateOGL->getAlphaMask());
                            }
                            else
                            {
                                setBlendState(false, 0, 0, 0, 0, 0, 0);
                                setColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                            }

                            if (shaderOGL)
                            {
                                assert(shaderOGL->getProgramId());
                                useProgram(shaderOGL->getProgramId());
                            }
                            else
                                useProgram(0);

                            break;
                        }

                        case Command::Type::DRAW:
                        {
                            const DrawCommand* drawCommand = static_cast<const DrawCommand*>(command.get());

                            // mesh buffer
                            BufferResourceOGL* indexBufferOGL = static_cast<BufferResourceOGL*>(resources[drawCommand->indexBuffer - 1].get());
                            BufferResourceOGL* vertexBufferOGL = static_cast<BufferResourceOGL*>(resources[drawCommand->vertexBuffer - 1].get());

                            assert(indexBufferOGL);
                            assert(indexBufferOGL->getBufferId());
                            assert(vertexBufferOGL);
                            assert(vertexBufferOGL->getBufferId());

                            // draw
                            GLenum mode;

                            switch (drawCommand->drawMode)
                            {
                                case DrawMode::POINT_LIST: mode = GL_POINTS; break;
                                case DrawMode::LINE_LIST: mode = GL_LINES; break;
                                case DrawMode::LINE_STRIP: mode = GL_LINE_STRIP; break;
                                case DrawMode::TRIANGLE_LIST: mode = GL_TRIANGLES; break;
                                case DrawMode::TRIANGLE_STRIP: mode = GL_TRIANGLE_STRIP; break;
                                default: throw DataError("Invalid draw mode");
                            }

                            bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferOGL->getBufferId());
                            bindBuffer(GL_ARRAY_BUFFER, vertexBufferOGL->getBufferId());

                            GLuint vertexOffset = 0;

                            for (GLuint index = 0; index < Vertex::ATTRIBUTES.size(); ++index)
                            {
                                const Vertex::Attribute& vertexAttribute = Vertex::ATTRIBUTES[index];

                                glEnableVertexAttribArrayProc(index);
                                glVertexAttribPointerProc(index,
                                                          getArraySize(vertexAttribute.dataType),
                                                          getVertexFormat(vertexAttribute.dataType),
                                                          isNormalized(vertexAttribute.dataType),
                                                          static_cast<GLsizei>(sizeof(Vertex)),
                                                          static_cast<const GLchar*>(nullptr) + vertexOffset);

                                vertexOffset += getDataTypeSize(vertexAttribute.dataType);
                            }

                            GLenum error;

                            if ((error = glGetErrorProc()) != GL_NO_ERROR)
                                throw DataError("Failed to update vertex attributes, error: " + std::to_string(error));

                            assert(drawCommand->indexCount);
                            assert(indexBufferOGL->getSize());
                            assert(vertexBufferOGL->getSize());

                            GLenum indexType;

                            switch (drawCommand->indexSize)
                            {
                                case 2: indexType = GL_UNSIGNED_SHORT; break;
                                case 4: indexType = GL_UNSIGNED_INT; break;
                                default: throw DataError("Invalid index size");
                            }

                            glDrawElementsProc(mode,
                                               static_cast<GLsizei>(drawCommand->indexCount),
                                               indexType,
                                               static_cast<const char*>(nullptr) + (drawCommand->startIndex * drawCommand->indexSize));

                            if ((error = glGetErrorProc()) != GL_NO_ERROR)
                                throw DataError("Failed to draw elements, error: " + std::to_string(error));

                            break;
                        }

                        case Command::Type::PUSH_DEBUG_MARKER:
                        {
                            const PushDebugMarkerCommand* pushDebugMarkerCommand = static_cast<const PushDebugMarkerCommand*>(command.get());
                            if (glPushGroupMarkerEXTProc) glPushGroupMarkerEXTProc(0, pushDebugMarkerCommand->name.c_str());
                            break;
                        }

                        case Command::Type::POP_DEBUG_MARKER:
                        {
                            if (glPopGroupMarkerEXTProc) glPopGroupMarkerEXTProc();
                            break;
                        }

                        case Command::Type::INIT_BLEND_STATE:
                        {
                            const InitBlendStateCommand* initBlendStateCommand = static_cast<const InitBlendStateCommand*>(command.get());

                            std::unique_ptr<BlendStateResourceOGL> blendStateResourceOGL(new BlendStateResourceOGL(*this,
                                                                                                                   initBlendStateCommand->enableBlending,
                                                                                                                   initBlendStateCommand->colorBlendSource,
                                                                                                                   initBlendStateCommand->colorBlendDest,
                                                                                                                   initBlendStateCommand->colorOperation,
                                                                                                                   initBlendStateCommand->alphaBlendSource,
                                                                                                                   initBlendStateCommand->alphaBlendDest,
                                                                                                                   initBlendStateCommand->alphaOperation,
                                                                                                                   initBlendStateCommand->colorMask));

                            if (initBlendStateCommand->blendState > resources.size())
                                resources.resize(initBlendStateCommand->blendState);
                            resources[initBlendStateCommand->blendState - 1] = std::move(blendStateResourceOGL);
                            break;
                        }

                        case Command::Type::INIT_BUFFER:
                        {
                            const InitBufferCommand* initBufferCommand = static_cast<const InitBufferCommand*>(command.get());

                            std::unique_ptr<BufferResourceOGL> bufferResourceOGL(new BufferResourceOGL(*this,
                                                                                                       initBufferCommand->usage,
                                                                                                       initBufferCommand->flags,
                                                                                                       initBufferCommand->data,
                                                                                                       initBufferCommand->size));

                            if (initBufferCommand->buffer > resources.size())
                                resources.resize(initBufferCommand->buffer);
                            resources[initBufferCommand->buffer - 1] = std::move(bufferResourceOGL);
                            break;
                        }

                        case Command::Type::SET_BUFFER_DATA:
                        {
                            const SetBufferDataCommand* setBufferDataCommand = static_cast<const SetBufferDataCommand*>(command.get());

                            BufferResourceOGL* bufferResourceOGL = static_cast<BufferResourceOGL*>(resources[setBufferDataCommand->buffer - 1].get());
                            bufferResourceOGL->setData(setBufferDataCommand->data);
                            break;
                        }

                        case Command::Type::INIT_SHADER:
                        {
                            const InitShaderCommand* initShaderCommand = static_cast<const InitShaderCommand*>(command.get());

                            std::unique_ptr<ShaderResourceOGL> shaderResourceOGL(new ShaderResourceOGL(*this,
                                                                                                       initShaderCommand->fragmentShader,
                                                                                                       initShaderCommand->vertexShader,
                                                                                                       initShaderCommand->vertexAttributes,
                                                                                                       initShaderCommand->fragmentShaderConstantInfo,
                                                                                                       initShaderCommand->vertexShaderConstantInfo,
                                                                                                       initShaderCommand->fragmentShaderDataAlignment,
                                                                                                       initShaderCommand->vertexShaderDataAlignment,
                                                                                                       initShaderCommand->fragmentShaderFunction,
                                                                                                       initShaderCommand->vertexShaderFunction));

                            if (initShaderCommand->shader > resources.size())
                                resources.resize(initShaderCommand->shader);
                            resources[initShaderCommand->shader - 1] = std::move(shaderResourceOGL);
                            break;
                        }

                        case Command::Type::SET_SHADER_CONSTANTS:
                        {
                            const SetShaderConstantsCommand* setShaderConstantsCommand = static_cast<const SetShaderConstantsCommand*>(command.get());

                            if (!currentShader)
                                throw DataError("No shader set");

                            // pixel shader constants
                            const std::vector<ShaderResourceOGL::Location>& fragmentShaderConstantLocations = currentShader->getFragmentShaderConstantLocations();

                            if (setShaderConstantsCommand->fragmentShaderConstants.size() > fragmentShaderConstantLocations.size())
                                throw DataError("Invalid pixel shader constant size");

                            for (size_t i = 0; i < setShaderConstantsCommand->fragmentShaderConstants.size(); ++i)
                            {
                                const ShaderResourceOGL::Location& fragmentShaderConstantLocation = fragmentShaderConstantLocations[i];
                                const std::vector<float>& fragmentShaderConstant = setShaderConstantsCommand->fragmentShaderConstants[i];

                                setUniform(fragmentShaderConstantLocation.location,
                                           fragmentShaderConstantLocation.dataType,
                                           fragmentShaderConstant.data());
                            }

                            // vertex shader constants
                            const std::vector<ShaderResourceOGL::Location>& vertexShaderConstantLocations = currentShader->getVertexShaderConstantLocations();

                            if (setShaderConstantsCommand->vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                                throw DataError("Invalid vertex shader constant size");

                            for (size_t i = 0; i < setShaderConstantsCommand->vertexShaderConstants.size(); ++i)
                            {
                                const ShaderResourceOGL::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                                const std::vector<float>& vertexShaderConstant = setShaderConstantsCommand->vertexShaderConstants[i];

                                setUniform(vertexShaderConstantLocation.location,
                                           vertexShaderConstantLocation.dataType,
                                           vertexShaderConstant.data());
                            }

                            break;
                        }

                        case Command::Type::INIT_TEXTURE:
                        {
                            const InitTextureCommand* initTextureCommand = static_cast<const InitTextureCommand*>(command.get());

                            std::unique_ptr<TextureResourceOGL> textureResourceOGL(new TextureResourceOGL(*this,
                                                                                                          initTextureCommand->levels,
                                                                                                          initTextureCommand->flags,
                                                                                                          initTextureCommand->sampleCount,
                                                                                                          initTextureCommand->pixelFormat));

                            if (initTextureCommand->texture > resources.size())
                                resources.resize(initTextureCommand->texture);
                            resources[initTextureCommand->texture - 1] = std::move(textureResourceOGL);
                            break;
                        }

                        case Command::Type::SET_TEXTURE_DATA:
                        {
                            const SetTextureDataCommand* setTextureDataCommand = static_cast<const SetTextureDataCommand*>(command.get());

                            TextureResourceOGL* textureResourceOGL = static_cast<TextureResourceOGL*>(resources[setTextureDataCommand->texture - 1].get());
                            textureResourceOGL->setData(setTextureDataCommand->levels);

                            break;
                        }

                        case Command::Type::SET_TEXTURE_PARAMETERS:
                        {
                            const SetTextureParametersCommand* setTextureParametersCommand = static_cast<const SetTextureParametersCommand*>(command.get());

                            TextureResourceOGL* textureResourceOGL = static_cast<TextureResourceOGL*>(resources[setTextureParametersCommand->texture - 1].get());
                            textureResourceOGL->setFilter(setTextureParametersCommand->filter);
                            textureResourceOGL->setAddressX(setTextureParametersCommand->addressX);
                            textureResourceOGL->setAddressY(setTextureParametersCommand->addressY);
                            textureResourceOGL->setMaxAnisotropy(setTextureParametersCommand->maxAnisotropy);
                            break;
                        }

                        case Command::Type::SET_TEXTURES:
                        {
                            const SetTexturesCommand* setTexturesCommand = static_cast<const SetTexturesCommand*>(command.get());

                            for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                            {
                                if (setTexturesCommand->textures[layer])
                                {
                                    TextureResourceOGL* textureOGL = static_cast<TextureResourceOGL*>(resources[setTexturesCommand->textures[layer] - 1].get());
                                    bindTexture(textureOGL->getTextureId(), layer);
                                }
                                else
                                    bindTexture(0, layer);
                            }

                            break;
                        }

                        default:
                            throw DataError("Invalid command");
                    }

                    if (command->type == Command::Type::PRESENT) return;
                }
            }
        }

        void RenderDeviceOGL::present()
        {
        }

        void RenderDeviceOGL::generateScreenshot(const std::string& filename)
        {
            bindFrameBuffer(frameBufferId);

            const GLsizei pixelSize = 4;

            std::vector<uint8_t> data(static_cast<size_t>(frameBufferWidth * frameBufferHeight * pixelSize));

            glReadPixelsProc(0, 0, frameBufferWidth, frameBufferHeight,
                             GL_RGBA, GL_UNSIGNED_BYTE, data.data());

            GLenum error;

            if ((error = glGetErrorProc()) != GL_NO_ERROR)
                throw SystemError("Failed to read pixels from frame buffer, error: " + std::to_string(error));

            uint32_t temp;
            uint32_t* rgba = reinterpret_cast<uint32_t*>(data.data());
            for (GLsizei row = 0; row < frameBufferHeight / 2; ++row)
            {
                for (GLsizei col = 0; col < frameBufferWidth; ++col)
                {
                    temp = rgba[static_cast<size_t>((frameBufferHeight - row - 1) * frameBufferWidth + col)];
                    rgba[static_cast<size_t>((frameBufferHeight - row - 1) * frameBufferWidth + col)] = rgba[static_cast<size_t>(row * frameBufferWidth + col)];
                    rgba[static_cast<size_t>(row * frameBufferWidth + col)] = temp;
                }
            }

            if (!stbi_write_png(filename.c_str(), frameBufferWidth, frameBufferHeight, pixelSize, data.data(), frameBufferWidth * pixelSize))
                throw FileError("Failed to save image to file");
        }
    } // namespace graphics
} // namespace ouzel

#endif
