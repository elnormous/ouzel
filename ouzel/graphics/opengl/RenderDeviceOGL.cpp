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
#include "thread/Lock.hpp"
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

            glBlendFuncSeparateProc = reinterpret_cast<PFNGLBLENDFUNCSEPARATEPROC >(getProcAddress("glBlendFuncSeparate"));
            glBlendEquationSeparateProc = reinterpret_cast<PFNGLBLENDEQUATIONSEPARATEPROC>(getProcAddress("glBlendEquationSeparate"));

#if OUZEL_SUPPORTS_OPENGLES
            glBlendFuncSeparateProc = glBlendFuncSeparate;
            glBlendEquationSeparateProc = glBlendEquationSeparate;

            glUniform1iProc = glUniform1i;
            glUniform1fvProc = glUniform1fv;
            glUniform2fvProc = glUniform2fv;
            glUniform3fvProc = glUniform3fv;
            glUniform4fvProc = glUniform4fv;
            glUniform1ivProc = glUniform1iv;
            glUniform2ivProc = glUniform2iv;
            glUniform3ivProc = glUniform3iv;
            glUniform4ivProc = glUniform4iv;
            glUniformMatrix3fvProc = glUniformMatrix3fv;
            glUniformMatrix4fvProc = glUniformMatrix4fv;

            glActiveTextureProc = glActiveTexture;
            glGenRenderbuffersProc = glGenRenderbuffers;
            glDeleteRenderbuffersProc = glDeleteRenderbuffers;
            glBindRenderbufferProc = glBindRenderbuffer;
            glRenderbufferStorageProc = glRenderbufferStorage;

            glGenFramebuffersProc = glGenFramebuffers;
            glDeleteFramebuffersProc = glDeleteFramebuffers;
            glBindFramebufferProc = glBindFramebuffer;
            glCheckFramebufferStatusProc = glCheckFramebufferStatus;
            glFramebufferRenderbufferProc = glFramebufferRenderbuffer;

#  if OUZEL_OPENGL_INTERFACE_EAGL
            glBlitFramebufferProc = glBlitFramebuffer;
#  endif

            glFramebufferTexture2DProc = glFramebufferTexture2D;

            glClearDepthfProc = glClearDepthf;

            glCreateShaderProc = glCreateShader;
            glDeleteShaderProc = glDeleteShader;
            glAttachShaderProc = glAttachShader;
            glDetachShaderProc = glDetachShader;
            glShaderSourceProc = glShaderSource;
            glCompileShaderProc = glCompileShader;
            glBindAttribLocationProc = glBindAttribLocation;
            glGetShaderivProc = glGetShaderiv;
            glGetShaderInfoLogProc = glGetShaderInfoLog;

            glCreateProgramProc = glCreateProgram;
            glDeleteProgramProc = glDeleteProgram;
            glUseProgramProc = glUseProgram;
            glLinkProgramProc = glLinkProgram;
            glGetProgramivProc = glGetProgramiv;
            glGetProgramInfoLogProc = glGetProgramInfoLog;
            glGetUniformLocationProc = glGetUniformLocation;

            glBindBufferProc = glBindBuffer;
            glDeleteBuffersProc = glDeleteBuffers;
            glGenBuffersProc = glGenBuffers;
            glBufferDataProc = glBufferData;
            glBufferSubDataProc = glBufferSubData;

            glEnableVertexAttribArrayProc = glEnableVertexAttribArray;
            glDisableVertexAttribArrayProc = glDisableVertexAttribArray;
            glVertexAttribPointerProc = glVertexAttribPointer;

