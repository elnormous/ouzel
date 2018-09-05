// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if OUZEL_PLATFORM_MACOS
#  include <dlfcn.h>
#endif

#include <cassert>
#include <sstream>

#if OUZEL_SUPPORTS_OPENGLES
#  define GL_GLEXT_PROTOTYPES 1
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  define GL_GLEXT_PROTOTYPES 1
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#if OUZEL_OPENGL_INTERFACE_EGL
#  include "EGL/egl.h"
#elif OUZEL_OPENGL_INTERFACE_GLX
#  define GL_GLEXT_PROTOTYPES 1
#  include <GL/glx.h>
#  include "GL/glxext.h"
#elif OUZEL_OPENGL_INTERFACE_WGL
#  include "GL/wglext.h"
#endif

#include "RenderDeviceOGL.hpp"
#include "BlendStateResourceOGL.hpp"
#include "BufferResourceOGL.hpp"
#include "RenderTargetResourceOGL.hpp"
#include "ShaderResourceOGL.hpp"
#include "TextureResourceOGL.hpp"
#include "core/Window.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"
#include "stb_image_write.h"

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
#else
PFNGLCLEARDEPTHPROC glClearDepthProc;
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

#if OUZEL_SUPPORTS_OPENGLES
PFNGLMAPBUFFEROESPROC glMapBufferProc;
PFNGLUNMAPBUFFEROESPROC glUnmapBufferProc;
PFNGLMAPBUFFERRANGEEXTPROC glMapBufferRangeProc;
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleProc;
#else
PFNGLMAPBUFFERPROC glMapBufferProc;
PFNGLUNMAPBUFFERPROC glUnmapBufferProc;
PFNGLMAPBUFFERRANGEPROC glMapBufferRangeProc;
#endif

#if OUZEL_PLATFORM_MACOS
#  define GET_CORE_PROC_ADDRESS(proc) dlsym(RTLD_DEFAULT, #proc)
#  define GET_EXT_PROC_ADDRESS(proc) dlsym(RTLD_DEFAULT, #proc)
#elif OUZEL_OPENGL_INTERFACE_EGL
#  define GET_CORE_PROC_ADDRESS(proc) proc
#  define GET_EXT_PROC_ADDRESS(proc) eglGetProcAddress(#proc)
#elif OUZEL_OPENGL_INTERFACE_GLX
#  define GET_CORE_PROC_ADDRESS(proc) glXGetProcAddress(reinterpret_cast<const GLubyte*>(#proc))
#  define GET_EXT_PROC_ADDRESS(proc) glXGetProcAddress(reinterpret_cast<const GLubyte*>(#proc))
#elif OUZEL_OPENGL_INTERFACE_WGL
#  define GET_CORE_PROC_ADDRESS(proc) wglGetProcAddress(#proc)
#  define GET_EXT_PROC_ADDRESS(proc) wglGetProcAddress(#proc)
#else
#  define GET_CORE_PROC_ADDRESS(proc) proc
#  define GET_EXT_PROC_ADDRESS(proc) proc
#endif

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

        RenderDeviceOGL::RenderDeviceOGL():
            RenderDevice(Renderer::Driver::OPENGL)
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

            resourceDeleteSet.clear();
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

            const GLubyte* deviceName = glGetString(GL_RENDERER);

            GLenum error;

            if ((error = glGetError()) != GL_NO_ERROR || !deviceName)
                Log(Log::Level::WARN) << "Failed to get OpenGL renderer, error: " + std::to_string(error);
            else
                Log(Log::Level::INFO) << "Using " << reinterpret_cast<const char*>(deviceName) << " for rendering";

            glBlendFuncSeparateProc = reinterpret_cast<PFNGLBLENDFUNCSEPARATEPROC>(GET_CORE_PROC_ADDRESS(glBlendFuncSeparate));
            glBlendEquationSeparateProc = reinterpret_cast<PFNGLBLENDEQUATIONSEPARATEPROC>(GET_CORE_PROC_ADDRESS(glBlendEquationSeparate));

            glUniform1iProc = reinterpret_cast<PFNGLUNIFORM1IPROC>(GET_CORE_PROC_ADDRESS(glUniform1i));
            glUniform1fvProc = reinterpret_cast<PFNGLUNIFORM1FVPROC>(GET_CORE_PROC_ADDRESS(glUniform1fv));
            glUniform2fvProc = reinterpret_cast<PFNGLUNIFORM2FVPROC>(GET_CORE_PROC_ADDRESS(glUniform2fv));
            glUniform3fvProc = reinterpret_cast<PFNGLUNIFORM3FVPROC>(GET_CORE_PROC_ADDRESS(glUniform3fv));
            glUniform4fvProc = reinterpret_cast<PFNGLUNIFORM4FVPROC>(GET_CORE_PROC_ADDRESS(glUniform4fv));
            glUniform1ivProc = reinterpret_cast<PFNGLUNIFORM1IVPROC>(GET_CORE_PROC_ADDRESS(glUniform1iv));
            glUniform2ivProc = reinterpret_cast<PFNGLUNIFORM2IVPROC>(GET_CORE_PROC_ADDRESS(glUniform2iv));
            glUniform3ivProc = reinterpret_cast<PFNGLUNIFORM3IVPROC>(GET_CORE_PROC_ADDRESS(glUniform3iv));
            glUniform4ivProc = reinterpret_cast<PFNGLUNIFORM4IVPROC>(GET_CORE_PROC_ADDRESS(glUniform4iv));
            glUniformMatrix3fvProc = reinterpret_cast<PFNGLUNIFORMMATRIX3FVPROC>(GET_CORE_PROC_ADDRESS(glUniformMatrix3fv));
            glUniformMatrix4fvProc = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(GET_CORE_PROC_ADDRESS(glUniformMatrix4fv));

            glActiveTextureProc = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(GET_CORE_PROC_ADDRESS(glActiveTexture));

