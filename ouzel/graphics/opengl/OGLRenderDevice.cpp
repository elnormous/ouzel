// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if !defined(_WIN32)
#  include <dlfcn.h>
#endif

#include <cassert>

#include "graphics/opengl/OGL.h"

#if OUZEL_OPENGLES
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

#include "OGLRenderDevice.hpp"
#include "OGLBlendState.hpp"
#include "OGLBuffer.hpp"
#include "OGLDepthStencilState.hpp"
#include "OGLRenderTarget.hpp"
#include "OGLShader.hpp"
#include "OGLTexture.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"
#include "stb_image_write.h"

template <typename T>
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

template <typename T>
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

static constexpr bool isVersionGreaterOrEqual(uint16_t majorVersion, uint16_t minorVersion,
                                              uint16_t checkMajorVersion, uint16_t checkMinorVersion)
{
    return (majorVersion == checkMajorVersion) ?
        (minorVersion >= checkMinorVersion) :
        (majorVersion > checkMajorVersion);
}

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            const ErrorCategory errorCategory {};
            std::error_code makeErrorCode(GLenum e)
            {
                return std::error_code(static_cast<int>(e), errorCategory);
            }

            static GLenum getIndexType(uint32_t indexSize)
            {
                switch (indexSize)
                {
                    case 1: return GL_UNSIGNED_BYTE;
                    case 2: return GL_UNSIGNED_SHORT;
                    case 4: return GL_UNSIGNED_INT; // Supported on OpenGL and OpenGL ES 3 and above
                    default: throw std::runtime_error("Invalid index size");
                }
            }

            static GLenum getVertexType(DataType dataType)
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

            static GLenum getDrawMode(DrawMode drawMode)
            {
                switch (drawMode)
                {
                    case DrawMode::POINT_LIST: return GL_POINTS;
                    case DrawMode::LINE_LIST: return GL_LINES;
                    case DrawMode::LINE_STRIP: return GL_LINE_STRIP;
                    case DrawMode::TRIANGLE_LIST: return GL_TRIANGLES;
                    case DrawMode::TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
                    default: throw std::runtime_error("Invalid draw mode");
                }
            }

            static GLenum getCullFace(CullMode cullMode)
            {
                switch (cullMode)
                {
                    case CullMode::NONE: return GL_NONE;
                    case CullMode::FRONT: return GL_FRONT;
                    case CullMode::BACK: return GL_BACK;
                    default: throw std::runtime_error("Invalid cull mode");
                }
            }

#if !OUZEL_OPENGLES
            static GLenum getFillMode(FillMode fillMode)
            {
                switch (fillMode)
                {
                    case FillMode::SOLID: return GL_FILL;
                    case FillMode::WIREFRAME: return GL_LINE;
                    default: throw std::runtime_error("Invalid fill mode");
                }
            }
