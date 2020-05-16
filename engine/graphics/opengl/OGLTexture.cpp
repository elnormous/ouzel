// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../../core/Setup.h"

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
            namespace
            {
                constexpr GLenum getOpenGlInternalPixelFormat(PixelFormat pixelFormat, std::uint32_t openGLVersion)
                {
#if OUZEL_OPENGLES
                    if (openGLVersion >= 3)
                    {
                        switch (pixelFormat)
                        {
                            case PixelFormat::A8UNorm: return GL_ALPHA8_OES;
                            case PixelFormat::R8UNorm: return GL_R8;
                            case PixelFormat::R8SNorm: return GL_R8_SNORM;
                            case PixelFormat::R8UInt: return GL_R8UI;
                            case PixelFormat::R8SInt: return GL_R8I;
                            case PixelFormat::R16UNorm: return GL_NONE;
                            case PixelFormat::R16SNorm: return GL_NONE;
                            case PixelFormat::R16UInt: return GL_R16UI;
                            case PixelFormat::R16SInt: return GL_R16I;
                            case PixelFormat::R16Float: return GL_R16F;
                            case PixelFormat::R32UInt: return GL_R32UI;
                            case PixelFormat::R32SInt: return GL_R32I;
                            case PixelFormat::R32Float: return GL_R32F;
                            case PixelFormat::RG8UNorm: return GL_RG8;
                            case PixelFormat::RG8SNorm: return GL_RG8_SNORM;
                            case PixelFormat::RG8UInt: return GL_RG8UI;
                            case PixelFormat::RG8SInt: return GL_RG8I;
                            case PixelFormat::RGBA8UNorm: return GL_RGBA8;
                            case PixelFormat::RGBA8UNormSRGB: return GL_SRGB8_ALPHA8;
                            case PixelFormat::RGBA8SNorm: return GL_RGBA8_SNORM;
                            case PixelFormat::RGBA8UInt: return GL_RGBA8UI;
                            case PixelFormat::RGBA8SInt: return GL_RGBA8I;
                            case PixelFormat::RGBA16UNorm: return GL_NONE;
                            case PixelFormat::RGBA16SNorm: return GL_NONE;
                            case PixelFormat::RGBA16UInt: return GL_RGBA16UI;
                            case PixelFormat::RGBA16SInt: return GL_RGBA16I;
                            case PixelFormat::RGBA16Float: return GL_RGBA16F;
                            case PixelFormat::RGBA32UInt: return GL_RGBA32UI;
                            case PixelFormat::RGBA32SInt: return GL_RGBA32I;
                            case PixelFormat::RGBA32Float: return GL_RGBA32F;
                            case PixelFormat::Depth: return GL_DEPTH_COMPONENT24;
                            case PixelFormat::DepthStencil: return GL_DEPTH24_STENCIL8;
                            default: return GL_NONE;
                        }
                    }
                    else
                    {
                        switch (pixelFormat)
                        {
                            case PixelFormat::A8UNorm: return GL_ALPHA;
                            case PixelFormat::RGBA8UNorm: return GL_RGBA;
                            case PixelFormat::Depth: return GL_DEPTH_COMPONENT24;
                            case PixelFormat::DepthStencil: return GL_DEPTH24_STENCIL8;
                            default: return GL_NONE;
                        }
                    }
#else
                    static_cast<void>(openGLVersion);

                    switch (pixelFormat)
                    {
                        case PixelFormat::A8UNorm: return GL_ALPHA8_EXT;

                        case PixelFormat::R8UNorm: return GL_R8;
                        case PixelFormat::R8SNorm: return GL_R8_SNORM;
                        case PixelFormat::R8UInt: return GL_R8UI;
                        case PixelFormat::R8SInt: return GL_R8I;
                        case PixelFormat::R16UNorm: return GL_R16;
                        case PixelFormat::R16SNorm: return GL_R16_SNORM;
                        case PixelFormat::R16UInt: return GL_R16UI;
                        case PixelFormat::R16SInt: return GL_R16I;
                        case PixelFormat::R16Float: return GL_R16F;
                        case PixelFormat::R32UInt: return GL_R32UI;
                        case PixelFormat::R32SInt: return GL_R32I;
                        case PixelFormat::R32Float: return GL_R32F;
                        case PixelFormat::RG8UNorm: return GL_RG8;
                        case PixelFormat::RG8SNorm: return GL_RG8_SNORM;
                        case PixelFormat::RG8UInt: return GL_RG8UI;
                        case PixelFormat::RG8SInt: return GL_RG8I;
                        case PixelFormat::RGBA8UNorm: return GL_RGBA8;
                        case PixelFormat::RGBA8UNormSRGB: return GL_SRGB8_ALPHA8;
                        case PixelFormat::RGBA8SNorm: return GL_RGBA8_SNORM;
                        case PixelFormat::RGBA8UInt: return GL_RGBA8UI;
                        case PixelFormat::RGBA8SInt: return GL_RGBA8I;
                        case PixelFormat::RGBA16UNorm: return GL_RGBA16;
                        case PixelFormat::RGBA16SNorm: return GL_RGBA16_SNORM;
                        case PixelFormat::RGBA16UInt: return GL_RGBA16UI;
                        case PixelFormat::RGBA16SInt: return GL_RGBA16I;
                        case PixelFormat::RGBA16Float: return GL_RGBA16F;
                        case PixelFormat::RGBA32UInt: return GL_RGBA32UI;
                        case PixelFormat::RGBA32SInt: return GL_RGBA32I;
                        case PixelFormat::RGBA32Float: return GL_RGBA32F;
                        case PixelFormat::Depth: return GL_DEPTH_COMPONENT24;
                        case PixelFormat::DepthStencil: return GL_DEPTH24_STENCIL8;
                        default: return GL_NONE;
                    }
#endif
                }

                constexpr GLenum getOpenGlPixelFormat(PixelFormat pixelFormat)
                {
                    switch (pixelFormat)
                    {
                        case PixelFormat::A8UNorm:
                            return GL_ALPHA;
                        case PixelFormat::R8UNorm:
                        case PixelFormat::R8SNorm:
                        case PixelFormat::R16UNorm:
                        case PixelFormat::R16SNorm:
                        case PixelFormat::R16Float:
                        case PixelFormat::R32Float:
                            return GL_RED;
                        case PixelFormat::R8UInt:
                        case PixelFormat::R8SInt:
                        case PixelFormat::R16UInt:
                        case PixelFormat::R16SInt:
                        case PixelFormat::R32UInt:
                        case PixelFormat::R32SInt:
                            return GL_RED_INTEGER;
                        case PixelFormat::RG8UNorm:
                        case PixelFormat::RG8SNorm:
                            return GL_RG;
                        case PixelFormat::RG8UInt:
                        case PixelFormat::RG8SInt:
                            return GL_RG_INTEGER;
                        case PixelFormat::RGBA8UNorm:
                        case PixelFormat::RGBA8UNormSRGB:
                        case PixelFormat::RGBA8SNorm:
                        case PixelFormat::RGBA16UNorm:
                        case PixelFormat::RGBA16SNorm:
                        case PixelFormat::RGBA16Float:
                        case PixelFormat::RGBA32Float:
                            return GL_RGBA;
                        case PixelFormat::RGBA8UInt:
                        case PixelFormat::RGBA8SInt:
                        case PixelFormat::RGBA16UInt:
                        case PixelFormat::RGBA16SInt:
                        case PixelFormat::RGBA32UInt:
                        case PixelFormat::RGBA32SInt:
                            return GL_RGBA_INTEGER;
                        case PixelFormat::Depth:
                            return GL_DEPTH_COMPONENT;
                        case PixelFormat::DepthStencil:
                            return GL_DEPTH_STENCIL;
                        default:
                            return 0;
                    }
                }

                constexpr GLenum getOpenGlPixelType(PixelFormat pixelFormat)
                {
                    switch (pixelFormat)
                    {
                        case PixelFormat::A8UNorm:
                        case PixelFormat::R8UNorm:
                        case PixelFormat::R16UNorm:
                        case PixelFormat::RG8UNorm:
                        case PixelFormat::RGBA8UNorm:
                        case PixelFormat::RGBA8UNormSRGB:
                        case PixelFormat::RGBA16UNorm:
                            return GL_UNSIGNED_BYTE;
                        case PixelFormat::R8SNorm:
                        case PixelFormat::R16SNorm:
                        case PixelFormat::RG8SNorm:
                        case PixelFormat::RGBA8SNorm:
                        case PixelFormat::RGBA16SNorm:
                            return GL_BYTE;
                        case PixelFormat::R8UInt:
                        case PixelFormat::R16UInt:
                        case PixelFormat::R32UInt:
                        case PixelFormat::RG8UInt:
                        case PixelFormat::RGBA8UInt:
                        case PixelFormat::RGBA16UInt:
                        case PixelFormat::RGBA32UInt:
                            return GL_UNSIGNED_INT;
                        case PixelFormat::R8SInt:
                        case PixelFormat::R16SInt:
                        case PixelFormat::R32SInt:
                        case PixelFormat::RG8SInt:
                        case PixelFormat::RGBA8SInt:
                        case PixelFormat::RGBA16SInt:
                        case PixelFormat::RGBA32SInt:
                            return GL_INT;
                        case PixelFormat::R16Float:
                        case PixelFormat::R32Float:
                        case PixelFormat::RGBA16Float:
                        case PixelFormat::RGBA32Float:
                        case PixelFormat::Depth:
                            return GL_UNSIGNED_INT;
                        case PixelFormat::DepthStencil:
                            return GL_UNSIGNED_INT_24_8;
                        default:
                            return 0;
                    }
                }

                constexpr GLint getWrapMode(SamplerAddressMode address)
                {
                    switch (address)
                    {
                        case SamplerAddressMode::clampToEdge:
                            return GL_CLAMP_TO_EDGE;
                        case SamplerAddressMode::clampToBorder:
                            return GL_CLAMP_TO_BORDER;
                        case SamplerAddressMode::repeat:
                            return GL_REPEAT;
                        case SamplerAddressMode::mirrorRepeat:
                            return GL_MIRRORED_REPEAT;
                        default:
                            throw std::runtime_error("Invalid texture address mode");
                    }
                }

                constexpr GLenum getTextureTarget(TextureType type)
                {
                    switch (type)
                    {
#if !OUZEL_OPENGLES
                        case TextureType::oneDimensional: return GL_TEXTURE_1D;
#endif
                        case TextureType::twoDimensional: return GL_TEXTURE_2D;
                        case TextureType::threeDimensional: return GL_TEXTURE_3D;
                        case TextureType::cube: return GL_TEXTURE_CUBE_MAP;
                        default: throw std::runtime_error("Invalid texture type");
                    }
                }

                constexpr GLenum getCubeFace(CubeFace face)
                {
                    switch (face)
                    {
                        case CubeFace::positiveX: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
                        case CubeFace::negativeX: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
                        case CubeFace::positiveY: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
                        case CubeFace::negativeY: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
                        case CubeFace::positiveZ: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
                        case CubeFace::negativeZ: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
                        default: throw std::runtime_error("Invalid cube face");
                    }
                }
            }

            Texture::Texture(RenderDevice& initRenderDevice,
                             const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& initLevels,
                             TextureType type,
                             Flags initFlags,
                             std::uint32_t initSampleCount,
                             PixelFormat initPixelFormat,
                             SamplerFilter initFilter,
                             std::uint32_t initMaxAnisotropy):
                RenderResource(initRenderDevice),
                levels(initLevels),
                flags(initFlags),
                mipmaps(static_cast<std::uint32_t>(initLevels.size())),
                sampleCount(initSampleCount),
                textureTarget(getTextureTarget(type)),
                internalPixelFormat(getOpenGlInternalPixelFormat(initPixelFormat, renderDevice.getAPIMajorVersion())),
                pixelFormat(getOpenGlPixelFormat(initPixelFormat)),
                pixelType(getOpenGlPixelType(initPixelFormat)),
                filter(initFilter),
                maxAnisotropy(static_cast<GLint>(initMaxAnisotropy))
            {
                if ((flags & Flags::bindRenderTarget) == Flags::bindRenderTarget &&
                    (mipmaps == 0 || mipmaps > 1))
                    throw std::runtime_error("Invalid mip map count");

                if (internalPixelFormat == GL_NONE)
                    throw std::runtime_error("Invalid pixel format");

                if (pixelFormat == GL_NONE)
                    throw std::runtime_error("Invalid pixel format");

                if (pixelType == GL_NONE)
                    throw std::runtime_error("Invalid pixel format");

                createTexture();

                renderDevice.bindTexture(textureTarget, 0, textureId);

                if ((flags & Flags::bindRenderTarget) != Flags::bindRenderTarget)
                {
                    if (!levels.empty())
                    {
                        if (renderDevice.isTextureBaseLevelSupported()) renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
                        if (renderDevice.isTextureMaxLevelSupported()) renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                        GLenum error;

                        if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to set texture base and max levels");
                    }

                    for (std::size_t level = 0; level < levels.size(); ++level)
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

                if ((flags & Flags::bindRenderTarget) != Flags::bindRenderTarget)
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

                    for (std::size_t level = 0; level < levels.size(); ++level)
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

            void Texture::setData(const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& newLevels)
            {
                if ((flags & Flags::dynamic) != Flags::dynamic ||
                    (flags & Flags::bindRenderTarget) == Flags::bindRenderTarget)
                    throw std::runtime_error("Texture is not dynamic");

                levels = newLevels;

                if (!textureId)
                    throw std::runtime_error("Texture not initialized");

                renderDevice.bindTexture(textureTarget, 0, textureId);

                for (std::size_t level = 0; level < levels.size(); ++level)
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

                switch (filter)
                {
                    case SamplerFilter::point:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        break;
                    case SamplerFilter::linear:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        break;
                    case SamplerFilter::bilinear:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        break;
                    case SamplerFilter::trilinear:
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

            void Texture::setMaxAnisotropy(std::uint32_t newMaxAnisotropy)
            {
                maxAnisotropy = static_cast<GLint>(newMaxAnisotropy);

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

                if ((flags & Flags::bindRenderTarget) == Flags::bindRenderTarget &&
                    renderDevice.isRenderTargetsSupported())
                {
                    if ((flags & Flags::bindShader) == Flags::bindShader)
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

                switch (filter)
                {
                    case SamplerFilter::point:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        break;
                    case SamplerFilter::linear:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        break;
                    case SamplerFilter::bilinear:
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        break;
                    case SamplerFilter::trilinear:
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

                if (maxAnisotropy > 1 && renderDevice.isAnisotropicFilteringSupported())
                {
                    renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set texture max anisotrophy");
                }
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
