// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_PIXELFORMAT_HPP
#define OUZEL_GRAPHICS_PIXELFORMAT_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class PixelFormat
        {
            Default,
            A8UNorm,
            R8UNorm,
            R8SNorm,
            R8UInt,
            R8SInt,
            R16UNorm,
            R16SNorm,
            R16UInt,
            R16SInt,
            R16Float,
            R32UInt,
            R32SInt,
            R32Float,
            RG8UNorm,
            RG8SNorm,
            RG8UInt,
            RG8SInt,
            RGBA8UNorm,
            RGBA8UNormSRGB,
            RGBA8SNorm,
            RGBA8UInt,
            RGBA8SInt,
            RGBA16UNorm,
            RGBA16SNorm,
            RGBA16UInt,
            RGBA16SInt,
            RGBA16Float,
            RGBA32UInt,
            RGBA32SInt,
            RGBA32Float,
            Depth,
            DepthStencil
        };

        inline std::uint32_t getPixelSize(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::A8UNorm:
                case PixelFormat::R8UNorm:
                case PixelFormat::R8SNorm:
                case PixelFormat::R8UInt:
                case PixelFormat::R8SInt:
                    return 1;
                case PixelFormat::R16UNorm:
                case PixelFormat::R16SNorm:
                case PixelFormat::R16UInt:
                case PixelFormat::R16SInt:
                case PixelFormat::R16Float:
                    return 2;
                case PixelFormat::R32UInt:
                case PixelFormat::R32SInt:
                case PixelFormat::R32Float:
                    return 4;
                case PixelFormat::RG8UNorm:
                case PixelFormat::RG8SNorm:
                case PixelFormat::RG8UInt:
                case PixelFormat::RG8SInt:
                    return 2;
                case PixelFormat::RGBA8UNorm:
                case PixelFormat::RGBA8UNormSRGB:
                case PixelFormat::RGBA8SNorm:
                case PixelFormat::RGBA8UInt:
                case PixelFormat::RGBA8SInt:
                    return 4;
                case PixelFormat::RGBA16UNorm:
                case PixelFormat::RGBA16SNorm:
                case PixelFormat::RGBA16UInt:
                case PixelFormat::RGBA16SInt:
                case PixelFormat::RGBA16Float:
                    return 8;
                case PixelFormat::RGBA32UInt:
                case PixelFormat::RGBA32SInt:
                case PixelFormat::RGBA32Float:
                    return 16;
                case PixelFormat::Depth:
                case PixelFormat::DepthStencil:
                    return 4;
                case PixelFormat::Default:
                    return 0;
                default:
                    return 0;
            }
        }

        inline std::uint32_t getChannelSize(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::A8UNorm:
                case PixelFormat::R8UNorm:
                case PixelFormat::R8SNorm:
                case PixelFormat::R8UInt:
                case PixelFormat::R8SInt:
                case PixelFormat::RG8UNorm:
                case PixelFormat::RG8SNorm:
                case PixelFormat::RG8UInt:
                case PixelFormat::RG8SInt:
                case PixelFormat::RGBA8UNorm:
                case PixelFormat::RGBA8UNormSRGB:
                case PixelFormat::RGBA8SNorm:
                case PixelFormat::RGBA8UInt:
                case PixelFormat::RGBA8SInt:
                    return 1;
                case PixelFormat::R16UNorm:
                case PixelFormat::R16SNorm:
                case PixelFormat::R16UInt:
                case PixelFormat::R16SInt:
                case PixelFormat::R16Float:
                case PixelFormat::RGBA16UNorm:
                case PixelFormat::RGBA16SNorm:
                case PixelFormat::RGBA16UInt:
                case PixelFormat::RGBA16SInt:
                case PixelFormat::RGBA16Float:
                    return 2;
                case PixelFormat::R32UInt:
                case PixelFormat::R32SInt:
                case PixelFormat::R32Float:
                case PixelFormat::RGBA32UInt:
                case PixelFormat::RGBA32SInt:
                case PixelFormat::RGBA32Float:
                    return 4;
                case PixelFormat::Depth:
                case PixelFormat::DepthStencil:
                    return 4;
                case PixelFormat::Default:
                    return 0;
                default:
                    return 0;
            }
        }

        inline std::uint32_t getChannelCount(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::A8UNorm:
                case PixelFormat::R8UNorm:
                case PixelFormat::R8SNorm:
                case PixelFormat::R8UInt:
                case PixelFormat::R8SInt:
                case PixelFormat::R16UNorm:
                case PixelFormat::R16SNorm:
                case PixelFormat::R16UInt:
                case PixelFormat::R16SInt:
                case PixelFormat::R16Float:
                case PixelFormat::R32UInt:
                case PixelFormat::R32SInt:
                case PixelFormat::R32Float:
                    return 1;
                case PixelFormat::RG8UNorm:
                case PixelFormat::RG8SNorm:
                case PixelFormat::RG8UInt:
                case PixelFormat::RG8SInt:
                    return 2;
                case PixelFormat::RGBA8UNorm:
                case PixelFormat::RGBA8UNormSRGB:
                case PixelFormat::RGBA8SNorm:
                case PixelFormat::RGBA8UInt:
                case PixelFormat::RGBA8SInt:
                case PixelFormat::RGBA16UNorm:
                case PixelFormat::RGBA16SNorm:
                case PixelFormat::RGBA16UInt:
                case PixelFormat::RGBA16SInt:
                case PixelFormat::RGBA16Float:
                case PixelFormat::RGBA32UInt:
                case PixelFormat::RGBA32SInt:
                case PixelFormat::RGBA32Float:
                    return 4;
                case PixelFormat::Depth:
                case PixelFormat::DepthStencil:
                    return 1;
                case PixelFormat::Default:
                    return 0;
                default:
                    return 0;
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_PIXELFORMAT_HPP