#if OUZEL_SUPPORTS_OPENGLES
            glClearDepthfProc = reinterpret_cast<PFNGLCLEARDEPTHFPROC>(GET_CORE_PROC_ADDRESS(glClearDepthf));
#else
            glClearDepthProc = reinterpret_cast<PFNGLCLEARDEPTHPROC>(GET_CORE_PROC_ADDRESS(glClearDepth));
#endif

            glCreateShaderProc = reinterpret_cast<PFNGLCREATESHADERPROC>(GET_CORE_PROC_ADDRESS(glCreateShader));
            glDeleteShaderProc = reinterpret_cast<PFNGLDELETESHADERPROC>(GET_CORE_PROC_ADDRESS(glDeleteShader));
            glAttachShaderProc = reinterpret_cast<PFNGLATTACHSHADERPROC>(GET_CORE_PROC_ADDRESS(glAttachShader));
            glDetachShaderProc = reinterpret_cast<PFNGLDETACHSHADERPROC>(GET_CORE_PROC_ADDRESS(glDetachShader));
            glShaderSourceProc = reinterpret_cast<PFNGLSHADERSOURCEPROC>(GET_CORE_PROC_ADDRESS(glShaderSource));
            glCompileShaderProc = reinterpret_cast<PFNGLCOMPILESHADERPROC>(GET_CORE_PROC_ADDRESS(glCompileShader));
            glBindAttribLocationProc = reinterpret_cast<PFNGLBINDATTRIBLOCATIONPROC>(GET_CORE_PROC_ADDRESS(glBindAttribLocation));
            glGetShaderivProc = reinterpret_cast<PFNGLGETSHADERIVPROC>(GET_CORE_PROC_ADDRESS(glGetShaderiv));
            glGetShaderInfoLogProc = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(GET_CORE_PROC_ADDRESS(glGetShaderInfoLog));

            glCreateProgramProc = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(GET_CORE_PROC_ADDRESS(glCreateProgram));
            glDeleteProgramProc = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(GET_CORE_PROC_ADDRESS(glDeleteProgram));
            glUseProgramProc = reinterpret_cast<PFNGLUSEPROGRAMPROC>(GET_CORE_PROC_ADDRESS(glUseProgram));
            glLinkProgramProc = reinterpret_cast<PFNGLLINKPROGRAMPROC>(GET_CORE_PROC_ADDRESS(glLinkProgram));
            glGetProgramivProc = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(GET_CORE_PROC_ADDRESS(glGetProgramiv));
            glGetProgramInfoLogProc = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(GET_CORE_PROC_ADDRESS(glGetProgramInfoLog));
            glGetUniformLocationProc = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(GET_CORE_PROC_ADDRESS(glGetUniformLocation));

            glBindBufferProc = reinterpret_cast<PFNGLBINDBUFFERPROC>(GET_CORE_PROC_ADDRESS(glBindBuffer));
            glDeleteBuffersProc = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(GET_CORE_PROC_ADDRESS(glDeleteBuffers));
            glGenBuffersProc = reinterpret_cast<PFNGLGENBUFFERSPROC>(GET_CORE_PROC_ADDRESS(glGenBuffers));
            glBufferDataProc = reinterpret_cast<PFNGLBUFFERDATAPROC>(GET_CORE_PROC_ADDRESS(glBufferData));
            glBufferSubDataProc = reinterpret_cast<PFNGLBUFFERSUBDATAPROC>(GET_CORE_PROC_ADDRESS(glBufferSubData));

            glEnableVertexAttribArrayProc = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(GET_CORE_PROC_ADDRESS(glEnableVertexAttribArray));
            glDisableVertexAttribArrayProc = reinterpret_cast<PFNGLDISABLEVERTEXATTRIBARRAYPROC>(GET_CORE_PROC_ADDRESS(glDisableVertexAttribArray));
            glVertexAttribPointerProc = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(GET_CORE_PROC_ADDRESS(glVertexAttribPointer));