#else
            glUniform1iProc = reinterpret_cast<PFNGLUNIFORM1IPROC>(getProcAddress("glUniform1i"));
            glUniform1fvProc = reinterpret_cast<PFNGLUNIFORM1FVPROC>(getProcAddress("glUniform1fv"));
            glUniform2fvProc = reinterpret_cast<PFNGLUNIFORM2FVPROC>(getProcAddress("glUniform2fv"));
            glUniform3fvProc = reinterpret_cast<PFNGLUNIFORM3FVPROC>(getProcAddress("glUniform3fv"));
            glUniform4fvProc = reinterpret_cast<PFNGLUNIFORM4FVPROC>(getProcAddress("glUniform4fv"));
            glUniform1ivProc = reinterpret_cast<PFNGLUNIFORM1IVPROC>(getProcAddress("glUniform1iv"));
            glUniform2ivProc = reinterpret_cast<PFNGLUNIFORM2IVPROC>(getProcAddress("glUniform2iv"));
            glUniform3ivProc = reinterpret_cast<PFNGLUNIFORM3IVPROC>(getProcAddress("glUniform3iv"));
            glUniform4ivProc = reinterpret_cast<PFNGLUNIFORM4IVPROC>(getProcAddress("glUniform4iv"));
            glUniformMatrix3fvProc = reinterpret_cast<PFNGLUNIFORMMATRIX3FVPROC>(getProcAddress("glUniformMatrix3fv"));
            glUniformMatrix4fvProc = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(getProcAddress("glUniformMatrix4fv"));

            glActiveTextureProc = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(getProcAddress("glActiveTexture"));

            glClearDepthProc = reinterpret_cast<PFNGLCLEARDEPTHPROC>(getProcAddress("glClearDepth"));

            glCreateShaderProc = reinterpret_cast<PFNGLCREATESHADERPROC>(getProcAddress("glCreateShader"));
            glDeleteShaderProc = reinterpret_cast<PFNGLDELETESHADERPROC>(getProcAddress("glDeleteShader"));
            glAttachShaderProc = reinterpret_cast<PFNGLATTACHSHADERPROC>(getProcAddress("glAttachShader"));
            glDetachShaderProc = reinterpret_cast<PFNGLDETACHSHADERPROC>(getProcAddress("glDetachShader"));
            glShaderSourceProc = reinterpret_cast<PFNGLSHADERSOURCEPROC>(getProcAddress("glShaderSource"));
            glCompileShaderProc = reinterpret_cast<PFNGLCOMPILESHADERPROC>(getProcAddress("glCompileShader"));
            glBindAttribLocationProc = reinterpret_cast<PFNGLBINDATTRIBLOCATIONPROC>(getProcAddress("glBindAttribLocation"));
            glGetShaderivProc = reinterpret_cast<PFNGLGETSHADERIVPROC>(getProcAddress("glGetShaderiv"));
            glGetShaderInfoLogProc = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(getProcAddress("glGetShaderInfoLog"));

            glCreateProgramProc = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(getProcAddress("glCreateProgram"));
            glDeleteProgramProc = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(getProcAddress("glDeleteProgram"));
            glUseProgramProc = reinterpret_cast<PFNGLUSEPROGRAMPROC>(getProcAddress("glUseProgram"));
            glLinkProgramProc = reinterpret_cast<PFNGLLINKPROGRAMPROC>(getProcAddress("glLinkProgram"));
            glGetProgramivProc = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(getProcAddress("glGetProgramiv"));
            glGetProgramInfoLogProc = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(getProcAddress("glGetProgramInfoLog"));
            glGetUniformLocationProc = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(getProcAddress("glGetUniformLocation"));

            glBindBufferProc = reinterpret_cast<PFNGLBINDBUFFERPROC>(getProcAddress("glBindBuffer"));
            glDeleteBuffersProc = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(getProcAddress("glDeleteBuffers"));
            glGenBuffersProc = reinterpret_cast<PFNGLGENBUFFERSPROC>(getProcAddress("glGenBuffers"));
            glBufferDataProc = reinterpret_cast<PFNGLBUFFERDATAPROC>(getProcAddress("glBufferData"));
            glBufferSubDataProc = reinterpret_cast<PFNGLBUFFERSUBDATAPROC>(getProcAddress("glBufferSubData"));

            glEnableVertexAttribArrayProc = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(getProcAddress("glEnableVertexAttribArray"));
            glDisableVertexAttribArrayProc = reinterpret_cast<PFNGLDISABLEVERTEXATTRIBARRAYPROC>(getProcAddress("glDisableVertexAttribArray"));
            glVertexAttribPointerProc = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(getProcAddress("glVertexAttribPointer"));
