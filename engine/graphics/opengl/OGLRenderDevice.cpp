// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if defined(__unix__) || defined(__APPLE__)
#  include <dlfcn.h>
#endif

#include <algorithm>
#include <cassert>

#include "OGL.h"

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
#elif OUZEL_OPENGL_INTERFACE_WGL
#  include "GL/wglext.h"
#endif

#include "OGLRenderDevice.hpp"
#include "OGLError.hpp"
#include "OGLBlendState.hpp"
#include "OGLBuffer.hpp"
#include "OGLDepthStencilState.hpp"
#include "OGLErrorCategory.hpp"
#include "OGLProcedureGetter.hpp"
#include "OGLRenderTarget.hpp"
#include "OGLShader.hpp"
#include "OGLTexture.hpp"
#include "../../core/Engine.hpp"
#include "../../core/Window.hpp"
#include "../../utils/Log.hpp"
#include "stb_image_write.h"

namespace ouzel::graphics::opengl
{
    namespace
    {
        const ErrorCategory errorCategory{};

        constexpr GLenum getIndexType(std::uint32_t indexSize)
        {
            switch (indexSize)
            {
                case 1: return GL_UNSIGNED_BYTE;
                case 2: return GL_UNSIGNED_SHORT;
                case 4: return GL_UNSIGNED_INT; // Supported on OpenGL and OpenGL ES 3 and above
                default: throw Error("Invalid index size");
            }
        }

        constexpr GLenum getVertexType(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::byte:
                case DataType::byteNorm:
                case DataType::byteVector2:
                case DataType::byteVector2Norm:
                case DataType::byteVector3:
                case DataType::byteVector3Norm:
                case DataType::byteVector4:
                case DataType::byteVector4Norm:
                    return GL_BYTE;

                case DataType::unsignedByte:
                case DataType::unsignedByteNorm:
                case DataType::unsignedByteVector2:
                case DataType::unsignedByteVector2Norm:
                case DataType::unsignedByteVector3:
                case DataType::unsignedByteVector3Norm:
                case DataType::unsignedByteVector4:
                case DataType::unsignedByteVector4Norm:
                    return GL_UNSIGNED_BYTE;

                case DataType::integer16:
                case DataType::integer16Norm:
                case DataType::integer16Vector2:
                case DataType::integer16Vector2Norm:
                case DataType::integer16Vector3:
                case DataType::integer16Vector3Norm:
                case DataType::integer16Vector4:
                case DataType::integer16Vector4Norm:
                    return GL_SHORT;

                case DataType::unsignedInteger16:
                case DataType::unsignedInteger16Norm:
                case DataType::unsignedInteger16Vector2:
                case DataType::unsignedInteger16Vector2Norm:
                case DataType::unsignedInteger16Vector3:
                case DataType::unsignedInteger16Vector3Norm:
                case DataType::unsignedInteger16Vector4:
                case DataType::unsignedInteger16Vector4Norm:
                    return GL_UNSIGNED_SHORT;

                case DataType::integer32:
                case DataType::integer32Vector2:
                case DataType::integer32Vector3:
                case DataType::integer32Vector4:
                    return GL_INT;

                case DataType::unsignedInteger32:
                case DataType::unsignedInteger32Vector2:
                case DataType::unsignedInteger32Vector3:
                case DataType::unsignedInteger32Vector4:
                    return GL_UNSIGNED_INT;

                case DataType::float32:
                case DataType::float32Vector2:
                case DataType::float32Vector3:
                case DataType::float32Vector4:
                case DataType::float32Matrix3:
                case DataType::float32Matrix4:
                    return GL_FLOAT;

                default:
                    throw Error("Invalid data type");
            }
        }

        constexpr GLint getArraySize(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::byte:
                case DataType::byteNorm:
                case DataType::unsignedByte:
                case DataType::unsignedByteNorm:
                case DataType::integer16:
                case DataType::integer16Norm:
                case DataType::unsignedInteger16:
                case DataType::unsignedInteger16Norm:
                case DataType::integer32:
                case DataType::unsignedInteger32:
                case DataType::float32:
                    return 1;

                case DataType::byteVector2:
                case DataType::byteVector2Norm:
                case DataType::unsignedByteVector2:
                case DataType::unsignedByteVector2Norm:
                case DataType::integer16Vector2:
                case DataType::integer16Vector2Norm:
                case DataType::unsignedInteger16Vector2:
                case DataType::unsignedInteger16Vector2Norm:
                case DataType::integer32Vector2:
                case DataType::unsignedInteger32Vector2:
                case DataType::float32Vector2:
                    return 2;

                case DataType::byteVector3:
                case DataType::byteVector3Norm:
                case DataType::unsignedByteVector3:
                case DataType::unsignedByteVector3Norm:
                case DataType::integer16Vector3:
                case DataType::integer16Vector3Norm:
                case DataType::unsignedInteger16Vector3:
                case DataType::unsignedInteger16Vector3Norm:
                case DataType::integer32Vector3:
                case DataType::unsignedInteger32Vector3:
                case DataType::float32Vector3:
                    return 3;

                case DataType::byteVector4:
                case DataType::byteVector4Norm:
                case DataType::unsignedByteVector4:
                case DataType::unsignedByteVector4Norm:
                case DataType::integer16Vector4:
                case DataType::integer16Vector4Norm:
                case DataType::unsignedInteger16Vector4:
                case DataType::unsignedInteger16Vector4Norm:
                case DataType::integer32Vector4:
                case DataType::unsignedInteger32Vector4:
                case DataType::float32Vector4:
                    return 4;

                case DataType::float32Matrix3:
                    return 3 * 3;

                case DataType::float32Matrix4:
                    return 4 * 4;

                default:
                    throw Error("Invalid data type");
            }
        }

        constexpr GLboolean isNormalized(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::byteNorm:
                case DataType::byteVector2Norm:
                case DataType::byteVector3Norm:
                case DataType::byteVector4Norm:
                case DataType::unsignedByteNorm:
                case DataType::unsignedByteVector2Norm:
                case DataType::unsignedByteVector3Norm:
                case DataType::unsignedByteVector4Norm:
                case DataType::integer16Norm:
                case DataType::integer16Vector2Norm:
                case DataType::integer16Vector3Norm:
                case DataType::integer16Vector4Norm:
                case DataType::unsignedInteger16Norm:
                case DataType::unsignedInteger16Vector2Norm:
                case DataType::unsignedInteger16Vector3Norm:
                case DataType::unsignedInteger16Vector4Norm:
                    return GL_TRUE;
                default:
                    return GL_FALSE;
            }
        }

        constexpr GLenum getDrawMode(DrawMode drawMode)
        {
            switch (drawMode)
            {
                case DrawMode::pointList: return GL_POINTS;
                case DrawMode::lineList: return GL_LINES;
                case DrawMode::lineStrip: return GL_LINE_STRIP;
                case DrawMode::triangleList: return GL_TRIANGLES;
                case DrawMode::triangleStrip: return GL_TRIANGLE_STRIP;
                default: throw Error("Invalid draw mode");
            }
        }

        constexpr GLenum getCullFace(CullMode cullMode)
        {
            switch (cullMode)
            {
                case CullMode::none: return GL_NONE;
                case CullMode::front: return GL_FRONT;
                case CullMode::back: return GL_BACK;
                default: throw Error("Invalid cull mode");
            }
        }

#if !OUZEL_OPENGLES
        constexpr GLenum getFillMode(FillMode fillMode)
        {
            switch (fillMode)
            {
                case FillMode::solid: return GL_FILL;
                case FillMode::wireframe: return GL_LINE;
                default: throw Error("Invalid fill mode");
            }
        }