#endif

            RenderDevice::RenderDevice(const std::function<void(const Event&)>& initCallback):
                graphics::RenderDevice(Driver::OPENGL, initCallback),
                textureBaseLevelSupported(false),
                textureMaxLevelSupported(false),
                uintElementIndexSupported(false)
            {
                projectionTransform = Matrix4F(1.0F, 0.0F, 0.0F, 0.0F,
                                               0.0F, 1.0F, 0.0F, 0.0F,
                                               0.0F, 0.0F, 2.0F, 0.0F,
                                               0.0F, 0.0F, -1.0F, 1.0F);

                renderTargetProjectionTransform = Matrix4F(1.0F, 0.0F, 0.0F, 0.0F,
                                                           0.0F, -1.0F, 0.0F, 0.0F,
                                                           0.0F, 0.0F, 2.0F, 0.0F,
                                                           0.0F, 0.0F, -1.0F, 1.0F);
            }

            RenderDevice::~RenderDevice()
            {
                if (vertexArrayId) glDeleteVertexArraysProc(1, &vertexArrayId);

                resources.clear();
            }

            void RenderDevice::init(Window* newWindow,
                                    const Size2U& newSize,
                                    uint32_t newSampleCount,
                                    graphics::Filter newTextureFilter,
                                    uint32_t newMaxAnisotropy,
                                    bool newSrgb,
                                    bool newVerticalSync,
                                    bool newDepth,
                                    bool newStencil,
                                    bool newDebugRenderer)
            {
                graphics::RenderDevice::init(newWindow,
                                             newSize,
                                             newSampleCount,
                                             newTextureFilter,
                                             newMaxAnisotropy,
                                             newSrgb,
                                             newVerticalSync,
                                             newDepth,
                                             newStencil,
                                             newDebugRenderer);

                frameBufferWidth = static_cast<GLsizei>(newSize.v[0]);
                frameBufferHeight = static_cast<GLsizei>(newSize.v[1]);

                glGetStringProc = getCoreProcAddress<PFNGLGETSTRINGPROC>("glGetString");
                glGetIntegervProc = getCoreProcAddress<PFNGLGETINTEGERVPROC>("glGetIntegerv");
                glGetErrorProc = getCoreProcAddress<PFNGLGETERRORPROC>("glGetError");

#if OUZEL_OPENGL_INTERFACE_WGL
                const GLubyte* versionPtr = glGetStringProc(GL_VERSION);

                if (!versionPtr)
                    throw std::runtime_error("Failed to get OpenGL version");

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

                if (apiMajorVersion < 2 || apiMajorVersion > 4)
                    throw std::runtime_error("Unsupported OpenGL version");
#endif

                const GLubyte* deviceName = glGetStringProc(GL_RENDERER);

                GLenum error;

                if ((error = glGetErrorProc()) != GL_NO_ERROR || !deviceName)
                    engine->log(Log::Level::WARN) << "Failed to get OpenGL renderer, error: " + std::to_string(error);
                else
                    engine->log(Log::Level::INFO) << "Using " << reinterpret_cast<const char*>(deviceName) << " for rendering";

                glEnableProc = getCoreProcAddress<PFNGLENABLEPROC>("glEnable");
                glDisableProc = getCoreProcAddress<PFNGLDISABLEPROC>("glDisable");
                glFrontFaceProc = getCoreProcAddress<PFNGLFRONTFACEPROC>("glFrontFace");
                glBindTextureProc = getCoreProcAddress<PFNGLBINDTEXTUREPROC>("glBindTexture");
                glGenTexturesProc = getCoreProcAddress<PFNGLGENTEXTURESPROC>("glGenTextures");
                glDeleteTexturesProc = getCoreProcAddress<PFNGLDELETETEXTURESPROC>("glDeleteTextures");
                glTexParameteriProc = getCoreProcAddress<PFNGLTEXPARAMETERIPROC>("glTexParameteri");
                glTexParameterivProc = getCoreProcAddress<PFNGLTEXPARAMETERIVPROC>("glTexParameteriv");
                glTexParameterfProc = getCoreProcAddress<PFNGLTEXPARAMETERFPROC>("glTexParameterf");
                glTexParameterfvProc = getCoreProcAddress<PFNGLTEXPARAMETERFVPROC>("glTexParameterfv");
                glTexImage2DProc = getCoreProcAddress<PFNGLTEXIMAGE2DPROC>("glTexImage2D");
                glTexSubImage2DProc = getCoreProcAddress<PFNGLTEXSUBIMAGE2DPROC>("glTexSubImage2D");
                glViewportProc = getCoreProcAddress<PFNGLVIEWPORTPROC>("glViewport");
                glClearProc = getCoreProcAddress<PFNGLCLEARPROC>("glClear");
                glClearColorProc = getCoreProcAddress<PFNGLCLEARCOLORPROC>("glClearColor");
                glClearStencilProc = getCoreProcAddress<PFNGLCLEARSTENCILPROC>("glClearStencil");
                glColorMaskProc = getCoreProcAddress<PFNGLCOLORMASKPROC>("glColorMask");
                glDepthMaskProc = getCoreProcAddress<PFNGLDEPTHMASKPROC>("glDepthMask");
                glDepthFuncProc = getCoreProcAddress<PFNGLDEPTHFUNCPROC>("glDepthFunc");
                glStencilMaskProc = getCoreProcAddress<PFNGLSTENCILMASKPROC>("glStencilMask");
                glStencilFuncSeparateProc = getCoreProcAddress<PFNGLSTENCILFUNCSEPARATEPROC>("glStencilFuncSeparate");
                glStencilOpSeparateProc = getCoreProcAddress<PFNGLSTENCILOPSEPARATEPROC>("glStencilOpSeparate");
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

#if OUZEL_OPENGLES
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

#if OUZEL_OPENGLES
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

                if (isVersionGreaterOrEqual(apiMajorVersion, apiMinorVersion, 3, 0))
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
                    const GLubyte* extensionsPtr = glGetStringProc(GL_EXTENSIONS);

                    if ((error = glGetErrorProc()) != GL_NO_ERROR || !extensionsPtr)
                        engine->log(Log::Level::WARN) << "Failed to get OpenGL extensions";
                    else
                        extensions = explodeString(reinterpret_cast<const char*>(extensionsPtr), ' ');
                }

                engine->log(Log::Level::ALL) << "Supported OpenGL extensions: " << extensions;

                textureBaseLevelSupported = false;
                textureMaxLevelSupported = false;
                uintElementIndexSupported = false;

                if (isVersionGreaterOrEqual(apiMajorVersion, apiMinorVersion, 4, 0))
                {
#if !OUZEL_OPENGLES
                    if (isVersionGreaterOrEqual(apiMajorVersion, apiMinorVersion, 4, 6)) // at least OpenGL 4.6
                        anisotropicFilteringSupported = true;

                    if (isVersionGreaterOrEqual(apiMajorVersion, apiMinorVersion, 4, 3)) // at least OpenGL 4.3
                    {
                        glCopyImageSubDataProc = getExtProcAddress<PFNGLCOPYIMAGESUBDATAPROC>("glCopyImageSubData");
                        glTexStorage2DMultisampleProc = getExtProcAddress<PFNGLTEXSTORAGE2DMULTISAMPLEPROC>("glTexStorage2DMultisample");
                    }
#endif
                }

                if (isVersionGreaterOrEqual(apiMajorVersion, apiMinorVersion, 3, 0))
                {
#if OUZEL_OPENGLES
                    if (isVersionGreaterOrEqual(apiMajorVersion, apiMinorVersion, 3, 1)) // at least OpenGL ES 3.1
                        glTexStorage2DMultisampleProc = getExtProcAddress<PFNGLTEXSTORAGE2DMULTISAMPLEPROC>("glTexStorage2DMultisample");
#endif
                    npotTexturesSupported = true;
                    renderTargetsSupported = true;
                    clampToBorderSupported = true;
                    multisamplingSupported = true;
                    textureBaseLevelSupported = true;
                    textureMaxLevelSupported = true;
                    uintElementIndexSupported = true;

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

#  if OUZEL_OPENGLES
                    glFramebufferTexture2DMultisampleProc = getExtProcAddress<PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC>("glFramebufferTexture2DMultisample");
#  endif
#endif
                }

                if (isVersionGreaterOrEqual(apiMajorVersion, apiMinorVersion, 2, 0))
                {
#if !OUZEL_OPENGLES
                    renderTargetsSupported = true;
                    textureBaseLevelSupported = true;
                    textureMaxLevelSupported = true;
                    uintElementIndexSupported = true;
#endif
                }

                if (isVersionGreaterOrEqual(apiMajorVersion, apiMinorVersion, 1, 3))
                {
#if !OUZEL_OPENGLES
                    clampToBorderSupported = true;
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
#if OUZEL_OPENGLES
                        glMapBufferRangeProc = getExtProcAddress<PFNGLMAPBUFFERRANGEEXTPROC>("glMapBufferRangeEXT");
#else
                        glMapBufferRangeProc = getExtProcAddress<PFNGLMAPBUFFERRANGEPROC>("glMapBufferRangeEXT");
#endif
                    }
