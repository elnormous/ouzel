// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_PIXELFORMAT_HPP
#define OUZEL_GRAPHICS_PIXELFORMAT_HPP

namespace ouzel::graphics
{
    enum class PixelFormat
    {
        a8UnsignedNorm,
        r8UnsignedNorm,
        r8SignedNorm,
        r8UnsignedInt,
        r8SignedInt,
        r16UnsignedNorm,
        r16SignedNorm,
        r16UnsignedInt,
        r16SignedInt,
        r16Float,
        r32UnsignedInt,
        r32SignedInt,
        r32Float,
        rg8UnsignedNorm,
        rg8SignedNorm,
        rg8UnsignedInt,
        rg8SignedInt,
        rgba8UnsignedNorm,
        rgba8UnsignedNormSRGB,
        rgba8SignedNorm,
        rgba8UnsignedInt,
        rgba8SignedInt,
        rgba16UnsignedNorm,
        rgba16SignedNorm,
        rgba16UnsignedInt,
        rgba16SignedInt,
        rgba16Float,
        rgba32UnsignedInt,
        rgba32SignedInt,
        rgba32Float,
        depth,
        depthStencil
    };

    inline std::uint32_t getPixelSize(PixelFormat pixelFormat) noexcept
    {
        switch (pixelFormat)
        {
            case PixelFormat::a8UnsignedNorm:
            case PixelFormat::r8UnsignedNorm:
            case PixelFormat::r8SignedNorm:
            case PixelFormat::r8UnsignedInt:
            case PixelFormat::r8SignedInt:
                return 1;
            case PixelFormat::r16UnsignedNorm:
            case PixelFormat::r16SignedNorm:
            case PixelFormat::r16UnsignedInt:
            case PixelFormat::r16SignedInt:
            case PixelFormat::r16Float:
                return 2;
            case PixelFormat::r32UnsignedInt:
            case PixelFormat::r32SignedInt:
            case PixelFormat::r32Float:
                return 4;
            case PixelFormat::rg8UnsignedNorm:
            case PixelFormat::rg8SignedNorm:
            case PixelFormat::rg8UnsignedInt:
            case PixelFormat::rg8SignedInt:
                return 2;
            case PixelFormat::rgba8UnsignedNorm:
            case PixelFormat::rgba8UnsignedNormSRGB:
            case PixelFormat::rgba8SignedNorm:
            case PixelFormat::rgba8UnsignedInt:
            case PixelFormat::rgba8SignedInt:
                return 4;
            case PixelFormat::rgba16UnsignedNorm:
            case PixelFormat::rgba16SignedNorm:
            case PixelFormat::rgba16UnsignedInt:
            case PixelFormat::rgba16SignedInt:
            case PixelFormat::rgba16Float:
                return 8;
            case PixelFormat::rgba32UnsignedInt:
            case PixelFormat::rgba32SignedInt:
            case PixelFormat::rgba32Float:
                return 16;
            case PixelFormat::depth:
            case PixelFormat::depthStencil:
                return 4;
            default:
                return 0;
        }
    }

    inline std::uint32_t getChannelSize(PixelFormat pixelFormat) noexcept
    {
        switch (pixelFormat)
        {
            case PixelFormat::a8UnsignedNorm:
            case PixelFormat::r8UnsignedNorm:
            case PixelFormat::r8SignedNorm:
            case PixelFormat::r8UnsignedInt:
            case PixelFormat::r8SignedInt:
            case PixelFormat::rg8UnsignedNorm:
            case PixelFormat::rg8SignedNorm:
            case PixelFormat::rg8UnsignedInt:
            case PixelFormat::rg8SignedInt:
            case PixelFormat::rgba8UnsignedNorm:
            case PixelFormat::rgba8UnsignedNormSRGB:
            case PixelFormat::rgba8SignedNorm:
            case PixelFormat::rgba8UnsignedInt:
            case PixelFormat::rgba8SignedInt:
                return 1;
            case PixelFormat::r16UnsignedNorm:
            case PixelFormat::r16SignedNorm:
            case PixelFormat::r16UnsignedInt:
            case PixelFormat::r16SignedInt:
            case PixelFormat::r16Float:
            case PixelFormat::rgba16UnsignedNorm:
            case PixelFormat::rgba16SignedNorm:
            case PixelFormat::rgba16UnsignedInt:
            case PixelFormat::rgba16SignedInt:
            case PixelFormat::rgba16Float:
                return 2;
            case PixelFormat::r32UnsignedInt:
            case PixelFormat::r32SignedInt:
            case PixelFormat::r32Float:
            case PixelFormat::rgba32UnsignedInt:
            case PixelFormat::rgba32SignedInt:
            case PixelFormat::rgba32Float:
                return 4;
            case PixelFormat::depth:
            case PixelFormat::depthStencil:
                return 4;
            default:
                return 0;
        }
    }

    inline std::uint32_t getChannelCount(PixelFormat pixelFormat) noexcept
    {
        switch (pixelFormat)
        {
            case PixelFormat::a8UnsignedNorm:
            case PixelFormat::r8UnsignedNorm:
            case PixelFormat::r8SignedNorm:
            case PixelFormat::r8UnsignedInt:
            case PixelFormat::r8SignedInt:
            case PixelFormat::r16UnsignedNorm:
            case PixelFormat::r16SignedNorm:
            case PixelFormat::r16UnsignedInt:
            case PixelFormat::r16SignedInt:
            case PixelFormat::r16Float:
            case PixelFormat::r32UnsignedInt:
            case PixelFormat::r32SignedInt:
            case PixelFormat::r32Float:
                return 1;
            case PixelFormat::rg8UnsignedNorm:
            case PixelFormat::rg8SignedNorm:
            case PixelFormat::rg8UnsignedInt:
            case PixelFormat::rg8SignedInt:
                return 2;
            case PixelFormat::rgba8UnsignedNorm:
            case PixelFormat::rgba8UnsignedNormSRGB:
            case PixelFormat::rgba8SignedNorm:
            case PixelFormat::rgba8UnsignedInt:
            case PixelFormat::rgba8SignedInt:
            case PixelFormat::rgba16UnsignedNorm:
            case PixelFormat::rgba16SignedNorm:
            case PixelFormat::rgba16UnsignedInt:
            case PixelFormat::rgba16SignedInt:
            case PixelFormat::rgba16Float:
            case PixelFormat::rgba32UnsignedInt:
            case PixelFormat::rgba32SignedInt:
            case PixelFormat::rgba32Float:
                return 4;
            case PixelFormat::depth:
            case PixelFormat::depthStencil:
                return 1;
            default:
                return 0;
        }
    }
}

#endif // OUZEL_GRAPHICS_PIXELFORMAT_HPP