#if OUZEL_SUPPORTS_OPENGLES
            glGenFramebuffersProc = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(GET_CORE_PROC_ADDRESS(glGenFramebuffers));
            glDeleteFramebuffersProc = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(GET_CORE_PROC_ADDRESS(glDeleteFramebuffers));
            glBindFramebufferProc = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(GET_CORE_PROC_ADDRESS(glBindFramebuffer));
            glCheckFramebufferStatusProc = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(GET_CORE_PROC_ADDRESS(glCheckFramebufferStatus));
            glFramebufferRenderbufferProc = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(GET_CORE_PROC_ADDRESS(glFramebufferRenderbuffer));
            glFramebufferTexture2DProc = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(GET_CORE_PROC_ADDRESS(glFramebufferTexture2D));

            glGenRenderbuffersProc = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(GET_CORE_PROC_ADDRESS(glGenRenderbuffers));
            glDeleteRenderbuffersProc = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(GET_CORE_PROC_ADDRESS(glDeleteRenderbuffers));
            glBindRenderbufferProc = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(GET_CORE_PROC_ADDRESS(glBindRenderbuffer));
            glRenderbufferStorageProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>(GET_CORE_PROC_ADDRESS(glRenderbufferStorage));

#  if OUZEL_OPENGL_INTERFACE_EAGL
            glBlitFramebufferProc = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(GET_CORE_PROC_ADDRESS(glBlitFramebuffer));
