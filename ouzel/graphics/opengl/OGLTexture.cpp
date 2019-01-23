// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include <stdexcept>
#include "OGLTexture.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        static GLenum getOGLInternalPixelFormat(PixelFormat pixelFormat, uint32_t openGLVersion)
        {
#if OUZEL_SUPPORTS_OPENGLES
            if (openGLVersion >= 3)
            {
                switch (pixelFormat)
                {
                    case PixelFormat::A8_UNORM: return GL_ALPHA8_OES;
                    case PixelFormat::R8_UNORM: return GL_R8;
                    case PixelFormat::R8_SNORM: return GL_R8_SNORM;
                    case PixelFormat::R8_UINT: return GL_R8UI;
                    case PixelFormat::R8_SINT: return GL_R8I;
                    case PixelFormat::R16_UNORM: return GL_NONE;
                    case PixelFormat::R16_SNORM: return GL_NONE;
                    case PixelFormat::R16_UINT: return GL_R16UI;
                    case PixelFormat::R16_SINT: return GL_R16I;
                    case PixelFormat::R16_FLOAT: return GL_R16F;
                    case PixelFormat::R32_UINT: return GL_R32UI;
                    case PixelFormat::R32_SINT: return GL_R32I;
                    case PixelFormat::R32_FLOAT: return GL_R32F;
                    case PixelFormat::RG8_UNORM: return GL_RG8;
                    case PixelFormat::RG8_SNORM: return GL_RG8_SNORM;
                    case PixelFormat::RG8_UINT: return GL_RG8UI;
                    case PixelFormat::RG8_SINT: return GL_RG8I;
                    case PixelFormat::RGBA8_UNORM: return GL_RGBA8;
                    case PixelFormat::RGBA8_SNORM: return GL_RGBA8_SNORM;
                    case PixelFormat::RGBA8_UINT: return GL_RGBA8UI;
                    case PixelFormat::RGBA8_SINT: return GL_RGBA8I;
                    case PixelFormat::RGBA16_UNORM: return GL_NONE;
                    case PixelFormat::RGBA16_SNORM: return GL_NONE;
                    case PixelFormat::RGBA16_UINT: return GL_RGBA16UI;
                    case PixelFormat::RGBA16_SINT: return GL_RGBA16I;
                    case PixelFormat::RGBA16_FLOAT: return GL_RGBA16F;
                    case PixelFormat::RGBA32_UINT: return GL_RGBA32UI;
                    case PixelFormat::RGBA32_SINT: return GL_RGBA32I;
                    case PixelFormat::RGBA32_FLOAT: return GL_RGBA32F;
                    case PixelFormat::DEPTH: return GL_DEPTH_COMPONENT24;
                    case PixelFormat::DEPTH_STENCIL: return GL_DEPTH24_STENCIL8;
                    default: return GL_NONE;
                }
            }
            else
            {
                switch (pixelFormat)
                {
                    case PixelFormat::A8_UNORM: return GL_ALPHA;
                    case PixelFormat::RGBA8_UNORM: return GL_RGBA;
                    default: return GL_NONE;
                }
            }
#else
            (void)openGLVersion;

            switch (pixelFormat)
            {
                case PixelFormat::A8_UNORM: return GL_ALPHA8_EXT;

                case PixelFormat::R8_UNORM: return GL_R8;
                case PixelFormat::R8_SNORM: return GL_R8_SNORM;
                case PixelFormat::R8_UINT: return GL_R8UI;
                case PixelFormat::R8_SINT: return GL_R8I;
                case PixelFormat::R16_UNORM: return GL_R16;
                case PixelFormat::R16_SNORM: return GL_R16_SNORM;
                case PixelFormat::R16_UINT: return GL_R16UI;
                case PixelFormat::R16_SINT: return GL_R16I;
                case PixelFormat::R16_FLOAT: return GL_R16F;
                case PixelFormat::R32_UINT: return GL_R32UI;
                case PixelFormat::R32_SINT: return GL_R32I;
                case PixelFormat::R32_FLOAT: return GL_R32F;
                case PixelFormat::RG8_UNORM: return GL_RG8;
                case PixelFormat::RG8_SNORM: return GL_RG8_SNORM;
                case PixelFormat::RG8_UINT: return GL_RG8UI;
                case PixelFormat::RG8_SINT: return GL_RG8I;
                case PixelFormat::RGBA8_UNORM: return GL_RGBA8;
                case PixelFormat::RGBA8_SNORM: return GL_RGBA8_SNORM;
                case PixelFormat::RGBA8_UINT: return GL_RGBA8UI;
                case PixelFormat::RGBA8_SINT: return GL_RGBA8I;
                case PixelFormat::RGBA16_UNORM: return GL_RGBA16;
                case PixelFormat::RGBA16_SNORM: return GL_RGBA16_SNORM;
                case PixelFormat::RGBA16_UINT: return GL_RGBA16UI;
                case PixelFormat::RGBA16_SINT: return GL_RGBA16I;
                case PixelFormat::RGBA16_FLOAT: return GL_RGBA16F;
                case PixelFormat::RGBA32_UINT: return GL_RGBA32UI;
                case PixelFormat::RGBA32_SINT: return GL_RGBA32I;
                case PixelFormat::RGBA32_FLOAT: return GL_RGBA32F;
                case PixelFormat::DEPTH: return GL_DEPTH_COMPONENT24;
                case PixelFormat::DEPTH_STENCIL: return GL_DEPTH24_STENCIL8;
                default: return GL_NONE;
            }
#endif
        }

        static GLenum getOGLPixelFormat(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::A8_UNORM:
                    return GL_ALPHA;
                case PixelFormat::R8_UNORM:
                case PixelFormat::R8_SNORM:
                case PixelFormat::R16_UNORM:
                case PixelFormat::R16_SNORM:
                case PixelFormat::R16_FLOAT:
                case PixelFormat::R32_FLOAT:
                    return GL_RED;
                case PixelFormat::R8_UINT:
                case PixelFormat::R8_SINT:
                case PixelFormat::R16_UINT:
                case PixelFormat::R16_SINT:
                case PixelFormat::R32_UINT:
                case PixelFormat::R32_SINT:
                    return GL_RED_INTEGER;
                case PixelFormat::RG8_UNORM:
                case PixelFormat::RG8_SNORM:
                    return GL_RG;
                case PixelFormat::RG8_UINT:
                case PixelFormat::RG8_SINT:
                    return GL_RG_INTEGER;
                case PixelFormat::RGBA8_UNORM:
                case PixelFormat::RGBA8_SNORM:
                case PixelFormat::RGBA16_UNORM:
                case PixelFormat::RGBA16_SNORM:
                case PixelFormat::RGBA16_FLOAT:
                case PixelFormat::RGBA32_FLOAT:
                    return GL_RGBA;
                case PixelFormat::RGBA8_UINT:
                case PixelFormat::RGBA8_SINT:
                case PixelFormat::RGBA16_UINT:
                case PixelFormat::RGBA16_SINT:
                case PixelFormat::RGBA32_UINT:
                case PixelFormat::RGBA32_SINT:
                    return GL_RGBA_INTEGER;
                case PixelFormat::DEPTH:
                    return GL_DEPTH_COMPONENT;
                case PixelFormat::DEPTH_STENCIL:
                    return GL_DEPTH_STENCIL;
                default:
                    return 0;
            }
        }

        static GLenum getOGLPixelType(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::A8_UNORM:
                case PixelFormat::R8_UNORM:
                case PixelFormat::R16_UNORM:
                case PixelFormat::RG8_UNORM:
                case PixelFormat::RGBA8_UNORM:
                case PixelFormat::RGBA16_UNORM:
                    return GL_UNSIGNED_BYTE;
                case PixelFormat::R8_SNORM:
                case PixelFormat::R16_SNORM:
                case PixelFormat::RG8_SNORM:
                case PixelFormat::RGBA8_SNORM:
                case PixelFormat::RGBA16_SNORM:
                    return GL_BYTE;
                case PixelFormat::R8_UINT:
                case PixelFormat::R16_UINT:
                case PixelFormat::R32_UINT:
                case PixelFormat::RG8_UINT:
                case PixelFormat::RGBA8_UINT:
                case PixelFormat::RGBA16_UINT:
                case PixelFormat::RGBA32_UINT:
                    return GL_UNSIGNED_INT;
                case PixelFormat::R8_SINT:
                case PixelFormat::R16_SINT:
                case PixelFormat::R32_SINT:
                case PixelFormat::RG8_SINT:
                case PixelFormat::RGBA8_SINT:
                case PixelFormat::RGBA16_SINT:
                case PixelFormat::RGBA32_SINT:
                    return GL_INT;
                case PixelFormat::R16_FLOAT:
                case PixelFormat::R32_FLOAT:
                case PixelFormat::RGBA16_FLOAT:
                case PixelFormat::RGBA32_FLOAT:
                case PixelFormat::DEPTH:
                    return GL_FLOAT;
                case PixelFormat::DEPTH_STENCIL:
                    return GL_UNSIGNED_INT_24_8;
                default:
                    return 0;
            }
        }

        static GLint getWrapMode(Texture::Address address)
        {
            switch (address)
            {
                case Texture::Address::CLAMP:
                    return GL_CLAMP_TO_EDGE;
                case Texture::Address::REPEAT:
                    return GL_REPEAT;
                case Texture::Address::MIRROR_REPEAT:
                    return GL_MIRRORED_REPEAT;
                default:
                    throw std::runtime_error("Invalid texture address mode");
            }
        }

        static GLenum getTextureType(Texture::Dimensions dimensions)
        {
            switch (dimensions)
            {
#if !OUZEL_SUPPORTS_OPENGLES
                case Texture::Dimensions::ONE: return GL_TEXTURE_1D;
#endif
                case Texture::Dimensions::TWO: return GL_TEXTURE_2D;
                case Texture::Dimensions::THREE: return GL_TEXTURE_3D;
                case Texture::Dimensions::CUBE: return GL_TEXTURE_CUBE_MAP;
                default: throw std::runtime_error("Invalid texture type");
            }
        }

        OGLTexture::OGLTexture(OGLRenderDevice& renderDeviceOGL,
                               const std::vector<Texture::Level>& initLevels,
                               uint32_t initFlags,
                               uint32_t initSampleCount,
                               PixelFormat initPixelFormat):
            OGLRenderResource(renderDeviceOGL),
            levels(initLevels),
            flags(initFlags),
            mipmaps(static_cast<uint32_t>(initLevels.size())),
            sampleCount(initSampleCount),
            internalPixelFormat(getOGLInternalPixelFormat(initPixelFormat, renderDevice.getAPIMajorVersion())),
            pixelFormat(getOGLPixelFormat(initPixelFormat)),
            pixelType(getOGLPixelType(initPixelFormat))
        {
            if ((flags & Texture::BIND_RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            if (internalPixelFormat == GL_NONE)
                throw std::runtime_error("Invalid pixel format");

            if (pixelFormat == GL_NONE)
                throw std::runtime_error("Invalid pixel format");

            if (pixelType == GL_NONE)
                throw std::runtime_error("Invalid pixel format");

            createTexture();

            renderDevice.bindTexture(textureId, 0);

            if (!(flags & Texture::BIND_RENDER_TARGET))
            {
                if (!levels.empty())
                {
                    if (renderDevice.isTextureBaseLevelSupported()) renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                    if (renderDevice.isTextureMaxLevelSupported()) renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                    GLenum error;

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set texture base and max levels");
                }

                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].data.empty())
                    {
                        renderDevice.glTexImage2DProc(GL_TEXTURE_2D, static_cast<GLint>(level), static_cast<GLint>(internalPixelFormat),
                                                      static_cast<GLsizei>(levels[level].size.v[0]),
                                                      static_cast<GLsizei>(levels[level].size.v[1]), 0,
                                                      pixelFormat, pixelType, levels[level].data.data());
                    }
                    else
                    {
                        renderDevice.glTexImage2DProc(GL_TEXTURE_2D, static_cast<GLint>(level), static_cast<GLint>(internalPixelFormat),
                                                      static_cast<GLsizei>(levels[level].size.v[0]),
                                                      static_cast<GLsizei>(levels[level].size.v[1]), 0,
                                                      pixelFormat, pixelType, nullptr);
                    }
                }

                GLenum error;

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to upload texture data");
            }

            setTextureParameters();
        }

        OGLTexture::~OGLTexture()
        {
            if (bufferId)
                renderDevice.deleteRenderBuffer(bufferId);

            if (textureId)
                renderDevice.deleteTexture(textureId);
        }

        void OGLTexture::reload()
        {
            textureId = 0;
            bufferId = 0;

            createTexture();

            if (!(flags & Texture::BIND_RENDER_TARGET))
            {
                renderDevice.bindTexture(textureId, 0);

                if (!levels.empty())
                {
                    if (renderDevice.isTextureBaseLevelSupported()) renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                    if (renderDevice.isTextureMaxLevelSupported()) renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                    GLenum error;

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set texture base and max levels");
                }

                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].data.empty())
                    {
                        renderDevice.glTexImage2DProc(GL_TEXTURE_2D, static_cast<GLint>(level), static_cast<GLint>(internalPixelFormat),
                                                      static_cast<GLsizei>(levels[level].size.v[0]),
                                                      static_cast<GLsizei>(levels[level].size.v[1]), 0,
                                                      pixelFormat, pixelType, levels[level].data.data());
                    }
                    else
                    {
                        renderDevice.glTexImage2DProc(GL_TEXTURE_2D, static_cast<GLint>(level), static_cast<GLint>(internalPixelFormat),
                                                      static_cast<GLsizei>(levels[level].size.v[0]),
                                                      static_cast<GLsizei>(levels[level].size.v[1]), 0,
                                                      pixelFormat, pixelType, nullptr);
                    }
                }

                GLenum error;

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to upload texture data");

                setTextureParameters();
            }
        }

        void OGLTexture::setData(const std::vector<Texture::Level>& newLevels)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::BIND_RENDER_TARGET)
                throw std::runtime_error("Texture is not dynamic");

            levels = newLevels;

            if (!textureId)
                throw std::runtime_error("Texture not initialized");

            renderDevice.bindTexture(textureId, 0);

            for (size_t level = 0; level < levels.size(); ++level)
            {
                if (!levels[level].data.empty())
                {
                    renderDevice.glTexSubImage2DProc(GL_TEXTURE_2D, static_cast<GLint>(level), 0, 0,
                                                     static_cast<GLsizei>(levels[level].size.v[0]),
                                                     static_cast<GLsizei>(levels[level].size.v[1]),
                                                     pixelFormat, pixelType,
                                                     levels[level].data.data());
                }
            }

            GLenum error;

            if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to upload texture data");
        }

        void OGLTexture::setFilter(Texture::Filter newFilter)
        {
            filter = newFilter;

            if (!textureId)
                throw std::runtime_error("Texture not initialized");

            renderDevice.bindTexture(textureId, 0);

            Texture::Filter finalFilter = (filter == Texture::Filter::DEFAULT) ? renderDevice.getTextureFilter() : filter;

            switch (finalFilter)
            {
                case Texture::Filter::DEFAULT:
                case Texture::Filter::POINT:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                case Texture::Filter::LINEAR:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                case Texture::Filter::BILINEAR:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
                case Texture::Filter::TRILINEAR:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
                default:
                    throw std::runtime_error("Invalid texture filter");
            }

            GLenum error;

            if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set texture filter");
        }

        void OGLTexture::setAddressX(Texture::Address newAddressX)
        {
            addressX = newAddressX;

            if (!textureId)
                throw std::runtime_error("Texture not initialized");

            renderDevice.bindTexture(textureId, 0);
            renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getWrapMode(addressX));

            GLenum error;
            if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");
        }

        void OGLTexture::setAddressY(Texture::Address newAddressY)
        {
            addressY = newAddressY;

            if (!textureId)
                throw std::runtime_error("Texture not initialized");

            renderDevice.bindTexture(textureId, 0);
            renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getWrapMode(addressY));

            GLenum error;
            if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");
        }

        void OGLTexture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;

            if (!textureId)
                throw std::runtime_error("Texture not initialized");

            renderDevice.bindTexture(textureId, 0);

            uint32_t finalMaxAnisotropy = (maxAnisotropy == 0) ? renderDevice.getMaxAnisotropy() : maxAnisotropy;

            if (finalMaxAnisotropy > 1 && renderDevice.isAnisotropicFilteringSupported())
            {
                renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(finalMaxAnisotropy));

                GLenum error;

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set texture max anisotrophy");
            }
        }

        void OGLTexture::createTexture()
        {
            width = static_cast<GLsizei>(levels.front().size.v[0]);
            height = static_cast<GLsizei>(levels.front().size.v[1]);

            if ((flags & Texture::BIND_RENDER_TARGET) && renderDevice.isRenderTargetsSupported())
            {
                if (flags & Texture::BIND_SHADER)
                {
                    renderDevice.glGenTexturesProc(1, &textureId);

                    GLenum error;
                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to create texture");

                    if (sampleCount > 1 && renderDevice.glTexStorage2DMultisampleProc)
                    {
                        renderDevice.glTexStorage2DMultisampleProc(GL_TEXTURE_2D_MULTISAMPLE,
                                                                   static_cast<GLsizei>(sampleCount),
                                                                   internalPixelFormat,
                                                                   width, height, GL_TRUE);
                    }
                    else
                    {
                        renderDevice.bindTexture(textureId, 0);

                        renderDevice.glTexImage2DProc(GL_TEXTURE_2D, 0, static_cast<GLint>(internalPixelFormat),
                                                      width, height, 0,
                                                      pixelFormat, pixelType, nullptr);

                        if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to set color render texture's storage");
                    }
                }
                else
                {
                    renderDevice.glGenRenderbuffersProc(1, &bufferId);
                    renderDevice.glBindRenderbufferProc(GL_RENDERBUFFER, bufferId);

                    if (sampleCount > 1 && renderDevice.glRenderbufferStorageMultisampleProc)
                    {
                        renderDevice.glRenderbufferStorageMultisampleProc(GL_RENDERBUFFER,
                                                                          static_cast<GLsizei>(sampleCount),
                                                                          internalPixelFormat,
                                                                          width, height);

                        GLenum error;
                        if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to set color render buffer's multisample storage");
                    }
                    else
                    {
                        renderDevice.glRenderbufferStorageProc(GL_RENDERBUFFER, internalPixelFormat,
                                                               width, height);

                        GLenum error;
                        if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to set color render buffer's storage");
                    }
                }
            }
            else
            {
                renderDevice.glGenTexturesProc(1, &textureId);

                GLenum error;
                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to create texture");
            }
        }

        void OGLTexture::setTextureParameters()
        {
            renderDevice.bindTexture(textureId, 0);

            Texture::Filter finalFilter = (filter == Texture::Filter::DEFAULT) ? renderDevice.getTextureFilter() : filter;

            switch (finalFilter)
            {
                case Texture::Filter::DEFAULT:
                case Texture::Filter::POINT:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                case Texture::Filter::LINEAR:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                case Texture::Filter::BILINEAR:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
                case Texture::Filter::TRILINEAR:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
                default:
                    throw std::runtime_error("Invalid texture filter");
            }

            GLenum error;

            if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set texture filter");

            switch (addressX)
            {
                case Texture::Address::CLAMP:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    break;
                case Texture::Address::REPEAT:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    break;
                case Texture::Address::MIRROR_REPEAT:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                    break;
                default:
                    throw std::runtime_error("Invalid texture address mode");
            }

            if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");

            switch (addressY)
            {
                case Texture::Address::CLAMP:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    break;
                case Texture::Address::REPEAT:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    break;
                case Texture::Address::MIRROR_REPEAT:
                    renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                    break;
                default:
                    throw std::runtime_error("Invalid texture address mode");
            }

            if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");

            uint32_t finalMaxAnisotropy = (maxAnisotropy == 0) ? renderDevice.getMaxAnisotropy() : maxAnisotropy;

            if (finalMaxAnisotropy > 1 && renderDevice.isAnisotropicFilteringSupported())
            {
                renderDevice.glTexParameteriProc(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(finalMaxAnisotropy));

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set texture max anisotrophy");
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