#endif

            anisotropicFilteringSupported = false;

            std::vector<std::string> extensions;

            if (apiMajorVersion >= 3)
            {
#if OUZEL_OPENGL_INTERFACE_EAGL
                glGetStringiProc = glGetStringi;
#else
                glGetStringiProc = reinterpret_cast<PFNGLGETSTRINGIPROC>(getProcAddress("glGetStringi"));
#endif

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

#if OUZEL_OPENGL_INTERFACE_EAGL
                glUniform1uivProc = glUniform1uiv;
                glUniform2uivProc = glUniform2uiv;
                glUniform3uivProc = glUniform3uiv;
                glUniform4uivProc = glUniform4uiv;

                glGenVertexArraysProc = glGenVertexArraysOES;
                glBindVertexArrayProc = glBindVertexArrayOES;
                glDeleteVertexArraysProc = glDeleteVertexArraysOES;
                glMapBufferProc = glMapBufferOES;
                glUnmapBufferProc = glUnmapBufferOES;
                glMapBufferRangeProc = glMapBufferRangeEXT;

                glRenderbufferStorageMultisampleProc = glRenderbufferStorageMultisampleAPPLE;
#else
                glUniform1uivProc = reinterpret_cast<PFNGLUNIFORM1UIVPROC>(getProcAddress("glUniform1uiv"));
                glUniform2uivProc = reinterpret_cast<PFNGLUNIFORM2UIVPROC>(getProcAddress("glUniform2uiv"));
                glUniform3uivProc = reinterpret_cast<PFNGLUNIFORM3UIVPROC>(getProcAddress("glUniform3uiv"));
                glUniform4uivProc = reinterpret_cast<PFNGLUNIFORM4UIVPROC>(getProcAddress("glUniform4uiv"));

                glGenVertexArraysProc = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(getProcAddress("glGenVertexArrays"));
                glBindVertexArrayProc = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(getProcAddress("glBindVertexArray"));
                glDeleteVertexArraysProc = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(getProcAddress("glDeleteVertexArrays"));
#  if OUZEL_OPENGL_INTERFACE_EGL
                glMapBufferProc = reinterpret_cast<PFNGLMAPBUFFEROESPROC>(getProcAddress("glMapBuffer"));
#  elif !OUZEL_SUPPORTS_OPENGLES
                glMapBufferProc = reinterpret_cast<PFNGLMAPBUFFERPROC>(getProcAddress("glMapBuffer"));

                glGenFramebuffersProc = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(getProcAddress("glGenFramebuffers"));
                glDeleteFramebuffersProc = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(getProcAddress("glDeleteFramebuffers"));
                glBindFramebufferProc = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(getProcAddress("glBindFramebuffer"));
                glCheckFramebufferStatusProc = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(getProcAddress("glCheckFramebufferStatus"));
                glFramebufferRenderbufferProc = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(getProcAddress("glFramebufferRenderbuffer"));
                glBlitFramebufferProc = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(getProcAddress("glBlitFramebuffer"));
                glFramebufferTexture2DProc = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(getProcAddress("glFramebufferTexture2D"));

                glGenRenderbuffersProc = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(getProcAddress("glGenRenderbuffers"));
                glDeleteRenderbuffersProc = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(getProcAddress("glDeleteRenderbuffers"));
                glBindRenderbufferProc = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(getProcAddress("glBindRenderbuffer"));
                glRenderbufferStorageProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>(getProcAddress("glRenderbufferStorage"));
#  endif
                glUnmapBufferProc = reinterpret_cast<PFNGLUNMAPBUFFERPROC>(getProcAddress("glUnmapBuffer"));
                glMapBufferRangeProc = reinterpret_cast<PFNGLMAPBUFFERRANGEPROC>(getProcAddress("glMapBufferRange"));
                glRenderbufferStorageMultisampleProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>(getProcAddress("glRenderbufferStorageMultisample"));

#  if OUZEL_SUPPORTS_OPENGLES
                glFramebufferTexture2DMultisampleProc = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC>(getProcAddress("glFramebufferTexture2DMultisample"));
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
                        glRenderbufferStorageMultisampleProc = glRenderbufferStorageMultisampleAPPLE;
                    }
