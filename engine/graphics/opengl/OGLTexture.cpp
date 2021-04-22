// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLTexture.hpp"
#include "OGLError.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel::graphics::opengl
{
    namespace
    {
        constexpr GLenum getOpenGlInternalPixelFormat(PixelFormat pixelFormat, std::uint32_t openGlVersion)
        {
#if OUZEL_OPENGLES
            if (openGlVersion >= 3)
            {
                switch (pixelFormat)
                {
                    case PixelFormat::a8UnsignedNorm: return GL_ALPHA8_OES;
                    case PixelFormat::r8UnsignedNorm: return GL_R8;
                    case PixelFormat::r8SignedNorm: return GL_R8_SNORM;
                    case PixelFormat::r8UnsignedInt: return GL_R8UI;
                    case PixelFormat::r8SignedInt: return GL_R8I;
                    case PixelFormat::r16UnsignedNorm: return GL_NONE;
                    case PixelFormat::r16SignedNorm: return GL_NONE;
                    case PixelFormat::r16UnsignedInt: return GL_R16UI;
                    case PixelFormat::r16SignedInt: return GL_R16I;
                    case PixelFormat::r16Float: return GL_R16F;
                    case PixelFormat::r32UnsignedInt: return GL_R32UI;
                    case PixelFormat::r32SignedInt: return GL_R32I;
                    case PixelFormat::r32Float: return GL_R32F;
                    case PixelFormat::rg8UnsignedNorm: return GL_RG8;
                    case PixelFormat::rg8SignedNorm: return GL_RG8_SNORM;
                    case PixelFormat::rg8UnsignedInt: return GL_RG8UI;
                    case PixelFormat::rg8SignedInt: return GL_RG8I;
                    case PixelFormat::rgba8UnsignedNorm: return GL_RGBA8;
                    case PixelFormat::rgba8UnsignedNormSRGB: return GL_SRGB8_ALPHA8;
                    case PixelFormat::rgba8SignedNorm: return GL_RGBA8_SNORM;
                    case PixelFormat::rgba8UnsignedInt: return GL_RGBA8UI;
                    case PixelFormat::rgba8SignedInt: return GL_RGBA8I;
                    case PixelFormat::rgba16UnsignedNorm: return GL_NONE;
                    case PixelFormat::rgba16SignedNorm: return GL_NONE;
                    case PixelFormat::rgba16UnsignedInt: return GL_RGBA16UI;
                    case PixelFormat::rgba16SignedInt: return GL_RGBA16I;
                    case PixelFormat::rgba16Float: return GL_RGBA16F;
                    case PixelFormat::rgba32UnsignedInt: return GL_RGBA32UI;
                    case PixelFormat::rgba32SignedInt: return GL_RGBA32I;
                    case PixelFormat::rgba32Float: return GL_RGBA32F;
                    case PixelFormat::depth: return GL_DEPTH_COMPONENT24;
                    case PixelFormat::depthStencil: return GL_DEPTH24_STENCIL8;
                    default: throw Error("Invalid pixel format");
                }
            }
            else
            {
                switch (pixelFormat)
                {
                    case PixelFormat::a8UnsignedNorm: return GL_ALPHA;
                    case PixelFormat::rgba8UnsignedNorm: return GL_RGBA;
                    case PixelFormat::depth: return GL_DEPTH_COMPONENT24;
                    case PixelFormat::depthStencil: return GL_DEPTH24_STENCIL8;
                    default: throw Error("Invalid pixel format");
                }
            }
#else
            static_cast<void>(openGlVersion);

            switch (pixelFormat)
            {
                case PixelFormat::a8UnsignedNorm: return GL_ALPHA8_EXT;

                case PixelFormat::r8UnsignedNorm: return GL_R8;
                case PixelFormat::r8SignedNorm: return GL_R8_SNORM;
                case PixelFormat::r8UnsignedInt: return GL_R8UI;
                case PixelFormat::r8SignedInt: return GL_R8I;
                case PixelFormat::r16UnsignedNorm: return GL_R16;
                case PixelFormat::r16SignedNorm: return GL_R16_SNORM;
                case PixelFormat::r16UnsignedInt: return GL_R16UI;
                case PixelFormat::r16SignedInt: return GL_R16I;
                case PixelFormat::r16Float: return GL_R16F;
                case PixelFormat::r32UnsignedInt: return GL_R32UI;
                case PixelFormat::r32SignedInt: return GL_R32I;
                case PixelFormat::r32Float: return GL_R32F;
                case PixelFormat::rg8UnsignedNorm: return GL_RG8;
                case PixelFormat::rg8SignedNorm: return GL_RG8_SNORM;
                case PixelFormat::rg8UnsignedInt: return GL_RG8UI;
                case PixelFormat::rg8SignedInt: return GL_RG8I;
                case PixelFormat::rgba8UnsignedNorm: return GL_RGBA8;
                case PixelFormat::rgba8UnsignedNormSRGB: return GL_SRGB8_ALPHA8;
                case PixelFormat::rgba8SignedNorm: return GL_RGBA8_SNORM;
                case PixelFormat::rgba8UnsignedInt: return GL_RGBA8UI;
                case PixelFormat::rgba8SignedInt: return GL_RGBA8I;
                case PixelFormat::rgba16UnsignedNorm: return GL_RGBA16;
                case PixelFormat::rgba16SignedNorm: return GL_RGBA16_SNORM;
                case PixelFormat::rgba16UnsignedInt: return GL_RGBA16UI;
                case PixelFormat::rgba16SignedInt: return GL_RGBA16I;
                case PixelFormat::rgba16Float: return GL_RGBA16F;
                case PixelFormat::rgba32UnsignedInt: return GL_RGBA32UI;
                case PixelFormat::rgba32SignedInt: return GL_RGBA32I;
                case PixelFormat::rgba32Float: return GL_RGBA32F;
                case PixelFormat::depth: return GL_DEPTH_COMPONENT24;
                case PixelFormat::depthStencil: return GL_DEPTH24_STENCIL8;
                default: throw Error("Invalid pixel format");
            }
#endif
        }

        constexpr GLenum getOpenGlPixelFormat(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::a8UnsignedNorm:
                    return GL_ALPHA;
                case PixelFormat::r8UnsignedNorm:
                case PixelFormat::r8SignedNorm:
                case PixelFormat::r16UnsignedNorm:
                case PixelFormat::r16SignedNorm:
                case PixelFormat::r16Float:
                case PixelFormat::r32Float:
                    return GL_RED;
                case PixelFormat::r8UnsignedInt:
                case PixelFormat::r8SignedInt:
                case PixelFormat::r16UnsignedInt:
                case PixelFormat::r16SignedInt:
                case PixelFormat::r32UnsignedInt:
                case PixelFormat::r32SignedInt:
                    return GL_RED_INTEGER;
                case PixelFormat::rg8UnsignedNorm:
                case PixelFormat::rg8SignedNorm:
                    return GL_RG;
                case PixelFormat::rg8UnsignedInt:
                case PixelFormat::rg8SignedInt:
                    return GL_RG_INTEGER;
                case PixelFormat::rgba8UnsignedNorm:
                case PixelFormat::rgba8UnsignedNormSRGB:
                case PixelFormat::rgba8SignedNorm:
                case PixelFormat::rgba16UnsignedNorm:
                case PixelFormat::rgba16SignedNorm:
                case PixelFormat::rgba16Float:
                case PixelFormat::rgba32Float:
                    return GL_RGBA;
                case PixelFormat::rgba8UnsignedInt:
                case PixelFormat::rgba8SignedInt:
                case PixelFormat::rgba16UnsignedInt:
                case PixelFormat::rgba16SignedInt:
                case PixelFormat::rgba32UnsignedInt:
                case PixelFormat::rgba32SignedInt:
                    return GL_RGBA_INTEGER;
                case PixelFormat::depth:
                    return GL_DEPTH_COMPONENT;
                case PixelFormat::depthStencil:
                    return GL_DEPTH_STENCIL;
                default:
                    throw Error("Invalid pixel format");
            }
        }

        constexpr GLenum getOpenGlPixelType(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::a8UnsignedNorm:
                case PixelFormat::r8UnsignedNorm:
                case PixelFormat::r16UnsignedNorm:
                case PixelFormat::rg8UnsignedNorm:
                case PixelFormat::rgba8UnsignedNorm:
                case PixelFormat::rgba8UnsignedNormSRGB:
                case PixelFormat::rgba16UnsignedNorm:
                    return GL_UNSIGNED_BYTE;
                case PixelFormat::r8SignedNorm:
                case PixelFormat::r16SignedNorm:
                case PixelFormat::rg8SignedNorm:
                case PixelFormat::rgba8SignedNorm:
                case PixelFormat::rgba16SignedNorm:
                    return GL_BYTE;
                case PixelFormat::r8UnsignedInt:
                case PixelFormat::r16UnsignedInt:
                case PixelFormat::r32UnsignedInt:
                case PixelFormat::rg8UnsignedInt:
                case PixelFormat::rgba8UnsignedInt:
                case PixelFormat::rgba16UnsignedInt:
                case PixelFormat::rgba32UnsignedInt:
                    return GL_UNSIGNED_INT;
                case PixelFormat::r8SignedInt:
                case PixelFormat::r16SignedInt:
                case PixelFormat::r32SignedInt:
                case PixelFormat::rg8SignedInt:
                case PixelFormat::rgba8SignedInt:
                case PixelFormat::rgba16SignedInt:
                case PixelFormat::rgba32SignedInt:
                    return GL_INT;
                case PixelFormat::r16Float:
                case PixelFormat::r32Float:
                case PixelFormat::rgba16Float:
                case PixelFormat::rgba32Float:
                case PixelFormat::depth:
                    return GL_UNSIGNED_INT;
                case PixelFormat::depthStencil:
                    return GL_UNSIGNED_INT_24_8;
                default:
                    throw Error("Invalid pixel format");
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
                    throw Error("Invalid texture address mode");
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
                default: throw Error("Invalid texture type");
            }
        }

        [[maybe_unused]]
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
                default: throw Error("Invalid cube face");
            }
        }
    }

    Texture::Texture(RenderDevice& initRenderDevice,
                     const std::vector<std::pair<Size<std::uint32_t, 2>, std::vector<std::uint8_t>>>& initLevels,
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
        filter(initFilter),
        maxAnisotropy(static_cast<GLint>(initMaxAnisotropy)),
        textureTarget(getTextureTarget(type)),
        internalPixelFormat(getOpenGlInternalPixelFormat(initPixelFormat, renderDevice.getAPIMajorVersion())),
        pixelFormat(getOpenGlPixelFormat(initPixelFormat)),
        pixelType(getOpenGlPixelType(initPixelFormat))
    {
        if ((flags & Flags::bindRenderTarget) == Flags::bindRenderTarget &&
            (mipmaps == 0 || mipmaps > 1))
            throw Error("Invalid mip map count");

        if (internalPixelFormat == GL_NONE)
            throw Error("Invalid pixel format");

        if (pixelFormat == GL_NONE)
            throw Error("Invalid pixel format");

        if (pixelType == GL_NONE)
            throw Error("Invalid pixel format");

        createTexture();

        renderDevice.bindTexture(textureTarget, 0, textureId);

        if ((flags & Flags::bindRenderTarget) != Flags::bindRenderTarget)
        {
            if (!levels.empty())
            {
                if (renderDevice.isTextureBaseLevelSupported()) renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
                if (renderDevice.isTextureMaxLevelSupported()) renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
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

            if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
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

                if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
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

            if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to upload texture data");

            setTextureParameters();
        }
    }

    void Texture::setData(const std::vector<std::pair<Size<std::uint32_t, 2>, std::vector<std::uint8_t>>>& newLevels)
    {
        if ((flags & Flags::dynamic) != Flags::dynamic ||
            (flags & Flags::bindRenderTarget) == Flags::bindRenderTarget)
            throw Error("Texture is not dynamic");

        levels = newLevels;

        if (!textureId)
            throw Error("Texture not initialized");

        renderDevice.bindTexture(textureTarget, 0, textureId);

        for (std::size_t level = 0; level < levels.size(); ++level)
            if (!levels[level].second.empty())
                renderDevice.glTexSubImage2DProc(GL_TEXTURE_2D, static_cast<GLint>(level), 0, 0,
                                                 static_cast<GLsizei>(levels[level].first.v[0]),
                                                 static_cast<GLsizei>(levels[level].first.v[1]),
                                                 pixelFormat, pixelType,
                                                 levels[level].second.data());

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to upload texture data");
    }

    void Texture::setFilter(SamplerFilter newFilter)
    {
        filter = newFilter;

        if (!textureId)
            throw Error("Texture not initialized");

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
                throw Error("Invalid texture filter");
        }

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to set texture filter");
    }

    void Texture::setAddressX(SamplerAddressMode newAddressX)
    {
        addressX = newAddressX;

        if (!textureId)
            throw Error("Texture not initialized");

        renderDevice.bindTexture(textureTarget, 0, textureId);
        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_S, getWrapMode(addressX));

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");
    }

    void Texture::setAddressY(SamplerAddressMode newAddressY)
    {
        addressY = newAddressY;

        if (!textureId)
            throw Error("Texture not initialized");

        renderDevice.bindTexture(textureTarget, 0, textureId);
        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_T, getWrapMode(addressY));

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");
    }

    void Texture::setAddressZ(SamplerAddressMode newAddressZ)
    {
        addressZ = newAddressZ;

        if (textureTarget == GL_TEXTURE_3D)
        {
            if (!textureId)
                throw Error("Texture not initialized");

            renderDevice.bindTexture(textureTarget, 0, textureId);
            renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_R, getWrapMode(addressZ));

            if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");
        }
    }

    void Texture::setMaxAnisotropy(std::uint32_t newMaxAnisotropy)
    {
        maxAnisotropy = static_cast<GLint>(newMaxAnisotropy);

        if (!textureId)
            throw Error("Texture not initialized");

        renderDevice.bindTexture(textureTarget, 0, textureId);

        if (maxAnisotropy > 1 && renderDevice.isAnisotropicFilteringSupported())
        {
            renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(maxAnisotropy));

            if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
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

                if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
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

                    if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
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

                    if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set color render buffer's multisample storage");
                }
                else
                {
                    renderDevice.glRenderbufferStorageProc(GL_RENDERBUFFER,
                                                           internalPixelFormat,
                                                           width, height);

                    if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set color render buffer's storage");
                }
            }
        }
        else
        {
            renderDevice.glGenTexturesProc(1, &textureId);

            if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
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
                throw Error("Invalid texture filter");
        }

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to set texture filter");

        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_S, getWrapMode(addressX));

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");

        renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_T, getWrapMode(addressY));

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");

        if (textureTarget == GL_TEXTURE_3D)
        {
            renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_WRAP_R, getWrapMode(addressZ));

            if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set texture wrap mode");
        }

        if (maxAnisotropy > 1 && renderDevice.isAnisotropicFilteringSupported())
        {
            renderDevice.glTexParameteriProc(textureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

            if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set texture max anisotrophy");
        }
    }
}

#endif