#  endif
#endif

            anisotropicFilteringSupported = false;

            std::vector<std::string> extensions;

            if (apiMajorVersion >= 3)
            {
                glGetStringiProc = reinterpret_cast<PFNGLGETSTRINGIPROC>(GET_EXT_PROC_ADDRESS(glGetStringi));

                GLint extensionCount;
                glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

                if ((error = glGetError()) != GL_NO_ERROR)
                    Log(Log::Level::WARN) << "Failed to get OpenGL extension count, error: " + std::to_string(error);
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
                const GLubyte* extensionPtr = glGetString(GL_EXTENSIONS);

                if ((error = glGetError()) != GL_NO_ERROR || !extensionPtr)
                    Log(Log::Level::WARN) << "Failed to get OpenGL extensions";
                else
                {
                    std::istringstream extensionStringStream(reinterpret_cast<const char*>(extensionPtr));

                    for (std::string extension; extensionStringStream >> extension;)
                        extensions.push_back(extension);
                }
            }

            {
                Log extensionLog(Log::Level::ALL);

                extensionLog << "Supported OpenGL extensions: ";
                bool first = true;

                for (const std::string& extension : extensions)
                {
                    if (!first) extensionLog << ", ";
                    first = false;
                    extensionLog << extension;
                }
            }

            if (apiMajorVersion >= 3)
            {
                for (const std::string& extension : extensions)
                {
                    if (extension == "GL_EXT_texture_filter_anisotropic")
                        anisotropicFilteringSupported = true;
                }

                glUniform1uivProc = reinterpret_cast<PFNGLUNIFORM1UIVPROC>(GET_EXT_PROC_ADDRESS(glUniform1uiv));
                glUniform2uivProc = reinterpret_cast<PFNGLUNIFORM2UIVPROC>(GET_EXT_PROC_ADDRESS(glUniform2uiv));
                glUniform3uivProc = reinterpret_cast<PFNGLUNIFORM3UIVPROC>(GET_EXT_PROC_ADDRESS(glUniform3uiv));
                glUniform4uivProc = reinterpret_cast<PFNGLUNIFORM4UIVPROC>(GET_EXT_PROC_ADDRESS(glUniform4uiv));

#if OUZEL_OPENGL_INTERFACE_EAGL
                glGenVertexArraysProc = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(GET_EXT_PROC_ADDRESS(glGenVertexArraysOES));
                glBindVertexArrayProc = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(GET_EXT_PROC_ADDRESS(glBindVertexArrayOES));
                glDeleteVertexArraysProc = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(GET_EXT_PROC_ADDRESS(glDeleteVertexArraysOES));

                glMapBufferProc = reinterpret_cast<PFNGLMAPBUFFEROESPROC>(GET_EXT_PROC_ADDRESS(glMapBufferOES));
                glUnmapBufferProc = reinterpret_cast<PFNGLUNMAPBUFFEROESPROC>(GET_EXT_PROC_ADDRESS(glUnmapBufferOES));
                glMapBufferRangeProc = reinterpret_cast<PFNGLMAPBUFFERRANGEPROC>(GET_EXT_PROC_ADDRESS(glMapBufferRangeEXT));

                glRenderbufferStorageMultisampleProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>(GET_EXT_PROC_ADDRESS(glRenderbufferStorageMultisampleAPPLE));
#else
                glGenVertexArraysProc = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(GET_EXT_PROC_ADDRESS(glGenVertexArrays));
                glBindVertexArrayProc = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(GET_EXT_PROC_ADDRESS(glBindVertexArray));
                glDeleteVertexArraysProc = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(GET_EXT_PROC_ADDRESS(glDeleteVertexArrays));
#  if OUZEL_OPENGL_INTERFACE_EGL
                glMapBufferProc = reinterpret_cast<PFNGLMAPBUFFEROESPROC>(GET_EXT_PROC_ADDRESS(glMapBuffer));
                glUnmapBufferProc = reinterpret_cast<PFNGLUNMAPBUFFEROESPROC>(GET_EXT_PROC_ADDRESS(glUnmapBuffer));
#  else
                glMapBufferProc = reinterpret_cast<PFNGLMAPBUFFERPROC>(GET_EXT_PROC_ADDRESS(glMapBuffer));
                glUnmapBufferProc = reinterpret_cast<PFNGLUNMAPBUFFERPROC>(GET_EXT_PROC_ADDRESS(glUnmapBuffer));

                glGenFramebuffersProc = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(GET_EXT_PROC_ADDRESS(glGenFramebuffers));
                glDeleteFramebuffersProc = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(GET_EXT_PROC_ADDRESS(glDeleteFramebuffers));
                glBindFramebufferProc = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(GET_EXT_PROC_ADDRESS(glBindFramebuffer));
                glCheckFramebufferStatusProc = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(GET_EXT_PROC_ADDRESS(glCheckFramebufferStatus));
                glFramebufferRenderbufferProc = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(GET_EXT_PROC_ADDRESS(glFramebufferRenderbuffer));
                glBlitFramebufferProc = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(GET_EXT_PROC_ADDRESS(glBlitFramebuffer));
                glFramebufferTexture2DProc = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(GET_EXT_PROC_ADDRESS(glFramebufferTexture2D));

                glGenRenderbuffersProc = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(GET_EXT_PROC_ADDRESS(glGenRenderbuffers));
                glDeleteRenderbuffersProc = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(GET_EXT_PROC_ADDRESS(glDeleteRenderbuffers));
                glBindRenderbufferProc = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(GET_EXT_PROC_ADDRESS(glBindRenderbuffer));
                glRenderbufferStorageProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>(GET_EXT_PROC_ADDRESS(glRenderbufferStorage));
#  endif
                glMapBufferRangeProc = reinterpret_cast<PFNGLMAPBUFFERRANGEPROC>(GET_EXT_PROC_ADDRESS(glMapBufferRange));
                glRenderbufferStorageMultisampleProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>(GET_EXT_PROC_ADDRESS(glRenderbufferStorageMultisample));

#  if OUZEL_SUPPORTS_OPENGLES
                glFramebufferTexture2DMultisampleProc = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC>(GET_EXT_PROC_ADDRESS(glFramebufferTexture2DMultisample));
#  endif
#endif
            }
            else
            {
                npotTexturesSupported = false;
                multisamplingSupported = false;

#if OUZEL_SUPPORTS_OPENGLES
                textureBaseLevelSupported = false;
                textureMaxLevelSupported = false;
#else
                renderTargetsSupported = false;
#endif

                for (const std::string& extension : extensions)
                {
                    if (extension == "GL_EXT_texture_filter_anisotropic")
                    {
                        anisotropicFilteringSupported = true;
                    }
                    else if (extension == "GL_OES_texture_npot" ||
                             extension == "GL_ARB_texture_non_power_of_two")
                    {
                        npotTexturesSupported = true;
                    }
#if OUZEL_SUPPORTS_OPENGLES
#  if OUZEL_OPENGL_INTERFACE_EAGL
                    else if (extension == "GL_APPLE_framebuffer_multisample")
                    {
                        multisamplingSupported = true;
                        glRenderbufferStorageMultisampleProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>(GET_EXT_PROC_ADDRESS(glRenderbufferStorageMultisampleAPPLE));
                    }
#  elif OUZEL_OPENGL_INTERFACE_EGL
                    else if (extension == "GL_OES_vertex_array_object")
                    {
                        glGenVertexArraysProc = reinterpret_cast<PFNGLGENVERTEXARRAYSOESPROC>(GET_EXT_PROC_ADDRESS(glGenVertexArraysOES));
                        glBindVertexArrayProc = reinterpret_cast<PFNGLBINDVERTEXARRAYOESPROC>(GET_EXT_PROC_ADDRESS(glBindVertexArrayOES));
                        glDeleteVertexArraysProc = reinterpret_cast<PFNGLDELETEVERTEXARRAYSOESPROC>(GET_EXT_PROC_ADDRESS(glDeleteVertexArraysOES));
                    }
                    else if (extension == "GL_OES_mapbuffer")
                    {
                        glMapBufferProc = reinterpret_cast<PFNGLMAPBUFFEROESPROC>(GET_EXT_PROC_ADDRESS(glMapBufferOES));
                        glUnmapBufferProc = reinterpret_cast<PFNGLUNMAPBUFFEROESPROC>(GET_EXT_PROC_ADDRESS(glUnmapBufferOES));
                    }
                    else if (extension == "GL_EXT_map_buffer_range")
                    {
                        glMapBufferRangeProc = reinterpret_cast<PFNGLMAPBUFFERRANGEEXTPROC>(GET_EXT_PROC_ADDRESS(glMapBufferRangeEXT));
                    }
                    else if (extension == "GL_IMG_multisampled_render_to_texture")
                    {
                        multisamplingSupported = true;
                        glRenderbufferStorageMultisampleProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC>(GET_EXT_PROC_ADDRESS(glRenderbufferStorageMultisampleIMG));
                        glFramebufferTexture2DMultisampleProc = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC>(GET_EXT_PROC_ADDRESS(glFramebufferTexture2DMultisampleIMG));
                    }
                    else if (extension == "GL_APPLE_texture_max_level")
                    {
                        textureMaxLevelSupported = true;
                    }
#  endif
#else
                    else if (extension == "GL_EXT_framebuffer_object")
                    {
                        renderTargetsSupported = true;

                        glGenFramebuffersProc = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(GET_EXT_PROC_ADDRESS(glGenFramebuffers));
                        glDeleteFramebuffersProc = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(GET_EXT_PROC_ADDRESS(glDeleteFramebuffers));
                        glBindFramebufferProc = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(GET_EXT_PROC_ADDRESS(glBindFramebuffer));
                        glCheckFramebufferStatusProc = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(GET_EXT_PROC_ADDRESS(glCheckFramebufferStatus));
                        glFramebufferRenderbufferProc = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(GET_EXT_PROC_ADDRESS(glFramebufferRenderbuffer));
                        glFramebufferTexture2DProc = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(GET_EXT_PROC_ADDRESS(glFramebufferTexture2D));

                        glGenRenderbuffersProc = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(GET_EXT_PROC_ADDRESS(glGenRenderbuffers));
                        glDeleteRenderbuffersProc = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(GET_EXT_PROC_ADDRESS(glDeleteRenderbuffers));
                        glBindRenderbufferProc = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(GET_EXT_PROC_ADDRESS(glBindRenderbuffer));
                        glRenderbufferStorageProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>(GET_EXT_PROC_ADDRESS(glRenderbufferStorage));
                    }
                    else if (extension == "GL_EXT_framebuffer_blit")
                    {
                        glBlitFramebufferProc = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(GET_EXT_PROC_ADDRESS(glBlitFramebuffer));
                    }
                    else if (extension == "GL_EXT_framebuffer_multisample")
                    {
                        multisamplingSupported = true;
                        glRenderbufferStorageMultisampleProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>(GET_EXT_PROC_ADDRESS(glRenderbufferStorageMultisample));
                    }
                    else if (extension == "GL_EXT_map_buffer_range")
                    {
                        glMapBufferRangeProc = reinterpret_cast<PFNGLMAPBUFFERRANGEPROC>(GET_EXT_PROC_ADDRESS(glMapBufferRangeEXT));
                    }
#endif
                }

                if (!multisamplingSupported) sampleCount = 1;
            }

            glDisable(GL_DITHER);
            glDepthFunc(GL_LEQUAL);

            if ((error = glGetError()) != GL_NO_ERROR)
                throw SystemError("Failed to set depth function, error: " + std::to_string(error));

