// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include <stdexcept>
#include "OGLTexture.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            static GLenum getOpenGlInternalPixelFormat(PixelFormat pixelFormat, uint32_t openGLVersion)
            {
#if OUZEL_OPENGLES
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
                        case PixelFormat::RGBA8_UNORM_SRGB: return GL_SRGB8_ALPHA8;
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
                        case PixelFormat::DEPTH: return GL_DEPTH_COMPONENT24;
                        case PixelFormat::DEPTH_STENCIL: return GL_DEPTH24_STENCIL8;
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
                    case PixelFormat::RGBA8_UNORM_SRGB: return GL_SRGB8_ALPHA8;
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

            static GLenum getOpenGlPixelFormat(PixelFormat pixelFormat)
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
                    case PixelFormat::RGBA8_UNORM_SRGB:
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

            static GLenum getOpenGlPixelType(PixelFormat pixelFormat)
            {
                switch (pixelFormat)
                {
                    case PixelFormat::A8_UNORM:
                    case PixelFormat::R8_UNORM:
                    case PixelFormat::R16_UNORM:
                    case PixelFormat::RG8_UNORM:
                    case PixelFormat::RGBA8_UNORM:
                    case PixelFormat::RGBA8_UNORM_SRGB:
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
                        return GL_UNSIGNED_INT;
                    case PixelFormat::DEPTH_STENCIL:
                        return GL_UNSIGNED_INT_24_8;
                    default:
                        return 0;
                }
            }

            static GLint getWrapMode(SamplerAddressMode address)
            {
                switch (address)
                {
                    case SamplerAddressMode::CLAMP_TO_EDGE:
                        return GL_CLAMP_TO_EDGE;
                    case SamplerAddressMode::CLAMP_TO_BORDER:
#if OUZEL_OPENGLES
                        return GL_CLAMP_TO_BORDER_EXT;
#else
                        return GL_CLAMP_TO_BORDER;
#endif
                    case SamplerAddressMode::REPEAT:
                        return GL_REPEAT;
                    case SamplerAddressMode::MIRROR_REPEAT:
                        return GL_MIRRORED_REPEAT;
                    default:
                        throw std::runtime_error("Invalid texture address mode");
                }
            }

            static GLenum getTextureTarget(TextureType type)
            {
                switch (type)
                {
#if !OUZEL_OPENGLES
                    case TextureType::ONE_DIMENSIONAL: return GL_TEXTURE_1D;
#endif
                    case TextureType::TWO_DIMENSIONAL: return GL_TEXTURE_2D;
                    case TextureType::THREE_DIMENSIONAL: return GL_TEXTURE_3D;
                    case TextureType::CUBE: return GL_TEXTURE_CUBE_MAP;
                    default: throw std::runtime_error("Invalid texture type");
                }
            }

            static GLenum getCubeFace(CubeFace face)
            {
                switch (face)
                {
                    case CubeFace::POSITIVE_X: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
                    case CubeFace::NEGATIVE_X: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
                    case CubeFace::POSITIVE_Y: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
                    case CubeFace::NEGATIVE_Y: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
                    case CubeFace::POSITIVE_Z: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
                    case CubeFace::NEGATIVE_Z: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
                    default: throw std::runtime_error("Invalid cube face");
                }
            }

            Texture::Texture(RenderDevice& initRenderDevice,
                             const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& initLevels,
                             TextureType type,
                             uint32_t initFlags,
                             uint32_t initSampleCount,
                             PixelFormat initPixelFormat):
                RenderResource(initRenderDevice),
                levels(initLevels),
                flags(initFlags),
                mipmaps(static_cast<uint32_t>(initLevels.size())),
                sampleCount(initSampleCount),
                textureTarget(getTextureTarget(type)),
                internalPixelFormat(getOpenGlInternalPixelFormat(initPixelFormat, renderDevice.getAPIMajorVersion())),
                pixelFormat(getOpenGlPixelFormat(initPixelFormat)),
                pixelType(getOpenGlPixelType(initPixelFormat))
            {
                if ((flags & Flags::BIND_RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                    throw std::runtime_error("Invalid mip map count");

                if (internalPixelFormat == GL_NONE)
                    throw std::runtime_error("Invalid pixel format");

                if (pixelFormat == GL_NONE)
                    throw std::runtime_error("Invalid pixel format");

                if (pixelType == GL_NONE)
                    throw std::runtime_error("Invalid pixel format");

                createTexture();

                renderDevice.bindTexture(textureTarget, 0, textureId);

                if (!(flags & Flags::BIND_RENDER_TARGET))
                {
                    if (!levels.empty())
                    {
                        if (renderDevice.isTextureBaseLevelSupported()) renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
                        if (renderDevice.isTextureMaxLevelSupported()) renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                        GLenum error;

                        if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to set texture base and max levels");
                    }

                    for (size_t level = 0; level < levels.size(); ++level)
                    {
                        if (!levels[level].second.empty())
                            renderDevice.glTexImage2DProc(GL_TEXTURE_2D, static_cast<GLint>(level), static_cast<GLint>(internalPixelFormat),
                                                          static_cast<GLsizei>(levels[level].first.v[0]),
                                                          static_cast<GLsizei>(levels[level].first.v[1]), 0,
                                                          pixelFormat, pixelType, levels[level].second.data());
                        else
                            renderDevice.glTexImage2DProc(GL_TEXTURE_2D, static_cast<GLint>(level), static_cast<GLint>(internalPixelFormat),
                                                          static_cast<GLsizei>(levels[level].first.v[0]),
                                                          static_cast<GLsizei>(levels[level].first.v[1]), 0,
                                                          pixelFormat, pixelType, nullptr);
                    }

                    GLenum error;

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to upload texture data");
                }

                setTextureParameters();
            }

            Texture::~Texture()
            {
                if (bufferId)
                    renderDevice.deleteRenderBuffer(bufferId);

                if (textureId)
                    renderDevice.deleteTexture(textureId);
            }

            void Texture::reload()
            {
                textureId = 0;
                bufferId = 0;

                createTexture();

                if (!(flags & Flags::BIND_RENDER_TARGET))
                {
                    renderDevice.bindTexture(textureTarget, 0, textureId);

                    if (!levels.empty())
                    {
                        if (renderDevice.isTextureBaseLevelSupported()) renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
                        if (renderDevice.isTextureMaxLevelSupported()) renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                        GLenum error;

                        if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to set texture base and max levels");
                    }

                    for (size_t level = 0; level < levels.size(); ++level)
                    {
                        if (!levels[level].second.empty())
                            renderDevice.glTexImage2DProc(GL_TEXTURE_2D, static_cast<GLint>(level), static_cast<GLint>(internalPixelFormat),
                                                          static_cast<GLsizei>(levels[level].first.v[0]),
                                                          static_cast<GLsizei>(levels[level].first.v[1]), 0,
                                                          pixelFormat, pixelType, levels[level].second.data());
                        else
                            renderDevice.glTexImage2DProc(GL_TEXTURE_2D, static_cast<GLint>(level), static_cast<GLint>(internalPixelFormat),
                                                          static_cast<GLsizei>(levels[level].first.v[0]),
                                                          static_cast<GLsizei>(levels[level].first.v[1]), 0,
                                                          pixelFormat, pixelType, nullptr);
                    }

                    GLenum error;

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to upload texture data");

                    setTextureParameters();
                }
            }

            void Texture::setData(const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& newLevels)
            {
                if (!(flags & Flags::DYNAMIC) || flags & Flags::BIND_RENDER_TARGET)
                    throw std::runtime_error("Texture is not dynamic");

                levels = newLevels;

                if (!textureId)
                    throw std::runtime_error("Texture not initialized");

                renderDevice.bindTexture(textureTarget, 0, textureId);

                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].second.empty())
                        renderDevice.glTexSubImage2DProc(GL_TEXTURE_2D, static_cast<GLint>(level), 0, 0,
                                                         static_cast<GLsizei>(levels[level].first.v[0]),
                                                         static_cast<GLsizei>(levels[level].first.v[1]),
                                                         pixelFormat, pixelType,
                                                         levels[level].second.data());
                }

                GLenum error;

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to upload texture data");
            }

            void Texture::setFilter(SamplerFilter newFilter)
            {
                filter = newFilter;

                if (!textureId)
                    throw std::runtime_error("Texture not initialized");

                renderDevice.bindTexture(textureTarget, 0, textureId);

                SamplerFilter finalFilter = (filter == SamplerFilter::DEFAULT) ? renderDevice.getTextureFilter() : filter;

                switch (finalFilter)
                {
                    case SamplerFilter::POINT:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        break;
                    case SamplerFilter::LINEAR:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        break;
                    case SamplerFilter::BILINEAR:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        break;
                    case SamplerFilter::TRILINEAR:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        break;
                    default:
                        throw std::runtime_error("Invalid texture filter");
                }

                GLenum error;

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set texture filter");
            }

            void Texture::setAddressX(SamplerAddressMode newAddressX)
            {
                addressX = newAddressX;

                if (!textureId)
                    throw std::runtime_error("Texture not initialized");

                renderDevice.bindTexture(textureTarget, 0, textureId);
                renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_S, getWrapMode(addressX));

                GLenum error;
                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");
            }

            void Texture::setAddressY(SamplerAddressMode newAddressY)
            {
                addressY = newAddressY;

                if (!textureId)
                    throw std::runtime_error("Texture not initialized");

                renderDevice.bindTexture(textureTarget, 0, textureId);
                renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_T, getWrapMode(addressY));

                GLenum error;
                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");
            }

            void Texture::setAddressZ(SamplerAddressMode newAddressZ)
            {
                addressZ = newAddressZ;

                if (textureTarget == GL_TEXTURE_3D)
                {
                    if (!textureId)
                        throw std::runtime_error("Texture not initialized");

                    renderDevice.bindTexture(textureTarget, 0, textureId);
                    renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_R, getWrapMode(addressZ));

                    GLenum error;
                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");
                }
            }

            void Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
            {
                maxAnisotropy = newMaxAnisotropy;

                if (!textureId)
                    throw std::runtime_error("Texture not initialized");

                renderDevice.bindTexture(textureTarget, 0, textureId);

                if (maxAnisotropy > 1 && renderDevice.isAnisotropicFilteringSupported())
                {
                    renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(maxAnisotropy));

                    GLenum error;

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set texture max anisotrophy");
                }
            }

            void Texture::createTexture()
            {
                width = static_cast<GLsizei>(levels.front().first.v[0]);
                height = static_cast<GLsizei>(levels.front().first.v[1]);

                if ((flags & Flags::BIND_RENDER_TARGET) && renderDevice.isRenderTargetsSupported())
                {
                    if (flags & Flags::BIND_SHADER)
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
                            renderDevice.bindTexture(textureTarget, 0, textureId);

                            renderDevice.glTexImage2DProc(textureTarget, 0,
                                                          static_cast<GLint>(internalPixelFormat),
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
                            renderDevice.glRenderbufferStorageProc(GL_RENDERBUFFER,
                                                                   internalPixelFormat,
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

            void Texture::setTextureParameters()
            {
                renderDevice.bindTexture(textureTarget, 0, textureId);

                SamplerFilter finalFilter = (filter == SamplerFilter::DEFAULT) ? renderDevice.getTextureFilter() : filter;

                switch (finalFilter)
                {
                    case SamplerFilter::DEFAULT:
                    case SamplerFilter::POINT:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        break;
                    case SamplerFilter::LINEAR:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        break;
                    case SamplerFilter::BILINEAR:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        break;
                    case SamplerFilter::TRILINEAR:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        break;
                    default:
                        throw std::runtime_error("Invalid texture filter");
                }

                GLenum error;

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set texture filter");

                renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_S, getWrapMode(addressX));

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");

                renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_T, getWrapMode(addressY));

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");

                if (textureTarget == GL_TEXTURE_3D)
                {
                    renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_R, getWrapMode(addressZ));

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");
                }

                auto finalMaxAnisotropy = static_cast<GLint>((maxAnisotropy == 0) ? renderDevice.getMaxAnisotropy() : maxAnisotropy);

                if (finalMaxAnisotropy > 1 && renderDevice.isAnisotropicFilteringSupported())
                {
                    renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, finalMaxAnisotropy);

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set texture max anisotrophy");
                }
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
