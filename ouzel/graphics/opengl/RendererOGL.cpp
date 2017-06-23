// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#if OUZEL_PLATFORM_MACOS
#include <dlfcn.h>
#endif

#include <sstream>

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

#if OUZEL_OPENGL_INTERFACE_EGL
    #include "EGL/egl.h"
#elif OUZEL_OPENGL_INTERFACE_GLX
    #define GL_GLEXT_PROTOTYPES 1
    #include "GL/glx.h"
    #include "GL/glxext.h"
#elif OUZEL_OPENGL_INTERFACE_WGL
    #include "GL/wglext.h"
#endif

#include "RendererOGL.h"
#include "TextureResourceOGL.h"
#include "ShaderResourceOGL.h"
#include "MeshBufferResourceOGL.h"
#include "BufferResourceOGL.h"
#include "BlendStateResourceOGL.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "core/Cache.h"
#include "utils/Log.h"
#include "stb_image_write.h"

#if OUZEL_SUPPORTS_OPENGLES
#include "ColorPSGLES2.h"
#include "ColorVSGLES2.h"
#include "TexturePSGLES2.h"
#include "TextureVSGLES2.h"
#include "ColorPSGLES3.h"
#include "ColorVSGLES3.h"
#include "TexturePSGLES3.h"
#include "TextureVSGLES3.h"
#else
#include "ColorPSGL2.h"
#include "ColorVSGL2.h"
#include "TexturePSGL2.h"
#include "TextureVSGL2.h"
#include "ColorPSGL3.h"
#include "ColorVSGL3.h"
#include "TexturePSGL3.h"
#include "TextureVSGL3.h"
#include "ColorPSGL4.h"
#include "ColorVSGL4.h"
#include "TexturePSGL4.h"
#include "TextureVSGL4.h"
#endif

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
        RendererOGL::RendererOGL():
            Renderer(Driver::OPENGL)
        {
            projectionTransform = Matrix4(1.0f, 0.0f, 0.0f, 0.0f,
                                          0.0f, 1.0f, 0.0f, 0.0f,
                                          0.0f, 0.0f, 2.0f, -1.0f,
                                          0.0f, 0.0f, 0.0f, 1.0f);

            renderTargetProjectionTransform = Matrix4(1.0f, 0.0f, 0.0f, 0.0f,
                                                      0.0f, -1.0f, 0.0f, 0.0f,
                                                      0.0f, 0.0f, 2.0f, -1.0f,
                                                      0.0f, 0.0f, 0.0f, 1.0f);

            stateCache.bufferId[GL_ELEMENT_ARRAY_BUFFER] = 0;
            stateCache.bufferId[GL_ARRAY_BUFFER] = 0;

            std::fill(std::begin(stateCache.textureId), std::end(stateCache.textureId), 0);
        }

        RendererOGL::~RendererOGL()
        {
            resourceDeleteSet.clear();
            resources.clear();
        }

        bool RendererOGL::init(Window* newWindow,
                               const Size2& newSize,
                               uint32_t newSampleCount,
                               Texture::Filter newTextureFilter,
                               uint32_t newMaxAnisotropy,
                               bool newVerticalSync,
                               bool newDepth,
                               bool newDebugRenderer)
        {
            if (!Renderer::init(newWindow,
                                newSize,
                                newSampleCount,
                                newTextureFilter,
                                newMaxAnisotropy,
                                newVerticalSync,
                                newDepth,
                                newDebugRenderer))
            {
                return false;
            }

            frameBufferWidth = static_cast<GLsizei>(size.v[0]);
            frameBufferHeight = static_cast<GLsizei>(size.v[1]);

            //const GLubyte* deviceVendor = glGetString(GL_VENDOR);
            const GLubyte* deviceName = glGetString(GL_RENDERER);

            if (checkOpenGLError() || !deviceName)
            {
                Log(Log::Level::WARN) << "Failed to get OpenGL renderer";
            }
            else
            {
                Log(Log::Level::INFO) << "Using " << reinterpret_cast<const char*>(deviceName) << " for rendering";
            }

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

#if OUZEL_OPENGL_INTERFACE_EAGL
            glBlitFramebufferProc = glBlitFramebuffer;
#endif

            if (apiMajorVersion >= 3)
            {
#if OUZEL_OPENGL_INTERFACE_EAGL
                glGetStringiProc = glGetStringi;
#else
                glGetStringiProc = reinterpret_cast<PFNGLGETSTRINGIPROC>(getProcAddress("glGetStringi"));
#endif
            }
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

            if (apiMajorVersion >= 3)
            {
                glGetStringiProc = reinterpret_cast<PFNGLGETSTRINGIPROC>(getProcAddress("glGetStringi"));
            }
#endif

            anisotropicFilteringSupported = false;

            std::vector<std::string> extensions;

            if (apiMajorVersion >= 3)
            {
                GLint extensionCount;
                glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

                if (checkOpenGLError())
                {
                    Log(Log::Level::WARN) << "Failed to get OpenGL extension count";
                }
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

                if (checkOpenGLError() || !extensionPtr)
                {
                    Log(Log::Level::WARN) << "Failed to get OpenGL extensions";
                }
                else
                {
                    std::istringstream extensionStringStream(reinterpret_cast<const char*>(extensionPtr));

                    for (std::string extension; extensionStringStream >> extension;)
                    {
                        extensions.push_back(extension);
                    }
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
                    {
                        anisotropicFilteringSupported = true;
                    }
                }

#if OUZEL_OPENGL_INTERFACE_EAGL
                glGenVertexArraysProc = glGenVertexArraysOES;
                glBindVertexArrayProc = glBindVertexArrayOES;
                glDeleteVertexArraysProc = glDeleteVertexArraysOES;
                glMapBufferProc = glMapBufferOES;
                glUnmapBufferProc = glUnmapBufferOES;
                glMapBufferRangeProc = glMapBufferRangeEXT;

                glRenderbufferStorageMultisampleProc = glRenderbufferStorageMultisampleAPPLE;
#else
                glGenVertexArraysProc = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(getProcAddress("glGenVertexArrays"));
                glBindVertexArrayProc = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(getProcAddress("glBindVertexArray"));
                glDeleteVertexArraysProc = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(getProcAddress("glDeleteVertexArrays"));
    #if OUZEL_OPENGL_INTERFACE_EGL
                glMapBufferProc = reinterpret_cast<PFNGLMAPBUFFEROESPROC>(getProcAddress("glMapBuffer"));
    #elif !OUZEL_SUPPORTS_OPENGLES
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
    #endif
                glUnmapBufferProc = reinterpret_cast<PFNGLUNMAPBUFFERPROC>(getProcAddress("glUnmapBuffer"));
                glMapBufferRangeProc = reinterpret_cast<PFNGLMAPBUFFERRANGEPROC>(getProcAddress("glMapBufferRange"));
                glRenderbufferStorageMultisampleProc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>(getProcAddress("glRenderbufferStorageMultisample"));

    #if OUZEL_SUPPORTS_OPENGLES
                glFramebufferTexture2DMultisampleProc = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC>(getProcAddress("glFramebufferTexture2DMultisample"));
    #endif
#endif
            }
            else
            {
                npotTexturesSupported = false;
                multisamplingSupported = false;

#if !OUZEL_SUPPORTS_OPENGLES
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
#if !OUZEL_SUPPORTS_OPENGLES
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
#elif OUZEL_OPENGL_INTERFACE_EAGL
                    else if (extension == "GL_APPLE_framebuffer_multisample")
                    {
                        multisamplingSupported = true;
                        glRenderbufferStorageMultisampleProc = glRenderbufferStorageMultisampleAPPLE;
                    }
#elif OUZEL_OPENGL_INTERFACE_EGL
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
#endif // OUZEL_OPENGL_INTERFACE_EGL
                }

                if (!multisamplingSupported)
                {
                    sampleCount = 1;
                }
            }

            std::shared_ptr<Shader> textureShader = std::make_shared<Shader>();

            switch (apiMajorVersion)
            {
#if OUZEL_SUPPORTS_OPENGLES
                case 2:
                    textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TexturePSGLES2_glsl), std::end(TexturePSGLES2_glsl)),
                                                   std::vector<uint8_t>(std::begin(TextureVSGLES2_glsl), std::end(TextureVSGLES2_glsl)),
                                                   VertexPCT::ATTRIBUTES,
                                                   {{"color", DataType::FLOAT_VECTOR4}},
                                                   {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                    break;
                case 3:
                    textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TexturePSGLES3_glsl), std::end(TexturePSGLES3_glsl)),
                                                   std::vector<uint8_t>(std::begin(TextureVSGLES3_glsl), std::end(TextureVSGLES3_glsl)),
                                                   VertexPCT::ATTRIBUTES,
                                                   {{"color", DataType::FLOAT_VECTOR4}},
                                                   {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                    break;
#else
                case 2:
                    textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TexturePSGL2_glsl), std::end(TexturePSGL2_glsl)),
                                                   std::vector<uint8_t>(std::begin(TextureVSGL2_glsl), std::end(TextureVSGL2_glsl)),
                                                   VertexPCT::ATTRIBUTES,
                                                   {{"color", DataType::FLOAT_VECTOR4}},
                                                   {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                    break;
                case 3:
                    textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TexturePSGL3_glsl), std::end(TexturePSGL3_glsl)),
                                                   std::vector<uint8_t>(std::begin(TextureVSGL3_glsl), std::end(TextureVSGL3_glsl)),
                                                   VertexPCT::ATTRIBUTES,
                                                   {{"color", DataType::FLOAT_VECTOR4}},
                                                   {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                    break;
                case 4:
                    textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TexturePSGL4_glsl), std::end(TexturePSGL4_glsl)),
                                                   std::vector<uint8_t>(std::begin(TextureVSGL4_glsl), std::end(TextureVSGL4_glsl)),
                                                   VertexPCT::ATTRIBUTES,
                                                   {{"color", DataType::FLOAT_VECTOR4}},
                                                   {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                    break;
#endif
                default:
                    Log(Log::Level::ERR) << "Unsupported OpenGL version";
                    return false;
            }

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            std::shared_ptr<Shader> colorShader = std::make_shared<Shader>();

            switch (apiMajorVersion)
            {
#if OUZEL_SUPPORTS_OPENGLES
                case 2:
                    colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(ColorPSGLES2_glsl), std::end(ColorPSGLES2_glsl)),
                                                 std::vector<uint8_t>(std::begin(ColorVSGLES2_glsl), std::end(ColorVSGLES2_glsl)),
                                                 VertexPC::ATTRIBUTES,
                                                 {{"color", DataType::FLOAT_VECTOR4}},
                                                 {{"modelViewProj", DataType::FLOAT_MATRIX4}});

                    break;
                case 3:
                    colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(ColorPSGLES3_glsl), std::end(ColorPSGLES3_glsl)),
                                                 std::vector<uint8_t>(std::begin(ColorVSGLES3_glsl), std::end(ColorVSGLES3_glsl)),
                                                 VertexPC::ATTRIBUTES,
                                                 {{"color", DataType::FLOAT_VECTOR4}},
                                                 {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                    break;
#else
                case 2:
                    colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(ColorPSGL2_glsl), std::end(ColorPSGL2_glsl)),
                                                 std::vector<uint8_t>(std::begin(ColorVSGL2_glsl), std::end(ColorVSGL2_glsl)),
                                                 VertexPC::ATTRIBUTES,
                                                 {{"color", DataType::FLOAT_VECTOR4}},
                                                 {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                    break;
                case 3:
                    colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(ColorPSGL3_glsl), std::end(ColorPSGL3_glsl)),
                                                 std::vector<uint8_t>(std::begin(ColorVSGL3_glsl), std::end(ColorVSGL3_glsl)),
                                                 VertexPC::ATTRIBUTES,
                                                 {{"color", DataType::FLOAT_VECTOR4}},
                                                 {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                    break;
                case 4:
                    colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(ColorPSGL4_glsl), std::end(ColorPSGL4_glsl)),
                                                 std::vector<uint8_t>(std::begin(ColorVSGL4_glsl), std::end(ColorVSGL4_glsl)),
                                                 VertexPC::ATTRIBUTES,
                                                 {{"color", DataType::FLOAT_VECTOR4}},
                                                 {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                    break;
#endif
                default:
                    Log(Log::Level::ERR) << "Unsupported OpenGL version";
                    return false;
            }

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            std::shared_ptr<BlendState> noBlendState = std::make_shared<BlendState>();

            noBlendState->init(false,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            std::shared_ptr<BlendState> addBlendState = std::make_shared<BlendState>();

            addBlendState->init(true,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            std::shared_ptr<BlendState> multiplyBlendState = std::make_shared<BlendState>();

            multiplyBlendState->init(true,
                                     BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                     BlendState::BlendOperation::ADD,
                                     BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                     BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            std::shared_ptr<BlendState> alphaBlendState = std::make_shared<BlendState>();

            alphaBlendState->init(true,
                                  BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                  BlendState::BlendOperation::ADD,
                                  BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                  BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            std::shared_ptr<Texture> whitePixelTexture = std::make_shared<Texture>();
            whitePixelTexture->initFromBuffer({255, 255, 255, 255}, Size2(1.0f, 1.0f), false, false);
            sharedEngine->getCache()->setTexture(TEXTURE_WHITE_PIXEL, whitePixelTexture);

            glDisable(GL_DITHER);
            glDepthFunc(GL_LEQUAL);

            if (RendererOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set depth function";
                return false;
            }

#if !OUZEL_SUPPORTS_OPENGLES
            if (sampleCount > 1)
            {
                glEnable(GL_MULTISAMPLE);

                if (RendererOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to enable multi-sampling";
                    return false;
                }
            }
#endif

            return true;
        }

        bool RendererOGL::upload()
        {
            clearMask = 0;
            if (clearColorBuffer) clearMask |= GL_COLOR_BUFFER_BIT;
            if (clearDepthBuffer) clearMask |= GL_DEPTH_BUFFER_BIT;

            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();

            frameBufferWidth = static_cast<GLsizei>(size.v[0]);
            frameBufferHeight = static_cast<GLsizei>(size.v[1]);

            dirty = false;

            return true;
        }

        bool RendererOGL::process()
        {
            if (!lockContext())
            {
                return false;
            }

            return Renderer::process();
        }

        bool RendererOGL::draw(const std::vector<DrawCommand>& drawCommands)
        {
            if (drawCommands.empty())
            {
                frameBufferClearedFrame = currentFrame;

                if (clearMask)
                {
                    if (!bindFrameBuffer(frameBufferId))
                    {
                        return false;
                    }

                    if (!setViewport(0, 0,
                                     frameBufferWidth,
                                     frameBufferHeight))
                    {
                        return false;
                    }

                    glClearColor(frameBufferClearColor[0],
                                 frameBufferClearColor[1],
                                 frameBufferClearColor[2],
                                 frameBufferClearColor[3]);

                    glClear(clearMask);

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to clear frame buffer";
                        return false;
                    }
                }

                if (!swapBuffers())
                {
                    return false;
                }
            }
            else for (const DrawCommand& drawCommand : drawCommands)
            {
#if !OUZEL_SUPPORTS_OPENGLES
                setPolygonFillMode(drawCommand.wireframe ? GL_LINE : GL_FILL);
#else
                if (drawCommand.wireframe)
                {
                    continue;
                }
#endif

                // blend state
                BlendStateResourceOGL* blendStateOGL = static_cast<BlendStateResourceOGL*>(drawCommand.blendState);

                if (!blendStateOGL)
                {
                    // don't render if invalid blend state
                    continue;
                }

                if (!setBlendState(blendStateOGL->isGLBlendEnabled(),
                                   blendStateOGL->getModeRGB(),
                                   blendStateOGL->getModeAlpha(),
                                   blendStateOGL->getSourceFactorRGB(),
                                   blendStateOGL->getDestFactorRGB(),
                                   blendStateOGL->getSourceFactorAlpha(),
                                   blendStateOGL->getDestFactorAlpha()))
                {
                    return false;
                }

                if (!setColorMask(blendStateOGL->getRedMask(),
                                  blendStateOGL->getGreenMask(),
                                  blendStateOGL->getBlueMask(),
                                  blendStateOGL->getAlphaMask()))
                {
                    return false;
                }

                // textures
                bool texturesValid = true;

                for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                {
                    TextureResourceOGL* textureOGL = nullptr;

                    if (drawCommand.textures.size() > layer)
                    {
                        textureOGL = static_cast<TextureResourceOGL*>(drawCommand.textures[layer]);
                    }

                    if (textureOGL)
                    {
                        if (!textureOGL->getTextureId())
                        {
                            texturesValid = false;
                            break;
                        }

                        if (!bindTexture(textureOGL->getTextureId(), layer))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (!bindTexture(0, layer))
                        {
                            return false;
                        }
                    }
                }

                if (!texturesValid)
                {
                    continue;
                }

                // shader
                ShaderResourceOGL* shaderOGL = static_cast<ShaderResourceOGL*>(drawCommand.shader);

                if (!shaderOGL || !shaderOGL->getProgramId())
                {
                    // don't render if invalid shader
                    continue;
                }

                useProgram(shaderOGL->getProgramId());

                // pixel shader constants
                const std::vector<ShaderResourceOGL::Location>& pixelShaderConstantLocations = shaderOGL->getPixelShaderConstantLocations();

                if (drawCommand.pixelShaderConstants.size() > pixelShaderConstantLocations.size())
                {
                    Log(Log::Level::ERR) << "Invalid pixel shader constant size";
                    return false;
                }

                for (size_t i = 0; i < drawCommand.pixelShaderConstants.size(); ++i)
                {
                    const ShaderResourceOGL::Location& pixelShaderConstantLocation = pixelShaderConstantLocations[i];
                    const std::vector<float>& pixelShaderConstant = drawCommand.pixelShaderConstants[i];

                    switch (pixelShaderConstantLocation.dataType)
                    {
                        case DataType::INTEGER:
                            glUniform1ivProc(pixelShaderConstantLocation.location, 1, reinterpret_cast<const GLint*>(pixelShaderConstant.data()));
                            break;
                        case DataType::INTEGER_VECTOR2:
                            glUniform2ivProc(pixelShaderConstantLocation.location, 1, reinterpret_cast<const GLint*>(pixelShaderConstant.data()));
                            break;
                        case DataType::INTEGER_VECTOR3:
                            glUniform3ivProc(pixelShaderConstantLocation.location, 1, reinterpret_cast<const GLint*>(pixelShaderConstant.data()));
                            break;
                        case DataType::INTEGER_VECTOR4:
                            glUniform4ivProc(pixelShaderConstantLocation.location, 1, reinterpret_cast<const GLint*>(pixelShaderConstant.data()));
                            break;
                        case DataType::FLOAT:
                            glUniform1fvProc(pixelShaderConstantLocation.location, 1, pixelShaderConstant.data());
                            break;
                        case DataType::FLOAT_VECTOR2:
                            glUniform2fvProc(pixelShaderConstantLocation.location, 1, pixelShaderConstant.data());
                            break;
                        case DataType::FLOAT_VECTOR3:
                            glUniform3fvProc(pixelShaderConstantLocation.location, 1, pixelShaderConstant.data());
                            break;
                        case DataType::FLOAT_VECTOR4:
                            glUniform4fvProc(pixelShaderConstantLocation.location, 1, pixelShaderConstant.data());
                            break;
                        case DataType::FLOAT_MATRIX3:
                            glUniformMatrix3fvProc(pixelShaderConstantLocation.location, 1, GL_FALSE, pixelShaderConstant.data());
                            break;
                        case DataType::FLOAT_MATRIX4:
                            glUniformMatrix4fvProc(pixelShaderConstantLocation.location, 1, GL_FALSE, pixelShaderConstant.data());
                            break;
                        default:
                            Log(Log::Level::ERR) << "Unsupported uniform size";
                            return false;
                    }
                }

                // vertex shader constants
                const std::vector<ShaderResourceOGL::Location>& vertexShaderConstantLocations = shaderOGL->getVertexShaderConstantLocations();

                if (drawCommand.vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                {
                    Log(Log::Level::ERR) << "Invalid vertex shader constant size";
                    return false;
                }

                for (size_t i = 0; i < drawCommand.vertexShaderConstants.size(); ++i)
                {
                    const ShaderResourceOGL::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                    const std::vector<float>& vertexShaderConstant = drawCommand.vertexShaderConstants[i];

                    switch (vertexShaderConstantLocation.dataType)
                    {
                        case DataType::INTEGER:
                            glUniform1ivProc(vertexShaderConstantLocation.location, 1, reinterpret_cast<const GLint*>(vertexShaderConstant.data()));
                            break;
                        case DataType::INTEGER_VECTOR2:
                            glUniform2ivProc(vertexShaderConstantLocation.location, 1, reinterpret_cast<const GLint*>(vertexShaderConstant.data()));
                            break;
                        case DataType::INTEGER_VECTOR3:
                            glUniform3ivProc(vertexShaderConstantLocation.location, 1, reinterpret_cast<const GLint*>(vertexShaderConstant.data()));
                            break;
                        case DataType::INTEGER_VECTOR4:
                            glUniform4ivProc(vertexShaderConstantLocation.location, 1, reinterpret_cast<const GLint*>(vertexShaderConstant.data()));
                            break;
                        case DataType::FLOAT:
                            glUniform1fvProc(vertexShaderConstantLocation.location, 1, vertexShaderConstant.data());
                            break;
                        case DataType::FLOAT_VECTOR2:
                            glUniform2fvProc(vertexShaderConstantLocation.location, 1, vertexShaderConstant.data());
                            break;
                        case DataType::FLOAT_VECTOR3:
                            glUniform3fvProc(vertexShaderConstantLocation.location, 1, vertexShaderConstant.data());
                            break;
                        case DataType::FLOAT_VECTOR4:
                            glUniform4fvProc(vertexShaderConstantLocation.location, 1, vertexShaderConstant.data());
                            break;
                        case DataType::FLOAT_MATRIX3:
                            glUniformMatrix3fvProc(vertexShaderConstantLocation.location, 1, GL_FALSE, vertexShaderConstant.data());
                            break;
                        case DataType::FLOAT_MATRIX4:
                            glUniformMatrix4fvProc(vertexShaderConstantLocation.location, 1, GL_FALSE, vertexShaderConstant.data());
                            break;
                        default:
                            Log(Log::Level::ERR) << "Unsupported uniform size";
                            return false;
                    }
                }

                // render target
                GLuint newFrameBufferId = 0;
                GLbitfield newClearMask = 0;
                const float* newClearColor = frameBufferClearColor;

                if (drawCommand.renderTarget)
                {
                    TextureResourceOGL* renderTargetOGL = static_cast<TextureResourceOGL*>(drawCommand.renderTarget);

                    if (!renderTargetOGL->getFrameBufferId())
                    {
                        continue;
                    }

                    newFrameBufferId = renderTargetOGL->getFrameBufferId();

                    if (renderTargetOGL->getFrameBufferClearedFrame() != currentFrame)
                    {
                        renderTargetOGL->setFrameBufferClearedFrame(currentFrame);
                        newClearMask = renderTargetOGL->getClearMask();
                        newClearColor = renderTargetOGL->getFrameBufferClearColor();
                    }
                }
                else
                {
                    newFrameBufferId = frameBufferId;

                    if (frameBufferClearedFrame != currentFrame)
                    {
                        frameBufferClearedFrame = currentFrame;
                        newClearMask = clearMask;
                        newClearColor = frameBufferClearColor;
                    }
                }

                if (!bindFrameBuffer(newFrameBufferId))
                {
                    return false;
                }

                setViewport(static_cast<GLint>(drawCommand.viewport.position.v[0]),
                            static_cast<GLint>(drawCommand.viewport.position.v[1]),
                            static_cast<GLsizei>(drawCommand.viewport.size.v[0]),
                            static_cast<GLsizei>(drawCommand.viewport.size.v[1]));

                if (newClearMask)
                {
                    if (newClearMask & GL_DEPTH_BUFFER_BIT)
                    {
                        // allow clearing the depth buffer
                        depthMask(true);
#if OUZEL_SUPPORTS_OPENGLES
                        glClearDepthfProc(1.0f);
#else
                        glClearDepthProc(1.0);
#endif
                    }

                    if (newClearMask & GL_COLOR_BUFFER_BIT)
                    {
                        glClearColor(newClearColor[0],
                                     newClearColor[1],
                                     newClearColor[2],
                                     newClearColor[3]);
                    }

                    glClear(newClearMask);

                    if (checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to clear frame buffer";
                        return false;
                    }
                }

                enableDepthTest(drawCommand.depthTest);

                GLint writeMask;
                glGetIntegerv(GL_DEPTH_WRITEMASK, &writeMask);
                depthMask(drawCommand.depthWrite);

                // scissor test
                setScissorTest(drawCommand.scissorTest,
                               static_cast<GLint>(drawCommand.scissorRectangle.position.v[0]),
                               static_cast<GLint>(drawCommand.scissorRectangle.position.v[1]),
                               static_cast<GLsizei>(drawCommand.scissorRectangle.size.v[0]),
                               static_cast<GLsizei>(drawCommand.scissorRectangle.size.v[1]));

                // mesh buffer
                MeshBufferResourceOGL* meshBufferOGL = static_cast<MeshBufferResourceOGL*>(drawCommand.meshBuffer);
                BufferResourceOGL* indexBufferOGL = meshBufferOGL->getIndexBufferOGL();
                BufferResourceOGL* vertexBufferOGL = meshBufferOGL->getVertexBufferOGL();

                if (!meshBufferOGL ||
                    !indexBufferOGL ||
                    !indexBufferOGL->getBufferId() ||
                    !vertexBufferOGL ||
                    !vertexBufferOGL->getBufferId())
                {
                    // don't render if invalid mesh buffer
                    continue;
                }

                // draw
                GLenum mode;

                switch (drawCommand.drawMode)
                {
                    case DrawMode::POINT_LIST: mode = GL_POINTS; break;
                    case DrawMode::LINE_LIST: mode = GL_LINES; break;
                    case DrawMode::LINE_STRIP: mode = GL_LINE_STRIP; break;
                    case DrawMode::TRIANGLE_LIST: mode = GL_TRIANGLES; break;
                    case DrawMode::TRIANGLE_STRIP: mode = GL_TRIANGLE_STRIP; break;
                    default: Log(Log::Level::ERR) << "Invalid draw mode"; return false;
                }

                if (!meshBufferOGL->bindBuffers())
                {
                    return false;
                }

                glDrawElements(mode,
                               static_cast<GLsizei>(drawCommand.indexCount),
                               meshBufferOGL->getIndexType(),
                               static_cast<const char*>(nullptr) + (drawCommand.startIndex * meshBufferOGL->getBytesPerIndex()));

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to draw elements";
                    return false;
                }
            }

            if (!swapBuffers())
            {
                return false;
            }

            return true;
        }

        bool RendererOGL::lockContext()
        {
            return true;
        }

        bool RendererOGL::swapBuffers()
        {
            return true;
        }

        std::vector<Size2> RendererOGL::getSupportedResolutions() const
        {
            return std::vector<Size2>();
        }

        bool RendererOGL::generateScreenshot(const std::string& filename)
        {
            bindFrameBuffer(frameBufferId);

            const GLsizei pixelSize = 4;

            std::vector<uint8_t> data(static_cast<size_t>(frameBufferWidth * frameBufferHeight * pixelSize));

            glReadPixels(0, 0, frameBufferWidth, frameBufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

            if (checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to read pixels from frame buffer";
                return false;
            }

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
            {
                Log(Log::Level::ERR) << "Failed to save image to file";
                return false;
            }
            
            return true;
        }

        BlendStateResource* RendererOGL::createBlendState()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            BlendStateResource* blendState = new BlendStateResourceOGL();
            return blendState;
        }

        TextureResource* RendererOGL::createTexture()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            TextureResource* texture = new TextureResourceOGL();
            return texture;
        }

        ShaderResource* RendererOGL::createShader()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            ShaderResource* shader = new ShaderResourceOGL();
            return shader;
        }

        MeshBufferResource* RendererOGL::createMeshBuffer()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            MeshBufferResource* meshBuffer = new MeshBufferResourceOGL();
            return meshBuffer;
        }

        BufferResource* RendererOGL::createBuffer()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            BufferResource* buffer = new BufferResourceOGL();
            return buffer;
        }

        void* RendererOGL::getProcAddress(const std::string& name) const
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
            {
                return result;
            }
            else
            {
                return nullptr;
            }
#else
            OUZEL_UNUSED(name);
            return nullptr;
#endif
        }

        RendererOGL::StateCache RendererOGL::stateCache;
    } // namespace graphics
} // namespace ouzel

#endif