#if OUZEL_OPENGLES // OpenGL ES
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
                    else if (extension == "OES_element_index_uint")
                        uintElementIndexSupported = true;
                    else if (extension == "GL_EXT_texture_border_clamp")
                        clampToBorderSupported = true;
#  if !OUZEL_OPENGL_INTERFACE_EAGL
                    else if (extension == "GL_EXT_copy_image")
                        glCopyImageSubDataProc = getExtProcAddress<PFNGLCOPYIMAGESUBDATAEXTPROC>("glCopyImageSubDataEXT");
                    else if (extension == "GL_EXT_multisampled_render_to_texture")
                    {
                        multisamplingSupported = true;
                        glRenderbufferStorageMultisampleProc = getExtProcAddress<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC>("glRenderbufferStorageMultisampleEXT");
                        glFramebufferTexture2DMultisampleProc = getExtProcAddress<PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC>("glFramebufferTexture2DMultisampleEXT");
                    }
                    else if (extension == "GL_IMG_multisampled_render_to_texture")
                    {
                        multisamplingSupported = true;
                        glRenderbufferStorageMultisampleProc = getExtProcAddress<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC>("glRenderbufferStorageMultisampleIMG");
                        glFramebufferTexture2DMultisampleProc = getExtProcAddress<PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC>("glFramebufferTexture2DMultisampleIMG");
                    }
#  endif
#else // OpenGL
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
                    throw std::system_error(makeErrorCode(error), "Failed to set depth function");

#if !OUZEL_OPENGLES
                if (srgb)
                    glEnableProc(GL_FRAMEBUFFER_SRGB);

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to enable sRGB frame buffer");

                if (sampleCount > 1)
                {
                    glEnableProc(GL_MULTISAMPLE);

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to enable multi-sampling");
                }