#endif
    }

    const std::error_category& getErrorCategory() noexcept
    {
        return errorCategory;
    }

    RenderDevice::RenderDevice(const Settings& settings,
                               core::Window& newWindow,
                               const std::function<void(const Event&)>& initCallback):
        graphics::RenderDevice(Driver::openGL, settings, newWindow, initCallback),
        textureBaseLevelSupported(false),
        textureMaxLevelSupported(false)
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

    void RenderDevice::init(GLsizei newWidth, GLsizei newHeight)
    {
        frameBufferWidth = newWidth;
        frameBufferHeight = newHeight;

        const ProcedureGetter getter(apiVersion);

        glGetStringProc = getter.get<PFNGLGETSTRINGPROC>("glGetString", ApiVersion(1, 0));
        glGetIntegervProc = getter.get<PFNGLGETINTEGERVPROC>("glGetIntegerv", ApiVersion(1, 0));
        glGetErrorProc = getter.get<PFNGLGETERRORPROC>("glGetError", ApiVersion(1, 0));
        glGetStringiProc = getter.get<PFNGLGETSTRINGIPROC>("glGetStringi", ApiVersion(3, 0));

        std::string rendererName;
        const auto rendererNamePointer = glGetStringProc(GL_RENDERER);

        if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
            logger.log(Log::Level::warning) << "Failed to get OpenGL renderer, error: " + std::to_string(error);
        else if (!rendererNamePointer)
            logger.log(Log::Level::warning) << "Failed to get OpenGL renderer";
        else
            rendererName = reinterpret_cast<const char*>(rendererNamePointer);

        std::string vendorName;
        const auto vendorNamePointer = glGetStringProc(GL_VENDOR);

        if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
            logger.log(Log::Level::warning) << "Failed to get OpenGL renderer's vendor, error: " + std::to_string(error);
        else if (!vendorNamePointer)
            logger.log(Log::Level::warning) << "Failed to get OpenGL renderer's vendor";
        else
            vendorName = reinterpret_cast<const char*>(vendorNamePointer);

        logger.log(Log::Level::info) << "Using " << rendererName << " by " << vendorName << " for rendering";

#if OUZEL_OPENGLES
        npotTexturesSupported = apiVersion >= ApiVersion(3, 0) || getter.hasExtension("GL_OES_texture_npot");
        renderTargetsSupported = apiVersion >= ApiVersion(3, 0);
        clampToBorderSupported = apiVersion >= ApiVersion(3, 2) || getter.hasExtension("GL_EXT_texture_border_clamp");
        multisamplingSupported = apiVersion >= ApiVersion(3, 0) ||
            getter.hasExtension("GL_APPLE_framebuffer_multisample") ||
            getter.hasExtension("GL_EXT_multisampled_render_to_texture") ||
            getter.hasExtension("GL_IMG_multisampled_render_to_texture");
        textureBaseLevelSupported = apiVersion >= ApiVersion(3, 0);
        textureMaxLevelSupported = apiVersion >= ApiVersion(3, 0) || getter.hasExtension("GL_APPLE_texture_max_level");
        uintIndicesSupported = apiVersion >= ApiVersion(3, 0) || getter.hasExtension("OES_element_index_uint");
        anisotropicFilteringSupported = getter.hasExtension("GL_EXT_texture_filter_anisotropic");

        glEnableProc = getter.get<PFNGLENABLEPROC>("glEnable", ApiVersion(1, 0));
        glDisableProc = getter.get<PFNGLDISABLEPROC>("glDisable", ApiVersion(1, 0));
        glFrontFaceProc = getter.get<PFNGLFRONTFACEPROC>("glFrontFace", ApiVersion(1, 0));
        glBindTextureProc = getter.get<PFNGLBINDTEXTUREPROC>("glBindTexture", ApiVersion(1, 0));
        glGenTexturesProc = getter.get<PFNGLGENTEXTURESPROC>("glGenTextures", ApiVersion(1, 0));
        glDeleteTexturesProc = getter.get<PFNGLDELETETEXTURESPROC>("glDeleteTextures", ApiVersion(1, 0));
        glTexParameteriProc = getter.get<PFNGLTEXPARAMETERIPROC>("glTexParameteri", ApiVersion(1, 0));
        glTexParameterivProc = getter.get<PFNGLTEXPARAMETERIVPROC>("glTexParameteriv", ApiVersion(1, 0));
        glTexParameterfProc = getter.get<PFNGLTEXPARAMETERFPROC>("glTexParameterf", ApiVersion(1, 0));
        glTexParameterfvProc = getter.get<PFNGLTEXPARAMETERFVPROC>("glTexParameterfv", ApiVersion(1, 0));
        glTexImage2DProc = getter.get<PFNGLTEXIMAGE2DPROC>("glTexImage2D", ApiVersion(1, 0));
        glTexSubImage2DProc = getter.get<PFNGLTEXSUBIMAGE2DPROC>("glTexSubImage2D", ApiVersion(1, 0));
        glViewportProc = getter.get<PFNGLVIEWPORTPROC>("glViewport", ApiVersion(1, 0));
        glClearProc = getter.get<PFNGLCLEARPROC>("glClear", ApiVersion(1, 0));
        glClearColorProc = getter.get<PFNGLCLEARCOLORPROC>("glClearColor", ApiVersion(1, 0));
        glClearStencilProc = getter.get<PFNGLCLEARSTENCILPROC>("glClearStencil", ApiVersion(1, 0));
        glColorMaskProc = getter.get<PFNGLCOLORMASKPROC>("glColorMask", ApiVersion(1, 0));
        glDepthMaskProc = getter.get<PFNGLDEPTHMASKPROC>("glDepthMask", ApiVersion(1, 0));
        glDepthFuncProc = getter.get<PFNGLDEPTHFUNCPROC>("glDepthFunc", ApiVersion(1, 0));
        glStencilMaskProc = getter.get<PFNGLSTENCILMASKPROC>("glStencilMask", ApiVersion(1, 0));
        glStencilFuncSeparateProc = getter.get<PFNGLSTENCILFUNCSEPARATEPROC>("glStencilFuncSeparate", ApiVersion(2, 0));
        glStencilOpSeparateProc = getter.get<PFNGLSTENCILOPSEPARATEPROC>("glStencilOpSeparate", ApiVersion(2, 0));
        glCullFaceProc = getter.get<PFNGLCULLFACEPROC>("glCullFace", ApiVersion(1, 0));
        glScissorProc = getter.get<PFNGLSCISSORPROC>("glScissor", ApiVersion(1, 0));
        glDrawElementsProc = getter.get<PFNGLDRAWELEMENTSPROC>("glDrawElements", ApiVersion(1, 0));
        glReadPixelsProc = getter.get<PFNGLREADPIXELSPROC>("glReadPixels", ApiVersion(1, 0));

        glBlendFuncSeparateProc = getter.get<PFNGLBLENDFUNCSEPARATEPROC>("glBlendFuncSeparate", ApiVersion(2, 0));
        glBlendEquationSeparateProc = getter.get<PFNGLBLENDEQUATIONSEPARATEPROC>("glBlendEquationSeparate", ApiVersion(2, 0));

        glActiveTextureProc = getter.get<PFNGLACTIVETEXTUREPROC>("glActiveTexture", ApiVersion(1, 0));
        glClearDepthfProc = getter.get<PFNGLCLEARDEPTHFPROC>("glClearDepthf", ApiVersion(1, 0));

        glUniform1iProc = getter.get<PFNGLUNIFORM1IPROC>("glUniform1i", ApiVersion(2, 0));
        glUniform1fvProc = getter.get<PFNGLUNIFORM1FVPROC>("glUniform1fv", ApiVersion(2, 0));
        glUniform2fvProc = getter.get<PFNGLUNIFORM2FVPROC>("glUniform2fv", ApiVersion(2, 0));
        glUniform3fvProc = getter.get<PFNGLUNIFORM3FVPROC>("glUniform3fv", ApiVersion(2, 0));
        glUniform4fvProc = getter.get<PFNGLUNIFORM4FVPROC>("glUniform4fv", ApiVersion(2, 0));
        glUniform1ivProc = getter.get<PFNGLUNIFORM1IVPROC>("glUniform1iv", ApiVersion(2, 0));
        glUniform2ivProc = getter.get<PFNGLUNIFORM2IVPROC>("glUniform2iv", ApiVersion(2, 0));
        glUniform3ivProc = getter.get<PFNGLUNIFORM3IVPROC>("glUniform3iv", ApiVersion(2, 0));
        glUniform4ivProc = getter.get<PFNGLUNIFORM4IVPROC>("glUniform4iv", ApiVersion(2, 0));
        glUniformMatrix3fvProc = getter.get<PFNGLUNIFORMMATRIX3FVPROC>("glUniformMatrix3fv", ApiVersion(2, 0));
        glUniformMatrix4fvProc = getter.get<PFNGLUNIFORMMATRIX4FVPROC>("glUniformMatrix4fv", ApiVersion(2, 0));

        glCreateShaderProc = getter.get<PFNGLCREATESHADERPROC>("glCreateShader", ApiVersion(2, 0));
        glDeleteShaderProc = getter.get<PFNGLDELETESHADERPROC>("glDeleteShader", ApiVersion(2, 0));
        glAttachShaderProc = getter.get<PFNGLATTACHSHADERPROC>("glAttachShader", ApiVersion(2, 0));
        glDetachShaderProc = getter.get<PFNGLDETACHSHADERPROC>("glDetachShader", ApiVersion(2, 0));
        glShaderSourceProc = getter.get<PFNGLSHADERSOURCEPROC>("glShaderSource", ApiVersion(2, 0));
        glCompileShaderProc = getter.get<PFNGLCOMPILESHADERPROC>("glCompileShader", ApiVersion(2, 0));
        glBindAttribLocationProc = getter.get<PFNGLBINDATTRIBLOCATIONPROC>("glBindAttribLocation", ApiVersion(2, 0));
        glGetShaderivProc = getter.get<PFNGLGETSHADERIVPROC>("glGetShaderiv", ApiVersion(2, 0));
        glGetShaderInfoLogProc = getter.get<PFNGLGETSHADERINFOLOGPROC>("glGetShaderInfoLog", ApiVersion(2, 0));

        glCreateProgramProc = getter.get<PFNGLCREATEPROGRAMPROC>("glCreateProgram", ApiVersion(2, 0));
        glDeleteProgramProc = getter.get<PFNGLDELETEPROGRAMPROC>("glDeleteProgram", ApiVersion(2, 0));
        glUseProgramProc = getter.get<PFNGLUSEPROGRAMPROC>("glUseProgram", ApiVersion(2, 0));
        glLinkProgramProc = getter.get<PFNGLLINKPROGRAMPROC>("glLinkProgram", ApiVersion(2, 0));
        glGetProgramivProc = getter.get<PFNGLGETPROGRAMIVPROC>("glGetProgramiv", ApiVersion(2, 0));
        glGetProgramInfoLogProc = getter.get<PFNGLGETPROGRAMINFOLOGPROC>("glGetProgramInfoLog", ApiVersion(2, 0));
        glGetUniformLocationProc = getter.get<PFNGLGETUNIFORMLOCATIONPROC>("glGetUniformLocation", ApiVersion(2, 0));

        glBindBufferProc = getter.get<PFNGLBINDBUFFERPROC>("glBindBuffer", ApiVersion(1, 1));
        glDeleteBuffersProc = getter.get<PFNGLDELETEBUFFERSPROC>("glDeleteBuffers", ApiVersion(1, 1));
        glGenBuffersProc = getter.get<PFNGLGENBUFFERSPROC>("glGenBuffers", ApiVersion(1, 1));
        glBufferDataProc = getter.get<PFNGLBUFFERDATAPROC>("glBufferData", ApiVersion(1, 1));
        glBufferSubDataProc = getter.get<PFNGLBUFFERSUBDATAPROC>("glBufferSubData", ApiVersion(1, 1));

        glEnableVertexAttribArrayProc = getter.get<PFNGLENABLEVERTEXATTRIBARRAYPROC>("glEnableVertexAttribArray", ApiVersion(2, 0));
        glDisableVertexAttribArrayProc = getter.get<PFNGLDISABLEVERTEXATTRIBARRAYPROC>("glDisableVertexAttribArray", ApiVersion(2, 0));
        glVertexAttribPointerProc = getter.get<PFNGLVERTEXATTRIBPOINTERPROC>("glVertexAttribPointer", ApiVersion(2, 0));

        glGenFramebuffersProc = getter.get<PFNGLGENFRAMEBUFFERSPROC>("glGenFramebuffers", ApiVersion(2, 0));
        glDeleteFramebuffersProc = getter.get<PFNGLDELETEFRAMEBUFFERSPROC>("glDeleteFramebuffers", ApiVersion(2, 0));
        glBindFramebufferProc = getter.get<PFNGLBINDFRAMEBUFFERPROC>("glBindFramebuffer", ApiVersion(2, 0));
        glCheckFramebufferStatusProc = getter.get<PFNGLCHECKFRAMEBUFFERSTATUSPROC>("glCheckFramebufferStatus", ApiVersion(2, 0));
        glFramebufferRenderbufferProc = getter.get<PFNGLFRAMEBUFFERRENDERBUFFERPROC>("glFramebufferRenderbuffer", ApiVersion(2, 0));
        glFramebufferTexture2DProc = getter.get<PFNGLFRAMEBUFFERTEXTURE2DPROC>("glFramebufferTexture2D", ApiVersion(2, 0));

        glBlitFramebufferProc = getter.get<PFNGLBLITFRAMEBUFFERPROC>("glBlitFramebuffer", ApiVersion(3, 0));

        glGenRenderbuffersProc = getter.get<PFNGLGENRENDERBUFFERSPROC>("glGenRenderbuffers", ApiVersion(2, 0));
        glDeleteRenderbuffersProc = getter.get<PFNGLDELETERENDERBUFFERSPROC>("glDeleteRenderbuffers", ApiVersion(2, 0));
        glBindRenderbufferProc = getter.get<PFNGLBINDRENDERBUFFERPROC>("glBindRenderbuffer", ApiVersion(2, 0));
        glRenderbufferStorageProc = getter.get<PFNGLRENDERBUFFERSTORAGEPROC>("glRenderbufferStorage", ApiVersion(2, 0));

        glTexStorage2DMultisampleProc = getter.get<PFNGLTEXSTORAGE2DMULTISAMPLEPROC>("glTexStorage2DMultisample", ApiVersion(3, 1),
                                                                                     {{"glTexStorage2DMultisampleEXT", "GL_EXT_multisampled_render_to_texture"}});

        glUniform1uivProc = getter.get<PFNGLUNIFORM1UIVPROC>("glUniform1uiv", ApiVersion(3, 0));
        glUniform2uivProc = getter.get<PFNGLUNIFORM2UIVPROC>("glUniform2uiv", ApiVersion(3, 0));
        glUniform3uivProc = getter.get<PFNGLUNIFORM3UIVPROC>("glUniform3uiv", ApiVersion(3, 0));
        glUniform4uivProc = getter.get<PFNGLUNIFORM4UIVPROC>("glUniform4uiv", ApiVersion(3, 0));

        glMapBufferRangeProc = getter.get<PFNGLMAPBUFFERRANGEPROC>("glMapBufferRange", ApiVersion(3, 0),
                                                                   {{"glMapBufferRangeEXT", "GL_EXT_map_buffer_range"}});
        glUnmapBufferProc = getter.get<PFNGLUNMAPBUFFERPROC>("glUnmapBuffer", ApiVersion(3, 0),
                                                             {{"glUnmapBufferOES", "GL_OES_mapbuffer"}});

        glGenVertexArraysProc = getter.get<PFNGLGENVERTEXARRAYSPROC>("glGenVertexArrays", ApiVersion(3, 0),
                                                                     {{"glGenVertexArraysOES", "GL_OES_vertex_array_object"}});
        glBindVertexArrayProc = getter.get<PFNGLBINDVERTEXARRAYPROC>("glBindVertexArray", ApiVersion(3, 0),
                                                                     {{"glBindVertexArrayOES", "GL_OES_vertex_array_object"}});
        glDeleteVertexArraysProc = getter.get<PFNGLDELETEVERTEXARRAYSPROC>("glDeleteVertexArrays", ApiVersion(3, 0),
                                                                           {{"glDeleteVertexArraysOES", "GL_OES_vertex_array_object"}});

        glPushGroupMarkerEXTProc = getter.get<PFNGLPUSHGROUPMARKEREXTPROC>("glPushGroupMarkerEXT", "GL_EXT_debug_marker");
        glPopGroupMarkerEXTProc = getter.get<PFNGLPOPGROUPMARKEREXTPROC>("glPopGroupMarkerEXT", "GL_EXT_debug_marker");

        glCopyImageSubDataProc = getter.get<PFNGLCOPYIMAGESUBDATAPROC>("glCopyImageSubData", ApiVersion(3, 2));

        glRenderbufferStorageMultisampleProc = getter.get<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>("glRenderbufferStorageMultisample", ApiVersion(3, 0),
                                                                                                   {{"glRenderbufferStorageMultisampleEXT", "GL_EXT_multisampled_render_to_texture"},
                                                                                                    {"glRenderbufferStorageMultisampleIMG", "GL_IMG_multisampled_render_to_texture"},
                                                                                                    {"glRenderbufferStorageMultisampleAPPLE", "GL_APPLE_framebuffer_multisample"}});

        glFramebufferTexture2DMultisampleProc = getter.get<PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC>({{"glFramebufferTexture2DMultisample", "GL_EXT_multisampled_render_to_texture"},
                                                                                                         {"glFramebufferTexture2DMultisampleIMG", "GL_IMG_multisampled_render_to_texture"}});

#  if OUZEL_OPENGL_INTERFACE_EAGL
        glDiscardFramebufferEXTProc = getter.get<PFNGLDISCARDFRAMEBUFFEREXTPROC>("glDiscardFramebufferEXT", ApiVersion(2, 0));
        glResolveMultisampleFramebufferAPPLEProc = getter.get<PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC>("glResolveMultisampleFramebufferAPPLE", ApiVersion(2, 0));

        glMapBufferProc = getter.get<PFNGLMAPBUFFEROESPROC>("glMapBufferOES", ApiVersion(3, 0),
                                                            {{"glMapBufferOES", "GL_OES_mapbuffer"}});

        if (getter.hasExtension("GL_APPLE_framebuffer_multisample")) multisamplingSupported = true;
#  else
        glMapBufferProc = getter.get<PFNGLMAPBUFFEROESPROC>("glMapBuffer", ApiVersion(3, 0),
                                                            {{"glMapBufferOES", "GL_OES_mapbuffer"}});
#  endif
#else // OUZEL_OPENGLES
        clampToBorderSupported = apiVersion >= ApiVersion(1, 3) || getter.hasExtension("GL_EXT_texture_mirror_clamp");
        textureBaseLevelSupported = apiVersion >= ApiVersion(1, 3);
        textureMaxLevelSupported = apiVersion >= ApiVersion(1, 3);
        uintIndicesSupported = apiVersion >= ApiVersion(2, 0);
        npotTexturesSupported = apiVersion >= ApiVersion(2, 0);
        renderTargetsSupported = apiVersion >= ApiVersion(3, 0) ||
            getter.hasExtension("GL_ARB_framebuffer_object") ||
            getter.hasExtension("GL_EXT_framebuffer_object");
        multisamplingSupported = apiVersion >= ApiVersion(3, 0);
        anisotropicFilteringSupported = apiVersion >= ApiVersion(4, 6) ||
            getter.hasExtension("GL_EXT_texture_filter_anisotropic") ||
            getter.hasExtension("GL_ARB_texture_filter_anisotropic");

        glEnableProc = getter.get<PFNGLENABLEPROC>("glEnable", ApiVersion(1, 0));
        glDisableProc = getter.get<PFNGLDISABLEPROC>("glDisable", ApiVersion(1, 0));
        glFrontFaceProc = getter.get<PFNGLFRONTFACEPROC>("glFrontFace", ApiVersion(1, 0));
        glBindTextureProc = getter.get<PFNGLBINDTEXTUREPROC>("glBindTexture", ApiVersion(1, 1));
        glGenTexturesProc = getter.get<PFNGLGENTEXTURESPROC>("glGenTextures", ApiVersion(1, 1));
        glDeleteTexturesProc = getter.get<PFNGLDELETETEXTURESPROC>("glDeleteTextures", ApiVersion(1, 1));
        glTexParameteriProc = getter.get<PFNGLTEXPARAMETERIPROC>("glTexParameteri", ApiVersion(1, 0));
        glTexParameterivProc = getter.get<PFNGLTEXPARAMETERIVPROC>("glTexParameteriv", ApiVersion(1, 0));
        glTexParameterfProc = getter.get<PFNGLTEXPARAMETERFPROC>("glTexParameterf", ApiVersion(1, 0));
        glTexParameterfvProc = getter.get<PFNGLTEXPARAMETERFVPROC>("glTexParameterfv", ApiVersion(1, 0));
        glTexImage2DProc = getter.get<PFNGLTEXIMAGE2DPROC>("glTexImage2D", ApiVersion(1, 0));
        glTexSubImage2DProc = getter.get<PFNGLTEXSUBIMAGE2DPROC>("glTexSubImage2D", ApiVersion(1, 1));
        glViewportProc = getter.get<PFNGLVIEWPORTPROC>("glViewport", ApiVersion(1, 0));
        glClearProc = getter.get<PFNGLCLEARPROC>("glClear", ApiVersion(1, 0));
        glClearColorProc = getter.get<PFNGLCLEARCOLORPROC>("glClearColor", ApiVersion(1, 0));
        glClearStencilProc = getter.get<PFNGLCLEARSTENCILPROC>("glClearStencil", ApiVersion(1, 0));
        glColorMaskProc = getter.get<PFNGLCOLORMASKPROC>("glColorMask", ApiVersion(1, 0));
        glDepthMaskProc = getter.get<PFNGLDEPTHMASKPROC>("glDepthMask", ApiVersion(1, 0));
        glDepthFuncProc = getter.get<PFNGLDEPTHFUNCPROC>("glDepthFunc", ApiVersion(1, 0));
        glStencilMaskProc = getter.get<PFNGLSTENCILMASKPROC>("glStencilMask", ApiVersion(1, 0));
        glStencilFuncSeparateProc = getter.get<PFNGLSTENCILFUNCSEPARATEPROC>("glStencilFuncSeparate", ApiVersion(2, 0));
        glStencilOpSeparateProc = getter.get<PFNGLSTENCILOPSEPARATEPROC>("glStencilOpSeparate", ApiVersion(2, 0));
        glCullFaceProc = getter.get<PFNGLCULLFACEPROC>("glCullFace", ApiVersion(1, 0));
        glScissorProc = getter.get<PFNGLSCISSORPROC>("glScissor", ApiVersion(1, 0));
        glDrawElementsProc = getter.get<PFNGLDRAWELEMENTSPROC>("glDrawElements", ApiVersion(1, 1));
        glReadPixelsProc = getter.get<PFNGLREADPIXELSPROC>("glReadPixels", ApiVersion(1, 0));

        glBlendFuncSeparateProc = getter.get<PFNGLBLENDFUNCSEPARATEPROC>("glBlendFuncSeparate", ApiVersion(2, 0));
        glBlendEquationSeparateProc = getter.get<PFNGLBLENDEQUATIONSEPARATEPROC>("glBlendEquationSeparate", ApiVersion(2, 0));

        glActiveTextureProc = getter.get<PFNGLACTIVETEXTUREPROC>("glActiveTexture", ApiVersion(2, 0));

        glPolygonModeProc = getter.get<PFNGLPOLYGONMODEPROC>("glPolygonMode", ApiVersion(1, 0));
        glClearDepthProc = getter.get<PFNGLCLEARDEPTHPROC>("glClearDepth", ApiVersion(1, 0));

        glUniform1iProc = getter.get<PFNGLUNIFORM1IPROC>("glUniform1i", ApiVersion(2, 0));
        glUniform1fvProc = getter.get<PFNGLUNIFORM1FVPROC>("glUniform1fv", ApiVersion(2, 0));
        glUniform2fvProc = getter.get<PFNGLUNIFORM2FVPROC>("glUniform2fv", ApiVersion(2, 0));
        glUniform3fvProc = getter.get<PFNGLUNIFORM3FVPROC>("glUniform3fv", ApiVersion(2, 0));
        glUniform4fvProc = getter.get<PFNGLUNIFORM4FVPROC>("glUniform4fv", ApiVersion(2, 0));
        glUniform1ivProc = getter.get<PFNGLUNIFORM1IVPROC>("glUniform1iv", ApiVersion(2, 0));
        glUniform2ivProc = getter.get<PFNGLUNIFORM2IVPROC>("glUniform2iv", ApiVersion(2, 0));
        glUniform3ivProc = getter.get<PFNGLUNIFORM3IVPROC>("glUniform3iv", ApiVersion(2, 0));
        glUniform4ivProc = getter.get<PFNGLUNIFORM4IVPROC>("glUniform4iv", ApiVersion(2, 0));
        glUniformMatrix3fvProc = getter.get<PFNGLUNIFORMMATRIX3FVPROC>("glUniformMatrix3fv", ApiVersion(2, 0));
        glUniformMatrix4fvProc = getter.get<PFNGLUNIFORMMATRIX4FVPROC>("glUniformMatrix4fv", ApiVersion(2, 0));

        glCreateShaderProc = getter.get<PFNGLCREATESHADERPROC>("glCreateShader", ApiVersion(2, 0));
        glDeleteShaderProc = getter.get<PFNGLDELETESHADERPROC>("glDeleteShader", ApiVersion(2, 0));
        glAttachShaderProc = getter.get<PFNGLATTACHSHADERPROC>("glAttachShader", ApiVersion(2, 0));
        glDetachShaderProc = getter.get<PFNGLDETACHSHADERPROC>("glDetachShader", ApiVersion(2, 0));
        glShaderSourceProc = getter.get<PFNGLSHADERSOURCEPROC>("glShaderSource", ApiVersion(2, 0));
        glCompileShaderProc = getter.get<PFNGLCOMPILESHADERPROC>("glCompileShader", ApiVersion(2, 0));
        glBindAttribLocationProc = getter.get<PFNGLBINDATTRIBLOCATIONPROC>("glBindAttribLocation", ApiVersion(2, 0));
        glGetShaderivProc = getter.get<PFNGLGETSHADERIVPROC>("glGetShaderiv", ApiVersion(2, 0));
        glGetShaderInfoLogProc = getter.get<PFNGLGETSHADERINFOLOGPROC>("glGetShaderInfoLog", ApiVersion(2, 0));

        glCreateProgramProc = getter.get<PFNGLCREATEPROGRAMPROC>("glCreateProgram", ApiVersion(2, 0));
        glDeleteProgramProc = getter.get<PFNGLDELETEPROGRAMPROC>("glDeleteProgram", ApiVersion(2, 0));
        glUseProgramProc = getter.get<PFNGLUSEPROGRAMPROC>("glUseProgram", ApiVersion(2, 0));
        glLinkProgramProc = getter.get<PFNGLLINKPROGRAMPROC>("glLinkProgram", ApiVersion(2, 0));
        glGetProgramivProc = getter.get<PFNGLGETPROGRAMIVPROC>("glGetProgramiv", ApiVersion(2, 0));
        glGetProgramInfoLogProc = getter.get<PFNGLGETPROGRAMINFOLOGPROC>("glGetProgramInfoLog", ApiVersion(2, 0));
        glGetUniformLocationProc = getter.get<PFNGLGETUNIFORMLOCATIONPROC>("glGetUniformLocation", ApiVersion(2, 0));

        glBindBufferProc = getter.get<PFNGLBINDBUFFERPROC>("glBindBuffer", ApiVersion(2, 0));
        glDeleteBuffersProc = getter.get<PFNGLDELETEBUFFERSPROC>("glDeleteBuffers", ApiVersion(2, 0));
        glGenBuffersProc = getter.get<PFNGLGENBUFFERSPROC>("glGenBuffers", ApiVersion(2, 0));
        glBufferDataProc = getter.get<PFNGLBUFFERDATAPROC>("glBufferData", ApiVersion(2, 0));
        glBufferSubDataProc = getter.get<PFNGLBUFFERSUBDATAPROC>("glBufferSubData", ApiVersion(2, 0));

        glEnableVertexAttribArrayProc = getter.get<PFNGLENABLEVERTEXATTRIBARRAYPROC>("glEnableVertexAttribArray", ApiVersion(2, 0));
        glDisableVertexAttribArrayProc = getter.get<PFNGLDISABLEVERTEXATTRIBARRAYPROC>("glDisableVertexAttribArray", ApiVersion(2, 0));
        glVertexAttribPointerProc = getter.get<PFNGLVERTEXATTRIBPOINTERPROC>("glVertexAttribPointer", ApiVersion(2, 0));

        glMapBufferProc = getter.get<PFNGLMAPBUFFERPROC>("glMapBuffer", ApiVersion(2, 0));
        glUnmapBufferProc = getter.get<PFNGLUNMAPBUFFERPROC>("glUnmapBuffer", ApiVersion(2, 0));

        glUniform1uivProc = getter.get<PFNGLUNIFORM1UIVPROC>("glUniform1uiv", ApiVersion(3, 0));
        glUniform2uivProc = getter.get<PFNGLUNIFORM2UIVPROC>("glUniform2uiv", ApiVersion(3, 0));
        glUniform3uivProc = getter.get<PFNGLUNIFORM3UIVPROC>("glUniform3uiv", ApiVersion(3, 0));
        glUniform4uivProc = getter.get<PFNGLUNIFORM4UIVPROC>("glUniform4uiv", ApiVersion(3, 0));

        glMapBufferRangeProc = getter.get<PFNGLMAPBUFFERRANGEPROC>("glMapBufferRange", ApiVersion(3, 0),
                                                                   {{"glMapBufferRange", "GL_ARB_map_buffer_range"}});

        glGenVertexArraysProc = getter.get<PFNGLGENVERTEXARRAYSPROC>("glGenVertexArrays", ApiVersion(3, 0),
                                                                     {{"glGenVertexArrays", "GL_ARB_vertex_array_object"}});
        glBindVertexArrayProc = getter.get<PFNGLBINDVERTEXARRAYPROC>("glBindVertexArray", ApiVersion(3, 0),
                                                                     {{"glBindVertexArray", "GL_ARB_vertex_array_object"}});
        glDeleteVertexArraysProc = getter.get<PFNGLDELETEVERTEXARRAYSPROC>("glDeleteVertexArrays", ApiVersion(3, 0),
                                                                           {{"glDeleteVertexArrays", "GL_ARB_vertex_array_object"}});

        glGenFramebuffersProc = getter.get<PFNGLGENFRAMEBUFFERSPROC>("glGenFramebuffers", ApiVersion(3, 0),
                                                                     {{"glGenFramebuffers", "GL_ARB_framebuffer_object"},
                                                                      {"glGenFramebuffersEXT", "GL_EXT_framebuffer_object"}});
        glDeleteFramebuffersProc = getter.get<PFNGLDELETEFRAMEBUFFERSPROC>("glDeleteFramebuffers", ApiVersion(3, 0),
                                                                           {{"glDeleteFramebuffers", "GL_ARB_framebuffer_object"},
                                                                            {"glDeleteFramebuffersEXT", "GL_EXT_framebuffer_object"}});
        glBindFramebufferProc = getter.get<PFNGLBINDFRAMEBUFFERPROC>("glBindFramebuffer", ApiVersion(3, 0),
                                                                     {{"glBindFramebuffer", "GL_ARB_framebuffer_object"},
                                                                      {"glBindFramebufferEXT", "GL_EXT_framebuffer_object"}});
        glCheckFramebufferStatusProc = getter.get<PFNGLCHECKFRAMEBUFFERSTATUSPROC>("glCheckFramebufferStatus", ApiVersion(3, 0),
                                                                                   {{"glCheckFramebufferStatus", "GL_ARB_framebuffer_object"},
                                                                                    {"CheckFramebufferStatusEXT", "GL_EXT_framebuffer_object"}});
        glFramebufferRenderbufferProc = getter.get<PFNGLFRAMEBUFFERRENDERBUFFERPROC>("glFramebufferRenderbuffer", ApiVersion(3, 0),
                                                                                     {{"glFramebufferRenderbuffer", "GL_ARB_framebuffer_object"},
                                                                                      {"glFramebufferRenderbufferEXT", "GL_EXT_framebuffer_object"}});
        glBlitFramebufferProc = getter.get<PFNGLBLITFRAMEBUFFERPROC>("glBlitFramebuffer", ApiVersion(3, 0),
                                                                     {{"glBlitFramebuffer", "GL_ARB_framebuffer_object"},
                                                                      {"glBlitFramebufferEXT", "GL_EXT_framebuffer_blit"}});
        glFramebufferTexture2DProc = getter.get<PFNGLFRAMEBUFFERTEXTURE2DPROC>("glFramebufferTexture2D", ApiVersion(3, 0),
                                                                               {{"glFramebufferTexture2D", "GL_ARB_framebuffer_object"},
                                                                                {"glFramebufferTexture2DEXT", "GL_EXT_framebuffer_object"}});
        glGenRenderbuffersProc = getter.get<PFNGLGENRENDERBUFFERSPROC>("glGenRenderbuffers", ApiVersion(3, 0),
                                                                       {{"glGenRenderbuffers", "GL_ARB_framebuffer_object"},
                                                                        {"glGenRenderbuffersEXT", "GL_EXT_framebuffer_object"}});
        glDeleteRenderbuffersProc = getter.get<PFNGLDELETERENDERBUFFERSPROC>("glDeleteRenderbuffers", ApiVersion(3, 0),
                                                                             {{"glDeleteRenderbuffers", "GL_ARB_framebuffer_object"},
                                                                              {"glDeleteRenderbuffersEXT", "GL_EXT_framebuffer_object"}});
        glBindRenderbufferProc = getter.get<PFNGLBINDRENDERBUFFERPROC>("glBindRenderbuffer", ApiVersion(3, 0),
                                                                       {{"glBindRenderbuffer", "GL_ARB_framebuffer_object"},
                                                                        {"glBindRenderbufferEXT", "GL_EXT_framebuffer_object"}});
        glRenderbufferStorageProc = getter.get<PFNGLRENDERBUFFERSTORAGEPROC>("glRenderbufferStorage", ApiVersion(3, 0),
                                                                             {{"glRenderbufferStorage", "GL_ARB_framebuffer_object"},
                                                                              {"glRenderbufferStorageEXT", "GL_EXT_framebuffer_object"}});

        glRenderbufferStorageMultisampleProc = getter.get<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>("glRenderbufferStorageMultisample", ApiVersion(3, 0),
                                                                                                   {{"glRenderbufferStorageMultisampleEXT", "GL_EXT_multisampled_render_to_texture"}});

        glTexStorage2DMultisampleProc = getter.get<PFNGLTEXSTORAGE2DMULTISAMPLEPROC>("glTexStorage2DMultisample", ApiVersion(4, 3),
                                                                                     {{"glTexStorage2DMultisample", "GL_ARB_texture_storage_multisample"}});

        glGenVertexArraysProc = getter.get<PFNGLGENVERTEXARRAYSPROC>("glGenVertexArrays", ApiVersion(3, 0),
                                                                     {{"glGenVertexArrays", "GL_ARB_vertex_array_object"}});
        glBindVertexArrayProc = getter.get<PFNGLBINDVERTEXARRAYPROC>("glBindVertexArray", ApiVersion(3, 0),
                                                                     {{"glBindVertexArray", "GL_ARB_vertex_array_object"}});
        glDeleteVertexArraysProc = getter.get<PFNGLDELETEVERTEXARRAYSPROC>("glDeleteVertexArrays", ApiVersion(3, 0),
                                                                           {{"glDeleteVertexArrays", "GL_ARB_vertex_array_object"}});

        glCopyImageSubDataProc = getter.get<PFNGLCOPYIMAGESUBDATAPROC>("glCopyImageSubData", ApiVersion(4, 3),
                                                                       {{"glCopyImageSubData", "GL_ARB_copy_image"}, {"glCopyImageSubDataEXT", "GL_EXT_copy_image"}});

        glPushGroupMarkerEXTProc = getter.get<PFNGLPUSHGROUPMARKEREXTPROC>("glPushGroupMarkerEXT", "GL_EXT_debug_marker");
        glPopGroupMarkerEXTProc = getter.get<PFNGLPOPGROUPMARKEREXTPROC>("glPopGroupMarkerEXT", "GL_EXT_debug_marker");
#endif

        if (!multisamplingSupported) sampleCount = 1;

        glDisableProc(GL_DITHER);

        if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to set depth function");

#if !OUZEL_OPENGLES
        if (srgb)
            glEnableProc(GL_FRAMEBUFFER_SRGB);

        if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to enable sRGB frame buffer");

        if (sampleCount > 1)
        {
            glEnableProc(GL_MULTISAMPLE);

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to enable multi-sampling");
        }
#endif

        if (glGenVertexArraysProc)
        {
            glGenVertexArraysProc(1, &vertexArrayId);

            glBindVertexArrayProc(vertexArrayId);

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to bind vertex array");
        }

        setFrontFace(GL_CW);
    }

    void RenderDevice::setUniform(GLint location, DataType dataType, const void* data)
    {
        switch (dataType)
        {
            case DataType::integer32:
                glUniform1ivProc(location, 1, static_cast<const GLint*>(data));
                break;
            case DataType::unsignedInteger32:
                if (!glUniform1uivProc) throw Error("Unsupported uniform size");
                glUniform1uivProc(location, 1, static_cast<const GLuint*>(data));
                break;
            case DataType::integer32Vector2:
                glUniform2ivProc(location, 1, static_cast<const GLint*>(data));
                break;
            case DataType::unsignedInteger32Vector2:
                if (!glUniform2uivProc) throw Error("Unsupported uniform size");
                glUniform2uivProc(location, 1, static_cast<const GLuint*>(data));
                break;
            case DataType::integer32Vector3:
                glUniform3ivProc(location, 1, static_cast<const GLint*>(data));
                break;
            case DataType::unsignedInteger32Vector3:
                if (!glUniform3uivProc) throw Error("Unsupported uniform size");
                glUniform3uivProc(location, 1, static_cast<const GLuint*>(data));
                break;
            case DataType::integer32Vector4:
                glUniform4ivProc(location, 1, static_cast<const GLint*>(data));
                break;
            case DataType::unsignedInteger32Vector4:
                if (!glUniform4uivProc) throw Error("Unsupported uniform size");
                glUniform4uivProc(location, 1, static_cast<const GLuint*>(data));
                break;
            case DataType::float32:
                glUniform1fvProc(location, 1, static_cast<const GLfloat*>(data));
                break;
            case DataType::float32Vector2:
                glUniform2fvProc(location, 1, static_cast<const GLfloat*>(data));
                break;
            case DataType::float32Vector3:
                glUniform3fvProc(location, 1, static_cast<const GLfloat*>(data));
                break;
            case DataType::float32Vector4:
                glUniform4fvProc(location, 1, static_cast<const GLfloat*>(data));
                break;
            case DataType::float32Matrix3:
                glUniformMatrix3fvProc(location, 1, GL_FALSE, static_cast<const GLfloat*>(data));
                break;
            case DataType::float32Matrix4:
                glUniformMatrix4fvProc(location, 1, GL_FALSE, static_cast<const GLfloat*>(data));
                break;
            default:
                throw Error("Unsupported uniform size");
        }
    }

    void RenderDevice::process()
    {
        graphics::RenderDevice::process();
        executeAll();

        const RenderTarget* currentRenderTarget = nullptr;
        const Shader* currentShader = nullptr;

        CommandBuffer commandBuffer;
        std::unique_ptr<Command> command;

        for (;;)
        {
            std::unique_lock lock(commandQueueMutex);
            while (commandQueue.empty()) commandQueueCondition.wait(lock);
            commandBuffer = std::move(commandQueue.front());
            commandQueue.pop();
            lock.unlock();

            while (!commandBuffer.isEmpty())
            {
                command = commandBuffer.popCommand();

                switch (command->type)
                {
                    case Command::Type::resize:
                    {
                        auto resizeCommand = static_cast<const ResizeCommand*>(command.get());
                        frameBufferWidth = static_cast<GLsizei>(resizeCommand->size.v[0]);
                        frameBufferHeight = static_cast<GLsizei>(resizeCommand->size.v[1]);
                        resizeFrameBuffer();
                        break;
                    }

                    case Command::Type::present:
                    {
                        present();
                        break;
                    }

                    case Command::Type::deleteResource:
                    {
                        auto deleteResourceCommand = static_cast<const DeleteResourceCommand*>(command.get());
                        resources[deleteResourceCommand->resource - 1].reset();
                        break;
                    }

                    case Command::Type::initRenderTarget:
                    {
                        auto initRenderTargetCommand = static_cast<const InitRenderTargetCommand*>(command.get());

                        std::set<Texture*> colorTextures;
                        for (const auto colorTextureId : initRenderTargetCommand->colorTextures)
                            colorTextures.insert(getResource<Texture>(colorTextureId));

                        auto renderTarget = std::make_unique<RenderTarget>(*this,
                                                                           colorTextures,
                                                                           getResource<Texture>(initRenderTargetCommand->depthTexture));

                        if (initRenderTargetCommand->renderTarget > resources.size())
                            resources.resize(initRenderTargetCommand->renderTarget);
                        resources[initRenderTargetCommand->renderTarget - 1] = std::move(renderTarget);
                        break;
                    }

                    case Command::Type::setRenderTarget:
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

                    case Command::Type::clearRenderTarget:
                    {
                        auto clearCommand = static_cast<const ClearRenderTargetCommand*>(command.get());

                        const GLbitfield clearMask = (clearCommand->clearColorBuffer ? GL_COLOR_BUFFER_BIT : 0) |
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

                            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                                throw std::system_error(makeErrorCode(error), "Failed to clear frame buffer");
                        }

                        break;
                    }

                    case Command::Type::blit:
                    {
#if !OUZEL_OPENGLES
                        auto blitCommand = static_cast<const BlitCommand*>(command.get());

                        auto sourceTexture = getResource<Texture>(blitCommand->sourceTexture);
                        auto destinationTexture = getResource<Texture>(blitCommand->destinationTexture);

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

                    case Command::Type::setScissorTest:
                    {
                        auto setScissorTestCommand = static_cast<const SetScissorTestCommand*>(command.get());

                        setScissorTest(setScissorTestCommand->enabled,
                                       static_cast<GLint>(setScissorTestCommand->rectangle.position.v[0]),
                                       static_cast<GLint>(setScissorTestCommand->rectangle.position.v[1]),
                                       static_cast<GLsizei>(setScissorTestCommand->rectangle.size.v[0]),
                                       static_cast<GLsizei>(setScissorTestCommand->rectangle.size.v[1]));

                        break;
                    }

                    case Command::Type::setViewport:
                    {
                        auto setViewportCommand = static_cast<const SetViewportCommand*>(command.get());

                        setViewport(static_cast<GLint>(setViewportCommand->viewport.position.v[0]),
                                    static_cast<GLint>(setViewportCommand->viewport.position.v[1]),
                                    static_cast<GLsizei>(setViewportCommand->viewport.size.v[0]),
                                    static_cast<GLsizei>(setViewportCommand->viewport.size.v[1]));

                        break;
                    }

                    case Command::Type::initDepthStencilState:
                    {
                        auto initDepthStencilStateCommand = static_cast<const InitDepthStencilStateCommand*>(command.get());
                        auto depthStencilState = std::make_unique<DepthStencilState>(*this,
                                                                                     initDepthStencilStateCommand->depthTest,
                                                                                     initDepthStencilStateCommand->depthWrite,
                                                                                     initDepthStencilStateCommand->compareFunction,
                                                                                     initDepthStencilStateCommand->stencilEnabled,
                                                                                     initDepthStencilStateCommand->stencilReadMask,
                                                                                     initDepthStencilStateCommand->stencilWriteMask,
                                                                                     initDepthStencilStateCommand->frontFaceStencilFailureOperation,
                                                                                     initDepthStencilStateCommand->frontFaceStencilDepthFailureOperation,
                                                                                     initDepthStencilStateCommand->frontFaceStencilPassOperation,
                                                                                     initDepthStencilStateCommand->frontFaceStencilCompareFunction,
                                                                                     initDepthStencilStateCommand->backFaceStencilFailureOperation,
                                                                                     initDepthStencilStateCommand->backFaceStencilDepthFailureOperation,
                                                                                     initDepthStencilStateCommand->backFaceStencilPassOperation,
                                                                                     initDepthStencilStateCommand->backFaceStencilCompareFunction);

                        if (initDepthStencilStateCommand->depthStencilState > resources.size())
                            resources.resize(initDepthStencilStateCommand->depthStencilState);
                        resources[initDepthStencilStateCommand->depthStencilState - 1] = std::move(depthStencilState);
                        break;
                    }

                    case Command::Type::setDepthStencilState:
                    {
                        auto setDepthStencilStateCommand = static_cast<const SetDepthStencilStateCommand*>(command.get());

                        if (setDepthStencilStateCommand->depthStencilState)
                        {
                            auto depthStencilState = getResource<DepthStencilState>(setDepthStencilStateCommand->depthStencilState);

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

                        if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to update depth stencil state");

                        break;
                    }

                    case Command::Type::setPipelineState:
                    {
                        auto setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command.get());

                        auto blendState = getResource<BlendState>(setPipelineStateCommand->blendState);
                        auto shader = getResource<Shader>(setPipelineStateCommand->shader);
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

                        const auto cullFace = getCullFace(setPipelineStateCommand->cullMode);
                        setCullFace(cullFace != GL_NONE, cullFace);

#if OUZEL_OPENGLES
                        if (setPipelineStateCommand->fillMode != FillMode::solid)
                            logger.log(Log::Level::warning) << "Unsupported fill mode";
#else
                        setPolygonFillMode(getFillMode(setPipelineStateCommand->fillMode));
#endif

                        break;
                    }

                    case Command::Type::draw:
                    {
                        auto drawCommand = static_cast<const DrawCommand*>(command.get());

                        // mesh buffer
                        auto indexBuffer = getResource<Buffer>(drawCommand->indexBuffer);
                        auto vertexBuffer = getResource<Buffer>(drawCommand->vertexBuffer);

                        assert(indexBuffer);
                        assert(indexBuffer->getBufferId());
                        assert(vertexBuffer);
                        assert(vertexBuffer->getBufferId());

                        // draw
                        bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getBufferId());
                        bindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getBufferId());

                        const std::byte* vertexOffset = nullptr;

                        for (GLuint index = 0; index < RenderDevice::vertexAttributes.size(); ++index)
                        {
                            const auto& vertexAttribute = RenderDevice::vertexAttributes[index];

                            glEnableVertexAttribArrayProc(index);
                            glVertexAttribPointerProc(index,
                                                      getArraySize(vertexAttribute.dataType),
                                                      getVertexType(vertexAttribute.dataType),
                                                      isNormalized(vertexAttribute.dataType),
                                                      static_cast<GLsizei>(sizeof(Vertex)),
                                                      vertexOffset);

                            vertexOffset += getDataTypeSize(vertexAttribute.dataType);
                        }

                        if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to update vertex attributes");

                        assert(drawCommand->indexCount);
                        assert(indexBuffer->getSize());
                        assert(vertexBuffer->getSize());

                        const std::byte* indexOffset = nullptr;
                        indexOffset += drawCommand->startIndex * drawCommand->indexSize;

                        glDrawElementsProc(getDrawMode(drawCommand->drawMode),
                                           static_cast<GLsizei>(drawCommand->indexCount),
                                           getIndexType(drawCommand->indexSize),
                                           indexOffset);

                        if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to draw elements");

                        break;
                    }

                    case Command::Type::pushDebugMarker:
                    {
                        auto pushDebugMarkerCommand = static_cast<const PushDebugMarkerCommand*>(command.get());
                        if (glPushGroupMarkerEXTProc) glPushGroupMarkerEXTProc(0, pushDebugMarkerCommand->name.c_str());
                        break;
                    }

                    case Command::Type::popDebugMarker:
                    {
                        if (glPopGroupMarkerEXTProc) glPopGroupMarkerEXTProc();
                        break;
                    }

                    case Command::Type::initBlendState:
                    {
                        auto initBlendStateCommand = static_cast<const InitBlendStateCommand*>(command.get());

                        auto blendState = std::make_unique<BlendState>(*this,
                                                                       initBlendStateCommand->enableBlending,
                                                                       initBlendStateCommand->colorBlendSource,
                                                                       initBlendStateCommand->colorBlendDest,
                                                                       initBlendStateCommand->colorOperation,
                                                                       initBlendStateCommand->alphaBlendSource,
                                                                       initBlendStateCommand->alphaBlendDest,
                                                                       initBlendStateCommand->alphaOperation,
                                                                       initBlendStateCommand->colorMask);

                        if (initBlendStateCommand->blendState > resources.size())
                            resources.resize(initBlendStateCommand->blendState);
                        resources[initBlendStateCommand->blendState - 1] = std::move(blendState);
                        break;
                    }

                    case Command::Type::initBuffer:
                    {
                        auto initBufferCommand = static_cast<const InitBufferCommand*>(command.get());

                        auto buffer = std::make_unique<Buffer>(*this,
                                                               initBufferCommand->bufferType,
                                                               initBufferCommand->flags,
                                                               initBufferCommand->data,
                                                               initBufferCommand->size);

                        if (initBufferCommand->buffer > resources.size())
                            resources.resize(initBufferCommand->buffer);
                        resources[initBufferCommand->buffer - 1] = std::move(buffer);
                        break;
                    }

                    case Command::Type::setBufferData:
                    {
                        auto setBufferDataCommand = static_cast<const SetBufferDataCommand*>(command.get());

                        auto buffer = getResource<Buffer>(setBufferDataCommand->buffer);
                        buffer->setData(setBufferDataCommand->data);
                        break;
                    }

                    case Command::Type::initShader:
                    {
                        auto initShaderCommand = static_cast<const InitShaderCommand*>(command.get());

                        auto shader = std::make_unique<Shader>(*this,
                                                               initShaderCommand->fragmentShader,
                                                               initShaderCommand->vertexShader,
                                                               initShaderCommand->vertexAttributes,
                                                               initShaderCommand->fragmentShaderConstantInfo,
                                                               initShaderCommand->vertexShaderConstantInfo,
                                                               initShaderCommand->fragmentShaderFunction,
                                                               initShaderCommand->vertexShaderFunction);

                        if (initShaderCommand->shader > resources.size())
                            resources.resize(initShaderCommand->shader);
                        resources[initShaderCommand->shader - 1] = std::move(shader);
                        break;
                    }

                    case Command::Type::setShaderConstants:
                    {
                        auto setShaderConstantsCommand = static_cast<const SetShaderConstantsCommand*>(command.get());

                        if (!currentShader)
                            throw Error("No shader set");

                        // pixel shader constants
                        const std::vector<Shader::Location>& fragmentShaderConstantLocations = currentShader->getFragmentShaderConstantLocations();

                        if (setShaderConstantsCommand->fragmentShaderConstants.size() > fragmentShaderConstantLocations.size())
                            throw Error("Invalid pixel shader constant size");

                        for (std::size_t i = 0; i < setShaderConstantsCommand->fragmentShaderConstants.size(); ++i)
                        {
                            const auto& fragmentShaderConstantLocation = fragmentShaderConstantLocations[i];
                            const auto& fragmentShaderConstant = setShaderConstantsCommand->fragmentShaderConstants[i];

                            setUniform(fragmentShaderConstantLocation.location,
                                       fragmentShaderConstantLocation.dataType,
                                       fragmentShaderConstant.data());
                        }

                        // vertex shader constants
                        const std::vector<Shader::Location>& vertexShaderConstantLocations = currentShader->getVertexShaderConstantLocations();

                        if (setShaderConstantsCommand->vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                            throw Error("Invalid vertex shader constant size");

                        for (std::size_t i = 0; i < setShaderConstantsCommand->vertexShaderConstants.size(); ++i)
                        {
                            const auto& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                            const auto& vertexShaderConstant = setShaderConstantsCommand->vertexShaderConstants[i];

                            setUniform(vertexShaderConstantLocation.location,
                                       vertexShaderConstantLocation.dataType,
                                       vertexShaderConstant.data());
                        }

                        break;
                    }

                    case Command::Type::initTexture:
                    {
                        auto initTextureCommand = static_cast<const InitTextureCommand*>(command.get());

                        auto texture = std::make_unique<Texture>(*this,
                                                                 initTextureCommand->levels,
                                                                 initTextureCommand->textureType,
                                                                 initTextureCommand->flags,
                                                                 initTextureCommand->sampleCount,
                                                                 initTextureCommand->pixelFormat,
                                                                 initTextureCommand->filter,
                                                                 initTextureCommand->maxAnisotropy);

                        if (initTextureCommand->texture > resources.size())
                            resources.resize(initTextureCommand->texture);
                        resources[initTextureCommand->texture - 1] = std::move(texture);
                        break;
                    }

                    case Command::Type::setTextureData:
                    {
                        auto setTextureDataCommand = static_cast<const SetTextureDataCommand*>(command.get());

                        auto texture = getResource<Texture>(setTextureDataCommand->texture);
                        texture->setData(setTextureDataCommand->levels);

                        break;
                    }

                    case Command::Type::setTextureParameters:
                    {
                        auto setTextureParametersCommand = static_cast<const SetTextureParametersCommand*>(command.get());

                        auto texture = getResource<Texture>(setTextureParametersCommand->texture);
                        texture->setFilter(setTextureParametersCommand->filter);
                        texture->setAddressX(setTextureParametersCommand->addressX);
                        texture->setAddressY(setTextureParametersCommand->addressY);
                        texture->setAddressZ(setTextureParametersCommand->addressZ);
                        texture->setMaxAnisotropy(setTextureParametersCommand->maxAnisotropy);
                        break;
                    }

                    case Command::Type::setTextures:
                    {
                        auto setTexturesCommand = static_cast<const SetTexturesCommand*>(command.get());

                        for (std::uint32_t layer = 0; layer < setTexturesCommand->textures.size(); ++layer)
                        {
                            if (auto texture = getResource<Texture>(setTexturesCommand->textures[layer]))
                                bindTexture(GL_TEXTURE_2D, layer, texture->getTextureId());
                            else
                                bindTexture(GL_TEXTURE_2D, layer, 0);
                        }

                        break;
                    }

                    default:
                        throw Error("Invalid command");
                }

                if (command->type == Command::Type::present) return;
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

        std::vector<std::uint8_t> data(static_cast<std::size_t>(frameBufferWidth * frameBufferHeight * pixelSize));

        glReadPixelsProc(0, 0, frameBufferWidth, frameBufferHeight,
                         GL_RGBA, GL_UNSIGNED_BYTE, data.data());

        if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to read pixels from frame buffer");

        // flip the image vertically
        const auto rowSize = frameBufferWidth * pixelSize;
        std::vector<std::uint8_t> temp(static_cast<std::size_t>(rowSize));
        for (GLsizei row = 0; row < frameBufferHeight / 2; ++row)
        {
            const auto topRowOffset = row * rowSize;
            const auto bottomRowOffset = (frameBufferHeight - row - 1) * rowSize;

            std::copy(data.begin() + topRowOffset,
                      data.begin() + topRowOffset + rowSize,
                      temp.begin());

            std::copy(data.begin() + bottomRowOffset,
                      data.begin() + bottomRowOffset + rowSize,
                      data.begin() + topRowOffset);

            std::copy(temp.begin(), temp.end(),
                      data.begin() + bottomRowOffset);

        }

        if (!stbi_write_png(filename.c_str(), frameBufferWidth, frameBufferHeight, pixelSize, data.data(), frameBufferWidth * pixelSize))
            throw Error("Failed to save image to file");
    }
}

#endif