#  elif OUZEL_OPENGL_INTERFACE_EGL
                    else if (extension == "GL_OES_vertex_array_object")
                    {
                        glGenVertexArraysProc = reinterpret_cast<PFNGLGENVERTEXARRAYSOESPROC>(getProcAddress("glGenVertexArraysOES"));
                        glBindVertexArrayProc = reinterpret_cast<PFNGLBINDVERTEXARRAYOESPROC>(getProcAddress("glBindVertexArrayOES"));
                        glDeleteVertexArraysProc = reinterpret_cast<PFNGLDELETEVERTEXARRAYSOESPROC>(getProcAddress("glDeleteVertexArraysOES"));
                    }
                    else if (extension == "GL_OES_mapbuffer")
                    {
                        glMapBufferProc = reinterpret_cast<PFNGLMAPBUFFEROESPROC>(getProcAddress("glMapBufferOES"));
                        glUnmapBufferProc = reinterpret_cast<PFNGLUNMAPBUFFEROESPROC>(getProcAddress("glUnmapBufferOES"));
                    }
                    else if (extension == "GL_EXT_map_buffer_range")
                    {
                        glMapBufferRangeProc = reinterpret_cast<PFNGLMAPBUFFERRANGEEXTPROC>(getProcAddress("glMapBufferRangeEXT"));
                    }
                    else if (extension == "GL_IMG_multisampled_render_to_texture")
                    {
                        multisamplingSupported = true;
                        glRenderbufferStorageMultisampleProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC>(getProcAddress("glRenderbufferStorageMultisampleIMG"));
                        glFramebufferTexture2DMultisampleProc = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC>(getProcAddress("glFramebufferTexture2DMultisampleIMG"));
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

                        glGenFramebuffersProc = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(getProcAddress("glGenFramebuffers"));
                        glDeleteFramebuffersProc = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(getProcAddress("glDeleteFramebuffers"));
                        glBindFramebufferProc = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(getProcAddress("glBindFramebuffer"));
                        glCheckFramebufferStatusProc = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(getProcAddress("glCheckFramebufferStatus"));
                        glFramebufferRenderbufferProc = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(getProcAddress("glFramebufferRenderbuffer"));
                        glFramebufferTexture2DProc = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(getProcAddress("glFramebufferTexture2D"));

                        glGenRenderbuffersProc = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(getProcAddress("glGenRenderbuffers"));
                        glDeleteRenderbuffersProc = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(getProcAddress("glDeleteRenderbuffers"));
                        glBindRenderbufferProc = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(getProcAddress("glBindRenderbuffer"));
                        glRenderbufferStorageProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>(getProcAddress("glRenderbufferStorage"));
                    }
                    else if (extension == "GL_EXT_framebuffer_blit")
                    {
                        glBlitFramebufferProc = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(getProcAddress("glBlitFramebuffer"));
                    }
                    else if (extension == "GL_EXT_framebuffer_multisample")
                    {
                        multisamplingSupported = true;
                        glRenderbufferStorageMultisampleProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>(getProcAddress("glRenderbufferStorageMultisample"));
                    }
                    else if (extension == "GL_EXT_map_buffer_range")
                    {
                        glMapBufferRangeProc = reinterpret_cast<PFNGLMAPBUFFERRANGEPROC>(getProcAddress("glMapBufferRangeEXT"));
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

            if (glGenVertexArraysProc) glGenVertexArraysProc(1, &vertexArrayId);
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

        void RenderDeviceOGL::process()
        {
            lockContext();

            RenderDevice::process();

            swapBuffers();
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

        void RenderDeviceOGL::processCommands(CommandBuffer& commands)
        {
            ShaderResourceOGL* currentShader = nullptr;

            if (vertexArrayId)
            {
                glBindVertexArrayProc(vertexArrayId);

                GLenum error;

                if ((error = glGetError()) != GL_NO_ERROR)
                    throw DataError("Failed to bind vertex array, error: " + std::to_string(error));
            }

            while (Command* command = commands.front())
            {
                switch (command->type)
                {
                    case Command::Type::SET_RENDER_TARGET:
                    {
                        const SetRenderTargetCommand* setRenderTargetCommand = static_cast<const SetRenderTargetCommand*>(command);

                        GLuint newFrameBufferId = 0;

                        if (setRenderTargetCommand->renderTarget)
                        {
                            TextureResourceOGL* renderTargetOGL = static_cast<TextureResourceOGL*>(setRenderTargetCommand->renderTarget);

                            if (!renderTargetOGL->getFrameBufferId())
                                continue;

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
                        const ClearCommand* clearCommand = static_cast<const ClearCommand*>(command);

                        GLuint newFrameBufferId = 0;
                        GLbitfield newClearMask = 0;
                        const float* newClearColor;
                        GLfloat newClearDepth;
                        GLsizei renderTargetWidth = 0;
                        GLsizei renderTargetHeight = 0;

                        if (clearCommand->renderTarget)
                        {
                            TextureResourceOGL* renderTargetOGL = static_cast<TextureResourceOGL*>(clearCommand->renderTarget);

                            if (!renderTargetOGL->getFrameBufferId())
                                continue;

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
                        const SetCullModeCommad* setCullModeCommad = static_cast<const SetCullModeCommad*>(command);

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
                        const SetFillModeCommad* setFillModeCommad = static_cast<const SetFillModeCommad*>(command);

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
                        const SetScissorTestCommand* setScissorTestCommand = static_cast<const SetScissorTestCommand*>(command);

                        setScissorTest(setScissorTestCommand->enabled,
                                       static_cast<GLint>(setScissorTestCommand->rectangle.position.x),
                                       static_cast<GLint>(setScissorTestCommand->rectangle.position.y),
                                       static_cast<GLsizei>(setScissorTestCommand->rectangle.size.width),
                                       static_cast<GLsizei>(setScissorTestCommand->rectangle.size.height));

                        break;
                    }

                    case Command::Type::SET_VIEWPORT:
                    {
                        const SetViewportCommand* setViewportCommand = static_cast<const SetViewportCommand*>(command);

                        setViewport(static_cast<GLint>(setViewportCommand->viewport.position.x),
                                    static_cast<GLint>(setViewportCommand->viewport.position.y),
                                    static_cast<GLsizei>(setViewportCommand->viewport.size.width),
                                    static_cast<GLsizei>(setViewportCommand->viewport.size.height));

                        break;
                    }

                    case Command::Type::SET_DEPTH_STATE:
                    {
                        const SetDepthStateCommand* setDepthStateCommand = static_cast<const SetDepthStateCommand*>(command);

                        enableDepthTest(setDepthStateCommand->depthTest);
                        setDepthMask(setDepthStateCommand->depthWrite);

                        break;
                    }

                    case Command::Type::SET_PIPELINE_STATE:
                    {
                        const SetPipelineStateCommand* setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command);

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
                        const DrawCommand* drawCommand = static_cast<const DrawCommand*>(command);

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
                        //const PushDebugMarkerCommand* pushDebugMarkerCommand = static_cast<const PushDebugMarkerCommand*>(command);
                        // TODO: implement
                        // EXT_debug_marker
                        // glPushGroupMarkerEXT
                        break;
                    }

                    case Command::Type::POP_DEBUG_MARKER:
                    {
                        //const PopDebugMarkerCommand* popDebugMarkerCommand = static_cast<const PopDebugMarkerCommand*>(command);
                        // TODO: implement
                        // EXT_debug_marker
                        // glPopGroupMarkerEXT
                        break;
                    }

                    case Command::Type::INIT_BLEND_STATE:
                    {
                        const InitBlendStateCommand* initBlendStateCommand = static_cast<const InitBlendStateCommand*>(command);

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
                        const InitBufferCommand* initBufferCommand = static_cast<const InitBufferCommand*>(command);

                        initBufferCommand->buffer->init(initBufferCommand->usage,
                                                        initBufferCommand->flags,
                                                        initBufferCommand->data,
                                                        initBufferCommand->size);
                        break;
                    }

                    case Command::Type::SET_BUFFER_DATA:
                    {
                        const SetBufferDataCommand* setBufferDataCommand = static_cast<const SetBufferDataCommand*>(command);

                        setBufferDataCommand->buffer->setData(setBufferDataCommand->data);
                        break;
                    }

                    case Command::Type::INIT_SHADER:
                    {
                        const InitShaderCommand* initShaderCommand = static_cast<const InitShaderCommand*>(command);

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
                        const SetShaderConstantsCommand* setShaderConstantsCommand = static_cast<const SetShaderConstantsCommand*>(command);

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

                    case Command::Type::SET_TEXTURES:
                    {
                        const SetTexturesCommand* setTexturesCommand = static_cast<const SetTexturesCommand*>(command);

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

                commands.pop();
            }
        }

        void RenderDeviceOGL::lockContext()
        {
        }

        void RenderDeviceOGL::swapBuffers()
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
            Lock lock(resourceMutex);

            BlendStateResource* blendState = new BlendStateResourceOGL(*this);
            resources.push_back(std::unique_ptr<RenderResource>(blendState));
            return blendState;
        }

        BufferResource* RenderDeviceOGL::createBuffer()
        {
            Lock lock(resourceMutex);

            BufferResource* buffer = new BufferResourceOGL(*this);
            resources.push_back(std::unique_ptr<RenderResource>(buffer));
            return buffer;
        }

        RenderTargetResource* RenderDeviceOGL::createRenderTarget()
        {
            Lock lock(resourceMutex);

            RenderTargetResource* renderTarget = new RenderTargetResourceOGL(*this);
            resources.push_back(std::unique_ptr<RenderResource>(renderTarget));
            return renderTarget;
        }

        ShaderResource* RenderDeviceOGL::createShader()
        {
            Lock lock(resourceMutex);

            ShaderResource* shader = new ShaderResourceOGL(*this);
            resources.push_back(std::unique_ptr<RenderResource>(shader));
            return shader;
        }

        TextureResource* RenderDeviceOGL::createTexture()
        {
            Lock lock(resourceMutex);

            TextureResource* texture = new TextureResourceOGL(*this);
            resources.push_back(std::unique_ptr<RenderResource>(texture));
            return texture;
        }

        void* RenderDeviceOGL::getProcAddress(const std::string& name) const
        {
#if OUZEL_PLATFORM_MACOS
            return dlsym(RTLD_DEFAULT, name.c_str());
#elif OUZEL_OPENGL_INTERFACE_EGL
            return reinterpret_cast<void*>(eglGetProcAddress(name.c_str()));
#elif OUZEL_OPENGL_INTERFACE_GLX
            return reinterpret_cast<void*>(glXGetProcAddress(reinterpret_cast<const GLubyte*>(name.c_str())));
#elif OUZEL_OPENGL_INTERFACE_WGL
            void* result = reinterpret_cast<void*>(wglGetProcAddress(name.c_str()));
            if (result && result != reinterpret_cast<void*>(1) &&
                result != reinterpret_cast<void*>(2) &&
                result != reinterpret_cast<void*>(3) &&
                result != reinterpret_cast<void*>(-1))
                return result;
            else
                return nullptr;
#else
            OUZEL_UNUSED(name);
            return nullptr;
#endif
        }
    } // namespace graphics
} // namespace ouzel

#endif