#if !OUZEL_SUPPORTS_OPENGLES
            if (sampleCount > 1)
            {
                glEnable(GL_MULTISAMPLE);

                if ((error = glGetError()) != GL_NO_ERROR)
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

                if ((error = glGetError()) != GL_NO_ERROR)
                    throw DataError("Failed to bind vertex array, error: " + std::to_string(error));
            }
        }

        void RenderDeviceOGL::setClearColorBuffer(bool clear)
        {
            RenderDevice::setClearColorBuffer(clear);

            if (clearColorBuffer)
                clearMask |= GL_COLOR_BUFFER_BIT;
            else
                clearMask &= ~static_cast<GLbitfield>(GL_COLOR_BUFFER_BIT);

        }

        void RenderDeviceOGL::setClearDepthBuffer(bool clear)
        {
            RenderDevice::setClearDepthBuffer(clear);

            if (clearDepthBuffer)
                clearMask |= GL_DEPTH_BUFFER_BIT;
            else
                clearMask &= ~static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT);
        }

        void RenderDeviceOGL::setClearColor(Color color)
        {
            RenderDevice::setClearColor(color);

            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();
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

            std::unique_ptr<Command> command;

            for (;;)
            {
                {
                    std::unique_lock<std::mutex> lock(commandQueueMutex);
                    while (!queueFinished && commandQueue.empty()) commandQueueCondition.wait(lock);
                    if (!commandQueue.empty())
                    {
                        command = std::move(commandQueue.front());
                        commandQueue.pop();
                    }
                    else if (queueFinished) break;
                }

                switch (command->type)
                {
                    case Command::Type::SET_RENDER_TARGET:
                    {
                        const SetRenderTargetCommand* setRenderTargetCommand = static_cast<const SetRenderTargetCommand*>(command.get());

                        GLuint newFrameBufferId = 0;

                        if (setRenderTargetCommand->renderTarget)
                        {
                            TextureResourceOGL* renderTargetOGL = static_cast<TextureResourceOGL*>(setRenderTargetCommand->renderTarget);

                            if (!renderTargetOGL->getFrameBufferId()) break;

                            newFrameBufferId = renderTargetOGL->getFrameBufferId();
                        }
                        else
                            newFrameBufferId = frameBufferId;

                        bindFrameBuffer(newFrameBufferId);

                        // TODO: update cull mode

                        break;
                    }

                    case Command::Type::CLEAR:
                    {
                        const ClearCommand* clearCommand = static_cast<const ClearCommand*>(command.get());

                        GLuint newFrameBufferId = 0;
                        GLbitfield newClearMask = 0;
                        const float* newClearColor;
                        GLfloat newClearDepth;
                        GLsizei renderTargetWidth = 0;
                        GLsizei renderTargetHeight = 0;

                        if (clearCommand->renderTarget)
                        {
                            TextureResourceOGL* renderTargetOGL = static_cast<TextureResourceOGL*>(clearCommand->renderTarget);

                            if (!renderTargetOGL->getFrameBufferId()) break;

                            renderTargetWidth = renderTargetOGL->getWidth();
                            renderTargetHeight = renderTargetOGL->getHeight();
                            newFrameBufferId = renderTargetOGL->getFrameBufferId();
                            newClearColor = renderTargetOGL->getFrameBufferClearColor();
                            newClearDepth = renderTargetOGL->getClearDepth();
                            newClearMask = renderTargetOGL->getClearMask();
                        }
                        else
                        {
                            renderTargetWidth = frameBufferWidth;
                            renderTargetHeight = frameBufferHeight;
                            newFrameBufferId = frameBufferId;
                            newClearColor = frameBufferClearColor;
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
                                setClearColorValue(newClearColor);

                            glClear(newClearMask);

                            GLenum error;

                            if ((error = glGetError()) != GL_NO_ERROR)
                                throw DataError("Failed to clear frame buffer, error: " + std::to_string(error));
                        }

                        break;
                    }

                    case Command::Type::SET_CULL_MODE:
                    {
                        const SetCullModeCommad* setCullModeCommad = static_cast<const SetCullModeCommad*>(command.get());

                        GLenum cullFace = GL_NONE;

                        switch (setCullModeCommad->cullMode)
                        {
                            case Renderer::CullMode::NONE: cullFace = GL_NONE; break;
                            case Renderer::CullMode::FRONT: cullFace = ((stateCache.frameBufferId != frameBufferId) ? GL_FRONT : GL_BACK); break; // flip the faces, because of the flipped y-axis
                            case Renderer::CullMode::BACK: cullFace = ((stateCache.frameBufferId != frameBufferId) ? GL_BACK : GL_FRONT); break;
                            default: throw DataError("Invalid cull mode");
                        }

                        setCullFace(cullFace != GL_NONE, cullFace);

                        break;
                    }

                    case Command::Type::SET_FILL_MODE:
                    {
                        const SetFillModeCommad* setFillModeCommad = static_cast<const SetFillModeCommad*>(command.get());

    #if OUZEL_SUPPORTS_OPENGLES
                        if (setFillModeCommad->fillMode != Renderer::FillMode::SOLID)
                            throw DataError("Unsupported fill mode");
    #else
                        GLenum fillMode = GL_NONE;

                        switch (setFillModeCommad->fillMode)
                        {
                            case Renderer::FillMode::SOLID: fillMode = GL_FILL; break;
                            case Renderer::FillMode::WIREFRAME: fillMode = GL_LINE; break;
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

                    case Command::Type::SET_DEPTH_STATE:
                    {
                        const SetDepthStateCommand* setDepthStateCommand = static_cast<const SetDepthStateCommand*>(command.get());

                        enableDepthTest(setDepthStateCommand->depthTest);
                        setDepthMask(setDepthStateCommand->depthWrite);

                        break;
                    }

                    case Command::Type::SET_PIPELINE_STATE:
                    {
                        const SetPipelineStateCommand* setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command.get());

                        BlendStateResourceOGL* blendStateOGL = static_cast<BlendStateResourceOGL*>(setPipelineStateCommand->blendState);
                        ShaderResourceOGL* shaderOGL = static_cast<ShaderResourceOGL*>(setPipelineStateCommand->shader);
                        currentShader = shaderOGL;

                        if (blendStateOGL)
                        {
                            setBlendState(blendStateOGL->isGLBlendEnabled(),
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
                        BufferResourceOGL* indexBufferOGL = static_cast<BufferResourceOGL*>(drawCommand->indexBuffer);
                        BufferResourceOGL* vertexBufferOGL = static_cast<BufferResourceOGL*>(drawCommand->vertexBuffer);

                        assert(indexBufferOGL);
                        assert(indexBufferOGL->getBufferId());
                        assert(vertexBufferOGL);
                        assert(vertexBufferOGL->getBufferId());

                        // draw
                        GLenum mode;

                        switch (drawCommand->drawMode)
                        {
                            case Renderer::DrawMode::POINT_LIST: mode = GL_POINTS; break;
                            case Renderer::DrawMode::LINE_LIST: mode = GL_LINES; break;
                            case Renderer::DrawMode::LINE_STRIP: mode = GL_LINE_STRIP; break;
                            case Renderer::DrawMode::TRIANGLE_LIST: mode = GL_TRIANGLES; break;
                            case Renderer::DrawMode::TRIANGLE_STRIP: mode = GL_TRIANGLE_STRIP; break;
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

                        if ((error = glGetError()) != GL_NO_ERROR)
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

                        glDrawElements(mode,
                                       static_cast<GLsizei>(drawCommand->indexCount),
                                       indexType,
                                       static_cast<const char*>(nullptr) + (drawCommand->startIndex * drawCommand->indexSize));

                        if ((error = glGetError()) != GL_NO_ERROR)
                            throw DataError("Failed to draw elements, error: " + std::to_string(error));

                        break;
                    }

                    case Command::Type::PUSH_DEBUG_MARKER:
                    {
                        //const PushDebugMarkerCommand* pushDebugMarkerCommand = static_cast<const PushDebugMarkerCommand*>(command.get());
                        // TODO: implement
                        // EXT_debug_marker
                        // glPushGroupMarkerEXT
                        break;
                    }

                    case Command::Type::POP_DEBUG_MARKER:
                    {
                        //const PopDebugMarkerCommand* popDebugMarkerCommand = static_cast<const PopDebugMarkerCommand*>(command.get());
                        // TODO: implement
                        // EXT_debug_marker
                        // glPopGroupMarkerEXT
                        break;
                    }

                    case Command::Type::INIT_BLEND_STATE:
                    {
                        const InitBlendStateCommand* initBlendStateCommand = static_cast<const InitBlendStateCommand*>(command.get());

                        initBlendStateCommand->blendState->init(initBlendStateCommand->enableBlending,
                                                                initBlendStateCommand->colorBlendSource,
                                                                initBlendStateCommand->colorBlendDest,
                                                                initBlendStateCommand->colorOperation,
                                                                initBlendStateCommand->alphaBlendSource,
                                                                initBlendStateCommand->alphaBlendDest,
                                                                initBlendStateCommand->alphaOperation,
                                                                initBlendStateCommand->colorMask);
                        break;
                    }

                    case Command::Type::INIT_BUFFER:
                    {
                        const InitBufferCommand* initBufferCommand = static_cast<const InitBufferCommand*>(command.get());

                        initBufferCommand->buffer->init(initBufferCommand->usage,
                                                        initBufferCommand->flags,
                                                        initBufferCommand->data,
                                                        initBufferCommand->size);
                        break;
                    }

                    case Command::Type::SET_BUFFER_DATA:
                    {
                        const SetBufferDataCommand* setBufferDataCommand = static_cast<const SetBufferDataCommand*>(command.get());

                        setBufferDataCommand->buffer->setData(setBufferDataCommand->data);
                        break;
                    }

                    case Command::Type::INIT_SHADER:
                    {
                        const InitShaderCommand* initShaderCommand = static_cast<const InitShaderCommand*>(command.get());

                        initShaderCommand->shader->init(initShaderCommand->fragmentShader,
                                                        initShaderCommand->vertexShader,
                                                        initShaderCommand->vertexAttributes,
                                                        initShaderCommand->fragmentShaderConstantInfo,
                                                        initShaderCommand->vertexShaderConstantInfo,
                                                        initShaderCommand->fragmentShaderDataAlignment,
                                                        initShaderCommand->vertexShaderDataAlignment,
                                                        initShaderCommand->fragmentShaderFunction,
                                                        initShaderCommand->vertexShaderFunction);

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

                        initTextureCommand->texture->init(initTextureCommand->levels,
                                                          initTextureCommand->flags,
                                                          initTextureCommand->sampleCount,
                                                          initTextureCommand->pixelFormat);

                        break;
                    }

                    case Command::Type::SET_TEXTURE_DATA:
                    {
                        const SetTextureDataCommand* setTextureDataCommand = static_cast<const SetTextureDataCommand*>(command.get());

                        setTextureDataCommand->texture->setData(setTextureDataCommand->levels);

                        break;
                    }

                    case Command::Type::SET_TEXTURE_PARAMETERS:
                    {
                        const SetTextureParametersCommand* setTextureParametersCommand = static_cast<const SetTextureParametersCommand*>(command.get());
                        break;
                    }

                    case Command::Type::SET_TEXTURES:
                    {
                        const SetTexturesCommand* setTexturesCommand = static_cast<const SetTexturesCommand*>(command.get());

                        for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                        {
                            TextureResourceOGL* textureOGL = static_cast<TextureResourceOGL*>(setTexturesCommand->textures[layer]);

                            if (textureOGL)
                                bindTexture(textureOGL->getTextureId(), layer);
                            else
                                bindTexture(0, layer);
                        }

                        break;
                    }

                    default:
                        throw DataError("Invalid command");
                }
            }

            present();
        }

        void RenderDeviceOGL::present()
        {
        }

        void RenderDeviceOGL::generateScreenshot(const std::string& filename)
        {
            bindFrameBuffer(frameBufferId);

            const GLsizei pixelSize = 4;

            std::vector<uint8_t> data(static_cast<size_t>(frameBufferWidth * frameBufferHeight * pixelSize));

            glReadPixels(0, 0, frameBufferWidth, frameBufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

            GLenum error;

            if ((error = glGetError()) != GL_NO_ERROR)
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

        BlendStateResource* RenderDeviceOGL::createBlendState()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            BlendStateResource* blendState = new BlendStateResourceOGL(*this);
            resources.push_back(std::unique_ptr<RenderResource>(blendState));
            return blendState;
        }

        BufferResource* RenderDeviceOGL::createBuffer()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            BufferResource* buffer = new BufferResourceOGL(*this);
            resources.push_back(std::unique_ptr<RenderResource>(buffer));
            return buffer;
        }

        RenderTargetResource* RenderDeviceOGL::createRenderTarget()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            RenderTargetResource* renderTarget = new RenderTargetResourceOGL(*this);
            resources.push_back(std::unique_ptr<RenderResource>(renderTarget));
            return renderTarget;
        }

        ShaderResource* RenderDeviceOGL::createShader()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            ShaderResource* shader = new ShaderResourceOGL(*this);
            resources.push_back(std::unique_ptr<RenderResource>(shader));
            return shader;
        }

        TextureResource* RenderDeviceOGL::createTexture()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            TextureResource* texture = new TextureResourceOGL(*this);
            resources.push_back(std::unique_ptr<RenderResource>(texture));
            return texture;
        }
    } // namespace graphics
} // namespace ouzel

#endif