#endif

                if (glGenVertexArraysProc)
                {
                    glGenVertexArraysProc(1, &vertexArrayId);

                    glBindVertexArrayProc(vertexArrayId);

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to bind vertex array");
                }

                setFrontFace(GL_CW);
            }

            void RenderDevice::resizeFrameBuffer()
            {
            }

            void RenderDevice::setUniform(GLint location, DataType dataType, const void* data)
            {
                switch (dataType)
                {
                    case DataType::INTEGER:
                        glUniform1ivProc(location, 1, static_cast<const GLint*>(data));
                        break;
                    case DataType::UNSIGNED_INTEGER:
                        if (!glUniform1uivProc) throw std::runtime_error("Unsupported uniform size");
                        glUniform1uivProc(location, 1, static_cast<const GLuint*>(data));
                        break;
                    case DataType::INTEGER_VECTOR2:
                        glUniform2ivProc(location, 1, static_cast<const GLint*>(data));
                        break;
                    case DataType::UNSIGNED_INTEGER_VECTOR2:
                        if (!glUniform2uivProc) throw std::runtime_error("Unsupported uniform size");
                        glUniform2uivProc(location, 1, static_cast<const GLuint*>(data));
                        break;
                    case DataType::INTEGER_VECTOR3:
                        glUniform3ivProc(location, 1, static_cast<const GLint*>(data));
                        break;
                    case DataType::UNSIGNED_INTEGER_VECTOR3:
                        if (!glUniform3uivProc) throw std::runtime_error("Unsupported uniform size");
                        glUniform3uivProc(location, 1, static_cast<const GLuint*>(data));
                        break;
                    case DataType::INTEGER_VECTOR4:
                        glUniform4ivProc(location, 1, static_cast<const GLint*>(data));
                        break;
                    case DataType::UNSIGNED_INTEGER_VECTOR4:
                        if (!glUniform4uivProc) throw std::runtime_error("Unsupported uniform size");
                        glUniform4uivProc(location, 1, static_cast<const GLuint*>(data));
                        break;
                    case DataType::FLOAT:
                        glUniform1fvProc(location, 1, static_cast<const GLfloat*>(data));
                        break;
                    case DataType::FLOAT_VECTOR2:
                        glUniform2fvProc(location, 1, static_cast<const GLfloat*>(data));
                        break;
                    case DataType::FLOAT_VECTOR3:
                        glUniform3fvProc(location, 1, static_cast<const GLfloat*>(data));
                        break;
                    case DataType::FLOAT_VECTOR4:
                        glUniform4fvProc(location, 1, static_cast<const GLfloat*>(data));
                        break;
                    case DataType::FLOAT_MATRIX3:
                        glUniformMatrix3fvProc(location, 1, GL_FALSE, static_cast<const GLfloat*>(data));
                        break;
                    case DataType::FLOAT_MATRIX4:
                        glUniformMatrix4fvProc(location, 1, GL_FALSE, static_cast<const GLfloat*>(data));
                        break;
                    default:
                        throw std::runtime_error("Unsupported uniform size");
                }
            }

            void RenderDevice::process()
            {
                graphics::RenderDevice::process();
                executeAll();

                RenderTarget* currentRenderTarget = nullptr;
                Shader* currentShader = nullptr;

                CommandBuffer commandBuffer;
                std::unique_ptr<Command> command;

                for (;;)
                {
                    std::unique_lock<std::mutex> lock(commandQueueMutex);
                    while (commandQueue.empty()) commandQueueCondition.wait(lock);
                    commandBuffer = std::move(commandQueue.front());
                    commandQueue.pop();
                    lock.unlock();

                    while (!commandBuffer.isEmpty())
                    {
                        command = commandBuffer.popCommand();

                        switch (command->type)
                        {
                            case Command::Type::RESIZE:
                            {
                                auto resizeCommand = static_cast<const ResizeCommand*>(command.get());
                                frameBufferWidth = static_cast<GLsizei>(resizeCommand->size.v[0]);
                                frameBufferHeight = static_cast<GLsizei>(resizeCommand->size.v[1]);
                                resizeFrameBuffer();
                                break;
                            }

                            case Command::Type::PRESENT:
                            {
                                present();
                                break;
                            }

                            case Command::Type::DELETE_RESOURCE:
                            {
                                auto deleteResourceCommand = static_cast<const DeleteResourceCommand*>(command.get());
                                resources[deleteResourceCommand->resource - 1].reset();
                                break;
                            }

                            case Command::INIT_RENDER_TARGET:
                            {
                                auto initRenderTargetCommand = static_cast<const InitRenderTargetCommand*>(command.get());

                                std::set<Texture*> colorTextures;
                                for (uintptr_t colorTextureId : initRenderTargetCommand->colorTextures)
                                    colorTextures.insert(getResource<Texture>(colorTextureId));

                                std::unique_ptr<RenderTarget> renderTarget(new RenderTarget(*this,
                                                                                                  colorTextures,
                                                                                                  getResource<Texture>(initRenderTargetCommand->depthTexture)));

                                if (initRenderTargetCommand->renderTarget > resources.size())
                                    resources.resize(initRenderTargetCommand->renderTarget);
                                resources[initRenderTargetCommand->renderTarget - 1] = std::move(renderTarget);
                                break;
                            }

                            case Command::Type::SET_RENDER_TARGET:
                            {
                                auto setRenderTargetCommand = static_cast<const SetRenderTargetCommand*>(command.get());

                                if (setRenderTargetCommand->renderTarget)
                                {
                                    currentRenderTarget = getResource<RenderTarget>(setRenderTargetCommand->renderTarget);

                                    if (!currentRenderTarget->getFrameBufferId()) break;
                                    bindFrameBuffer(currentRenderTarget->getFrameBufferId());
                                    setFrontFace(GL_CCW);
                                }
                                else
                                {
                                    currentRenderTarget = nullptr;
                                    bindFrameBuffer(frameBufferId);
                                    setFrontFace(GL_CW);
                                }
                                break;
                            }

                            case Command::Type::CLEAR_RENDER_TARGET:
                            {
                                auto clearCommand = static_cast<const ClearRenderTargetCommand*>(command.get());

                                GLbitfield clearMask = (clearCommand->clearColorBuffer ? GL_COLOR_BUFFER_BIT : 0) |
                                    (clearCommand->clearDepthBuffer ? GL_DEPTH_BUFFER_BIT : 0 |
                                    (clearCommand->clearStencilBuffer ? GL_STENCIL_BUFFER_BIT : 0));

                                if (clearMask)
                                {
                                    if (clearCommand->clearColorBuffer)
                                    {
                                        setClearColorValue(clearCommand->clearColor.norm());
                                        glColorMaskProc(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                                    }

                                    if (clearCommand->clearDepthBuffer)
                                    {
                                        setClearDepthValue(clearCommand->clearDepth);
                                        glDepthMaskProc(GL_TRUE);
                                    }

                                    if (clearCommand->clearStencilBuffer)
                                    {
                                        setClearStencilValue(static_cast<GLint>(clearCommand->clearStencil));
                                        glStencilMaskProc(0xFFFFFFFF);
                                    }

                                    // disable the scissor test to clear entire render target
                                    if (stateCache.scissorTestEnabled)
                                        glDisableProc(GL_SCISSOR_TEST);

                                    glClearProc(clearMask);

                                    if (stateCache.scissorTestEnabled)
                                        glEnableProc(GL_SCISSOR_TEST);
                                    // restore the masks
                                    if (clearCommand->clearColorBuffer)
                                        glColorMaskProc(stateCache.redMask,
                                                        stateCache.greenMask,
                                                        stateCache.blueMask,
                                                        stateCache.alphaMask);
                                    if (clearCommand->clearDepthBuffer)
                                        glDepthMaskProc(stateCache.depthMask);
                                    if (clearCommand->clearStencilBuffer)
                                        glStencilMaskProc(stateCache.stencilMask);

                                    GLenum error;
                                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                                        throw std::system_error(makeErrorCode(error), "Failed to clear frame buffer");
                                }

                                break;
                            }

                            case Command::Type::BLIT:
                            {
#if !OUZEL_OPENGLES
                                auto blitCommand = static_cast<const BlitCommand*>(command.get());

                                Texture* sourceTexture = getResource<Texture>(blitCommand->sourceTexture);
                                Texture* destinationTexture = getResource<Texture>(blitCommand->destinationTexture);

                                if (glCopyImageSubDataProc)
                                    glCopyImageSubDataProc(sourceTexture->getTextureId(),
                                                           GL_TEXTURE_2D,
                                                           static_cast<GLint>(blitCommand->sourceLevel),
                                                           static_cast<GLint>(blitCommand->sourceX),
                                                           static_cast<GLint>(blitCommand->sourceY),
                                                           0,
                                                           destinationTexture->getTextureId(),
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

                            case Command::Type::SET_SCISSOR_TEST:
                            {
                                auto setScissorTestCommand = static_cast<const SetScissorTestCommand*>(command.get());

                                setScissorTest(setScissorTestCommand->enabled,
                                               static_cast<GLint>(setScissorTestCommand->rectangle.position.v[0]),
                                               static_cast<GLint>(setScissorTestCommand->rectangle.position.v[1]),
                                               static_cast<GLsizei>(setScissorTestCommand->rectangle.size.v[0]),
                                               static_cast<GLsizei>(setScissorTestCommand->rectangle.size.v[1]));

                                break;
                            }

                            case Command::Type::SET_VIEWPORT:
                            {
                                auto setViewportCommand = static_cast<const SetViewportCommand*>(command.get());

                                setViewport(static_cast<GLint>(setViewportCommand->viewport.position.v[0]),
                                            static_cast<GLint>(setViewportCommand->viewport.position.v[1]),
                                            static_cast<GLsizei>(setViewportCommand->viewport.size.v[0]),
                                            static_cast<GLsizei>(setViewportCommand->viewport.size.v[1]));

                                break;
                            }

                            case Command::Type::INIT_DEPTH_STENCIL_STATE:
                            {
                                auto initDepthStencilStateCommand = static_cast<const InitDepthStencilStateCommand*>(command.get());
                                std::unique_ptr<DepthStencilState> depthStencilState(new DepthStencilState(*this,
                                                                                                                 initDepthStencilStateCommand->depthTest,
                                                                                                                 initDepthStencilStateCommand->depthWrite,
                                                                                                                 initDepthStencilStateCommand->compareFunction,
                                                                                                                 initDepthStencilStateCommand->stencilEnabled,
                                                                                                                 initDepthStencilStateCommand->stencilReadMask,
                                                                                                                 initDepthStencilStateCommand->stencilWriteMask,
                                                                                                                 initDepthStencilStateCommand->frontFaceStencil,
                                                                                                                 initDepthStencilStateCommand->backFaceStencil));

                                if (initDepthStencilStateCommand->depthStencilState > resources.size())
                                    resources.resize(initDepthStencilStateCommand->depthStencilState);
                                resources[initDepthStencilStateCommand->depthStencilState - 1] = std::move(depthStencilState);
                                break;
                            }

                            case Command::Type::SET_DEPTH_STENCIL_STATE:
                            {
                                auto setDepthStencilStateCommand = static_cast<const SetDepthStencilStateCommand*>(command.get());

                                if (setDepthStencilStateCommand->depthStencilState)
                                {
                                    DepthStencilState* depthStencilState = getResource<DepthStencilState>(setDepthStencilStateCommand->depthStencilState);

                                    enableDepthTest(depthStencilState->getDepthTest());
                                    setDepthMask(depthStencilState->getDepthMask());
                                    glDepthFuncProc(depthStencilState->getCompareFunction());
                                    enableStencilTest(depthStencilState->getStencilTest());
                                    setStencilMask(depthStencilState->getStencilWriteMask());
                                    glStencilOpSeparateProc(GL_FRONT,
                                                            depthStencilState->getFrontFaceFail(),
                                                            depthStencilState->getFrontFaceDepthFail(),
                                                            depthStencilState->getFrontFacePass());
                                    glStencilFuncSeparateProc(GL_FRONT,
                                                              depthStencilState->getFrontFaceFunction(),
                                                              static_cast<GLint>(setDepthStencilStateCommand->stencilReferenceValue),
                                                              depthStencilState->getStencilReadMask());
                                    glStencilOpSeparateProc(GL_BACK,
                                                            depthStencilState->getBackFaceFail(),
                                                            depthStencilState->getBackFaceDepthFail(),
                                                            depthStencilState->getBackFacePass());
                                    glStencilFuncSeparateProc(GL_BACK,
                                                              depthStencilState->getBackFaceFunction(),
                                                              static_cast<GLint>(setDepthStencilStateCommand->stencilReferenceValue),
                                                              depthStencilState->getStencilReadMask());
                                }
                                else
                                {
                                    enableDepthTest(false);
                                    setDepthMask(GL_FALSE);
                                    setDepthFunc(GL_LESS);
                                    enableStencilTest(false);
                                    setStencilMask(0xFFFFFFFF);
                                }

                                GLenum error;
                                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                                    throw std::system_error(makeErrorCode(error), "Failed to update depth stencil state");

                                break;
                            }

                            case Command::Type::SET_PIPELINE_STATE:
                            {
                                auto setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command.get());

                                BlendState* blendState = getResource<BlendState>(setPipelineStateCommand->blendState);
                                Shader* shader = getResource<Shader>(setPipelineStateCommand->shader);
                                currentShader = shader;

                                if (blendState)
                                {
                                    setBlendState(blendState->isBlendEnabled(),
                                                  blendState->getModeRGB(),
                                                  blendState->getModeAlpha(),
                                                  blendState->getSourceFactorRGB(),
                                                  blendState->getDestFactorRGB(),
                                                  blendState->getSourceFactorAlpha(),
                                                  blendState->getDestFactorAlpha());

                                    setColorMask(blendState->getRedMask(),
                                                 blendState->getGreenMask(),
                                                 blendState->getBlueMask(),
                                                 blendState->getAlphaMask());
                                }
                                else
                                {
                                    setBlendState(false, 0, 0, 0, 0, 0, 0);
                                    setColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                                }

                                if (shader)
                                {
                                    assert(shader->getProgramId());
                                    useProgram(shader->getProgramId());
                                }
                                else
                                    useProgram(0);

                                const GLenum cullFace = getCullFace(setPipelineStateCommand->cullMode);
                                setCullFace(cullFace != GL_NONE, cullFace);

#if OUZEL_OPENGLES
                                if (setPipelineStateCommand->fillMode != FillMode::SOLID)
                                    engine->log(Log::Level::WARN) << "Unsupported fill mode";
#else
                                setPolygonFillMode(getFillMode(setPipelineStateCommand->fillMode));
#endif

                                break;
                            }

                            case Command::Type::DRAW:
                            {
                                auto drawCommand = static_cast<const DrawCommand*>(command.get());

                                // mesh buffer
                                Buffer* indexBuffer = getResource<Buffer>(drawCommand->indexBuffer);
                                Buffer* vertexBuffer = getResource<Buffer>(drawCommand->vertexBuffer);

                                assert(indexBuffer);
                                assert(indexBuffer->getBufferId());
                                assert(vertexBuffer);
                                assert(vertexBuffer->getBufferId());

                                // draw
                                bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getBufferId());
                                bindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getBufferId());

                                GLuint vertexOffset = 0;

                                for (GLuint index = 0; index < RenderDevice::VERTEX_ATTRIBUTES.size(); ++index)
                                {
                                    const Vertex::Attribute& vertexAttribute = RenderDevice::VERTEX_ATTRIBUTES[index];

                                    glEnableVertexAttribArrayProc(index);
                                    glVertexAttribPointerProc(index,
                                                              getArraySize(vertexAttribute.dataType),
                                                              getVertexType(vertexAttribute.dataType),
                                                              isNormalized(vertexAttribute.dataType),
                                                              static_cast<GLsizei>(sizeof(Vertex)),
                                                              reinterpret_cast<void*>(static_cast<uintptr_t>(vertexOffset)));

                                    vertexOffset += getDataTypeSize(vertexAttribute.dataType);
                                }

                                GLenum error;
                                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                                    throw std::system_error(makeErrorCode(error), "Failed to update vertex attributes");

                                assert(drawCommand->indexCount);
                                assert(indexBuffer->getSize());
                                assert(vertexBuffer->getSize());

                                glDrawElementsProc(getDrawMode(drawCommand->drawMode),
                                                   static_cast<GLsizei>(drawCommand->indexCount),
                                                   getIndexType(drawCommand->indexSize),
                                                   reinterpret_cast<void*>(static_cast<uintptr_t>(drawCommand->startIndex * drawCommand->indexSize)));

                                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                                    throw std::system_error(makeErrorCode(error), "Failed to draw elements");

                                break;
                            }

                            case Command::Type::PUSH_DEBUG_MARKER:
                            {
                                auto pushDebugMarkerCommand = static_cast<const PushDebugMarkerCommand*>(command.get());
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
                                auto initBlendStateCommand = static_cast<const InitBlendStateCommand*>(command.get());

                                std::unique_ptr<BlendState> blendState(new BlendState(*this,
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
                                resources[initBlendStateCommand->blendState - 1] = std::move(blendState);
                                break;
                            }

                            case Command::Type::INIT_BUFFER:
                            {
                                auto initBufferCommand = static_cast<const InitBufferCommand*>(command.get());

                                std::unique_ptr<Buffer> buffer(new Buffer(*this,
                                                                                initBufferCommand->usage,
                                                                                initBufferCommand->flags,
                                                                                initBufferCommand->data,
                                                                                initBufferCommand->size));

                                if (initBufferCommand->buffer > resources.size())
                                    resources.resize(initBufferCommand->buffer);
                                resources[initBufferCommand->buffer - 1] = std::move(buffer);
                                break;
                            }

                            case Command::Type::SET_BUFFER_DATA:
                            {
                                auto setBufferDataCommand = static_cast<const SetBufferDataCommand*>(command.get());

                                Buffer* buffer = getResource<Buffer>(setBufferDataCommand->buffer);
                                buffer->setData(setBufferDataCommand->data);
                                break;
                            }

                            case Command::Type::INIT_SHADER:
                            {
                                auto initShaderCommand = static_cast<const InitShaderCommand*>(command.get());

                                std::unique_ptr<Shader> shader(new Shader(*this,
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
                                resources[initShaderCommand->shader - 1] = std::move(shader);
                                break;
                            }

                            case Command::Type::SET_SHADER_CONSTANTS:
                            {
                                auto setShaderConstantsCommand = static_cast<const SetShaderConstantsCommand*>(command.get());

                                if (!currentShader)
                                    throw std::runtime_error("No shader set");

                                // pixel shader constants
                                const std::vector<Shader::Location>& fragmentShaderConstantLocations = currentShader->getFragmentShaderConstantLocations();

                                if (setShaderConstantsCommand->fragmentShaderConstants.size() > fragmentShaderConstantLocations.size())
                                    throw std::runtime_error("Invalid pixel shader constant size");

                                for (size_t i = 0; i < setShaderConstantsCommand->fragmentShaderConstants.size(); ++i)
                                {
                                    const Shader::Location& fragmentShaderConstantLocation = fragmentShaderConstantLocations[i];
                                    const std::vector<float>& fragmentShaderConstant = setShaderConstantsCommand->fragmentShaderConstants[i];

                                    setUniform(fragmentShaderConstantLocation.location,
                                               fragmentShaderConstantLocation.dataType,
                                               fragmentShaderConstant.data());
                                }

                                // vertex shader constants
                                const std::vector<Shader::Location>& vertexShaderConstantLocations = currentShader->getVertexShaderConstantLocations();

                                if (setShaderConstantsCommand->vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                                    throw std::runtime_error("Invalid vertex shader constant size");

                                for (size_t i = 0; i < setShaderConstantsCommand->vertexShaderConstants.size(); ++i)
                                {
                                    const Shader::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                                    const std::vector<float>& vertexShaderConstant = setShaderConstantsCommand->vertexShaderConstants[i];

                                    setUniform(vertexShaderConstantLocation.location,
                                               vertexShaderConstantLocation.dataType,
                                               vertexShaderConstant.data());
                                }

                                break;
                            }

                            case Command::Type::INIT_TEXTURE:
                            {
                                auto initTextureCommand = static_cast<const InitTextureCommand*>(command.get());

                                std::unique_ptr<Texture> texture(new Texture(*this,
                                                                                   initTextureCommand->levels,
                                                                                   initTextureCommand->dimensions,
                                                                                   initTextureCommand->flags,
                                                                                   initTextureCommand->sampleCount,
                                                                                   initTextureCommand->pixelFormat));

                                if (initTextureCommand->texture > resources.size())
                                    resources.resize(initTextureCommand->texture);
                                resources[initTextureCommand->texture - 1] = std::move(texture);
                                break;
                            }

                            case Command::Type::SET_TEXTURE_DATA:
                            {
                                auto setTextureDataCommand = static_cast<const SetTextureDataCommand*>(command.get());

                                Texture* texture = getResource<Texture>(setTextureDataCommand->texture);
                                texture->setData(setTextureDataCommand->levels);

                                break;
                            }

                            case Command::Type::SET_TEXTURE_PARAMETERS:
                            {
                                auto setTextureParametersCommand = static_cast<const SetTextureParametersCommand*>(command.get());

                                Texture* texture = getResource<Texture>(setTextureParametersCommand->texture);
                                texture->setFilter(setTextureParametersCommand->filter);
                                texture->setAddressX(setTextureParametersCommand->addressX);
                                texture->setAddressY(setTextureParametersCommand->addressY);
                                texture->setAddressZ(setTextureParametersCommand->addressZ);
                                texture->setMaxAnisotropy(setTextureParametersCommand->maxAnisotropy);
                                break;
                            }

                            case Command::Type::SET_TEXTURES:
                            {
                                auto setTexturesCommand = static_cast<const SetTexturesCommand*>(command.get());

                                for (uint32_t layer = 0; layer < setTexturesCommand->textures.size(); ++layer)
                                {
                                    if (Texture* texture = getResource<Texture>(setTexturesCommand->textures[layer]))
                                        bindTexture(GL_TEXTURE_2D, layer, texture->getTextureId());
                                    else
                                        bindTexture(GL_TEXTURE_2D, layer, 0);
                                }

                                break;
                            }

                            default:
                                throw std::runtime_error("Invalid command");
                        }

                        if (command->type == Command::Type::PRESENT) return;
                    }
                }
            }

            void RenderDevice::present()
            {
            }

            void RenderDevice::generateScreenshot(const std::string& filename)
            {
                bindFrameBuffer(frameBufferId);

                const GLsizei pixelSize = 4;

                std::vector<uint8_t> data(static_cast<size_t>(frameBufferWidth * frameBufferHeight * pixelSize));

                glReadPixelsProc(0, 0, frameBufferWidth, frameBufferHeight,
                                 GL_RGBA, GL_UNSIGNED_BYTE, data.data());

                GLenum error;

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to read pixels from frame buffer");

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
                    throw std::runtime_error("Failed to save image to file");
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
