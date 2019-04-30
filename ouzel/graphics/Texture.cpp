// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "Texture.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

static constexpr float GAMMA = 2.2F;

namespace ouzel
{
    namespace graphics
    {
        static void downsample2x2A8(uint32_t width, uint32_t height,
                                    const std::vector<float>& original, std::vector<float>& resized)
        {
            const uint32_t dstWidth = width >> 1;
            const uint32_t dstHeight = height >> 1;
            const uint32_t pitch = width * 1;
            resized.resize(dstWidth * dstHeight * 1);
            const float* src = original.data();
            float* dst = resized.data();

            if (dstWidth > 0 && dstHeight > 0)
            {
                for (uint32_t y = 0; y < dstHeight; ++y, src += pitch * 2)
                {
                    const float* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                    {
                        float a = 0.0F;
                        a += pixel[0];
                        a += pixel[1];
                        a += pixel[pitch + 0];
                        a += pixel[pitch + 1];
                        dst[0] = a / 4.0F;
                    }
                }
            }
            else if (dstHeight > 0)
            {
                for (uint32_t y = 0; y < dstHeight; ++y, src += pitch * 2, dst += 1)
                {
                    const float* pixel = src;

                    float a = 0.0F;
                    a += pixel[0];
                    a += pixel[pitch + 0];
                    dst[0] = a / 2.0F;
                }
            }
            else if (dstWidth > 0)
            {
                const float* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                {
                    float a = 0.0F;
                    a += pixel[0];
                    a += pixel[1];
                    dst[0] = a / 2.0F;
                }
            }
        }

        static void downsample2x2R8(uint32_t width, uint32_t height,
                                    const std::vector<float>& original, std::vector<float>& resized)
        {
            std::vector<float> normalized(width * height * 1);

            const uint32_t dstWidth = width >> 1;
            const uint32_t dstHeight = height >> 1;
            const uint32_t pitch = width * 1;
            resized.resize(dstWidth * dstHeight * 1);
            const float* src = original.data();
            float* dst = resized.data();

            if (dstWidth > 0 && dstHeight > 0)
            {
                for (uint32_t y = 0; y < dstHeight; ++y, src += pitch * 2)
                {
                    const float* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                    {
                        float r = 0.0F;
                        r += pixel[0];
                        r += pixel[1];
                        r += pixel[pitch + 0];
                        r += pixel[pitch + 1];
                        dst[0] = r / 4.0F;
                    }
                }
            }
            else if (dstHeight > 0)
            {
                for (uint32_t y = 0; y < dstHeight; ++y, src += pitch * 2, dst += 1)
                {
                    const float* pixel = src;

                    float r = 0.0F;
                    r += pixel[0];
                    r += pixel[pitch + 0];
                    dst[0] = r / 2.0F;
                }
            }
            else if (dstWidth > 0)
            {
                const float* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                {
                    float r = 0.0F;
                    r += pixel[0];
                    r += pixel[1];
                    dst[0] = r / 2.0F;
                }
            }
        }

        static void downsample2x2Rg8(uint32_t width, uint32_t height,
                                     const std::vector<float>& original, std::vector<float>& resized)
        {
            std::vector<float> normalized(width * height * 2);

            const uint32_t dstWidth = width >> 1;
            const uint32_t dstHeight = height >> 1;
            const uint32_t pitch = width * 2;
            resized.resize(dstWidth * dstHeight * 2);
            const float* src = original.data();
            float* dst = resized.data();

            if (dstWidth > 0 && dstHeight > 0)
            {
                for (uint32_t  y = 0; y < dstHeight; ++y, src += pitch * 2)
                {
                    const float* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 4, dst += 2)
                    {
                        float r = 0.0F;
                        float g = 0.0F;

                        r += pixel[0];
                        g += pixel[1];

                        r += pixel[2];
                        g += pixel[3];

                        r += pixel[pitch + 0];
                        g += pixel[pitch + 1];

                        r += pixel[pitch + 2];
                        g += pixel[pitch + 3];

                        dst[0] = r / 4.0F;
                        dst[1] = g / 4.0F;
                    }
                }
            }
            else if (dstHeight > 0)
            {
                for (uint32_t y = 0; y < dstHeight; ++y, src += pitch * 2, dst += 2)
                {
                    const float* pixel = src;
                    float r = 0.0F;
                    float g = 0.0F;

                    r += pixel[0];
                    g += pixel[1];

                    r += pixel[pitch + 0];
                    g += pixel[pitch + 1];

                    dst[0] = r / 2.0F;
                    dst[1] = g / 2.0F;
                }
            }
            else if (dstWidth > 0)
            {
                const float* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 4, dst += 2)
                {
                    float r = 0.0F;
                    float g = 0.0F;

                    r += pixel[0];
                    g += pixel[1];

                    r += pixel[2];
                    g += pixel[3];

                    dst[0] = r / 2.0F;
                    dst[1] = g / 2.0F;
                }
            }
        }

        static void downsample2x2Rgba8(uint32_t width, uint32_t height,
                                       const std::vector<float>& original, std::vector<float>& resized)
        {
            const uint32_t dstWidth = width >> 1;
            const uint32_t dstHeight = height >> 1;
            const uint32_t pitch = width * 4;
            resized.resize(dstWidth * dstHeight * 4);
            const float* src = original.data();
            float* dst = resized.data();

            if (dstWidth > 0 && dstHeight > 0)
            {
                for (uint32_t y = 0; y < dstHeight; ++y, src += pitch * 2)
                {
                    const float* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 8, dst += 4)
                    {
                        float pixels = 0.0F;
                        float r = 0.0F;
                        float g = 0.0F;
                        float b = 0.0F;
                        float a = 0.0F;

                        if (pixel[3] > 0.0F)
                        {
                            r += pixel[0];
                            g += pixel[1];
                            b += pixel[2];
                            pixels += 1.0F;
                        }
                        a += pixel[3];

                        if (pixel[7] > 0.0F)
                        {
                            r += pixel[4];
                            g += pixel[5];
                            b += pixel[6];
                            pixels += 1.0F;
                        }
                        a += pixel[7];

                        if (pixel[pitch + 3] > 0.0F)
                        {
                            r += pixel[pitch + 0];
                            g += pixel[pitch + 1];
                            b += pixel[pitch + 2];
                            pixels += 1.0F;
                        }
                        a += pixel[pitch + 3];

                        if (pixel[pitch + 7] > 0.0F)
                        {
                            r += pixel[pitch + 4];
                            g += pixel[pitch + 5];
                            b += pixel[pitch + 6];
                            pixels += 1.0F;
                        }
                        a += pixel[pitch + 7];

                        if (pixels > 0.0F)
                        {
                            dst[0] = r / pixels;
                            dst[1] = g / pixels;
                            dst[2] = b / pixels;
                            dst[3] = a / 4.0F;
                        }
                        else
                        {
                            dst[0] = 0;
                            dst[1] = 0;
                            dst[2] = 0;
                            dst[3] = 0;
                        }
                    }
                }
            }
            else if (dstHeight > 0)
            {
                for (uint32_t y = 0; y < dstHeight; ++y, src += pitch * 2, dst += 4)
                {
                    const float* pixel = src;

                    float pixels = 0.0F;
                    float r = 0.0F;
                    float g = 0.0F;
                    float b = 0.0F;
                    float a = 0.0F;

                    if (pixel[3] > 0)
                    {
                        r += pixel[0];
                        g += pixel[1];
                        b += pixel[2];
                        pixels += 1.0F;
                    }
                    a = pixel[3];

                    if (pixel[pitch + 3] > 0)
                    {
                        r += pixel[pitch + 0];
                        g += pixel[pitch + 1];
                        b += pixel[pitch + 2];
                        pixels += 1.0F;
                    }
                    a += pixel[pitch + 3];

                    if (pixels > 0.0F)
                    {
                        dst[0] = r / pixels;
                        dst[1] = g / pixels;
                        dst[2] = b / pixels;
                        dst[3] = a / 2.0F;
                    }
                    else
                    {
                        dst[0] = 0;
                        dst[1] = 0;
                        dst[2] = 0;
                        dst[3] = 0;
                    }
                }
            }
            else if (dstWidth > 0)
            {
                const float* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 8, dst += 4)
                {
                    float pixels = 0.0F;
                    float r = 0.0F;
                    float g = 0.0F;
                    float b = 0.0F;
                    float a = 0.0F;

                    if (pixel[3] > 0)
                    {
                        r += pixel[0];
                        g += pixel[1];
                        b += pixel[2];
                        pixels += 1.0F;
                    }
                    a += pixel[3];

                    if (pixel[7] > 0)
                    {
                        r += pixel[4];
                        g += pixel[5];
                        b += pixel[6];
                        pixels += 1.0F;
                    }
                    a += pixel[7];

                    if (pixels > 0.0F)
                    {
                        dst[0] = r / pixels;
                        dst[1] = g / pixels;
                        dst[2] = b / pixels;
                        dst[3] = a / 2.0F;
                    }
                    else
                    {
                        dst[0] = 0;
                        dst[1] = 0;
                        dst[2] = 0;
                        dst[3] = 0;
                    }
                }
            }
        }

        static inline float gammaDecode(uint8_t value)
        {
            return pow(value / 255.0F, GAMMA);
        }

        static inline uint8_t gammaEncode(float value)
        {
            return static_cast<uint8_t>(round(pow(value, 1.0F / GAMMA) * 255.0F));
        }

        static void decode(const Size2U& size,
                           const std::vector<uint8_t>& encodedData,
                           PixelFormat pixelFormat,
                           std::vector<float>& decodedData)
        {
            uint32_t channelCount = getChannelCount(pixelFormat);
            uint32_t pitch = size.width() * channelCount;
            decodedData.resize(size.width() * size.height() * channelCount);
            const uint8_t* src = encodedData.data();
            float* dst = decodedData.data();

            switch (pixelFormat)
            {
                case PixelFormat::RGBA8_UNORM:
                case PixelFormat::RGBA8_UNORM_SRGB:
                    for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                    {
                        const uint8_t* pixel = src;
                        for (uint32_t x = 0; x < size.width(); ++x, pixel += 4, dst += 4)
                        {
                            dst[0] = gammaDecode(pixel[0]); // red
                            dst[1] = gammaDecode(pixel[1]); // green
                            dst[2] = gammaDecode(pixel[2]); // blue
                            dst[3] = pixel[3] / 255.0F; // alpha
                        }
                    }
                    break;

                case PixelFormat::RG8_UNORM:
                    for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                    {
                        const uint8_t* pixel = src;
                        for (uint32_t x = 0; x < size.width(); ++x, pixel += 2, dst += 2)
                        {
                            dst[0] = gammaDecode(pixel[0]); // red
                            dst[1] = gammaDecode(pixel[1]); // green
                        }
                    }
                    break;

                case PixelFormat::R8_UNORM:
                    for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                    {
                        const uint8_t* pixel = src;
                        for (uint32_t x = 0; x < size.width(); ++x, pixel += 4, dst += 1)
                        {
                            dst[0] = gammaDecode(pixel[0]); // red
                        }
                    }
                    break;

                case PixelFormat::A8_UNORM:
                    for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                    {
                        const uint8_t* pixel = src;
                        for (uint32_t x = 0; x < size.width(); ++x, pixel += 1, dst += 1)
                        {
                            dst[0] = pixel[0] / 255.0F; // alpha
                        }
                    }
                    break;

                default:
                    throw std::runtime_error("Invalid pixel format");
            }
        }

        static void encode(const Size2U& size,
                           const std::vector<float>& decodedData,
                           PixelFormat pixelFormat,
                           std::vector<uint8_t>& encodedData)
        {
            uint32_t pixelSize = getPixelSize(pixelFormat);
            uint32_t pitch = size.width() * pixelSize;
            encodedData.resize(size.width() * size.height() * pixelSize);
            const float* src = decodedData.data();
            uint8_t* dst = encodedData.data();

            switch (pixelFormat)
            {
                case PixelFormat::RGBA8_UNORM:
                case PixelFormat::RGBA8_UNORM_SRGB:
                    for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                    {
                        const float* pixel = src;
                        for (uint32_t x = 0; x < size.width(); ++x, pixel += 4, dst += 4)
                        {
                            dst[0] = gammaEncode(pixel[0]); //red
                            dst[1] = gammaEncode(pixel[1]); // green
                            dst[2] = gammaEncode(pixel[2]); // blue
                            dst[3] = static_cast<uint8_t>(round(pixel[3] * 255.0F)); // alpha
                        }
                    }
                    break;

                case PixelFormat::RG8_UNORM:
                    for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                    {
                        const float* pixel = src;
                        for (uint32_t x = 0; x < size.width(); ++x, pixel += 2, dst += 2)
                        {
                            dst[0] = gammaEncode(pixel[0]); //red
                            dst[1] = gammaEncode(pixel[1]); // green
                        }
                    }
                    break;

                case PixelFormat::R8_UNORM:
                    for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                    {
                        const float* pixel = src;
                        for (uint32_t x = 0; x < size.width(); ++x, pixel += 1, dst += 1)
                        {
                            dst[0] = gammaEncode(pixel[0]); //red
                        }
                    }
                    break;

                case PixelFormat::A8_UNORM:
                    for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                    {
                        const float* pixel = src;
                        for (uint32_t x = 0; x < size.width(); ++x, pixel += 1, dst += 1)
                        {
                            dst[0] = static_cast<uint8_t>(round(pixel[0] * 255.0F)); // alpha
                        }
                    }
                    break;

                default:
                    throw std::runtime_error("Invalid pixel format");
            }
        }

        static std::vector<Texture::Level> calculateSizes(const Size2U& size,
                                                          const std::vector<uint8_t>& data,
                                                          uint32_t mipmaps,
                                                          PixelFormat pixelFormat)
        {
            std::vector<Texture::Level> levels;

            uint32_t newWidth = size.v[0];
            uint32_t newHeight = size.v[1];

            uint32_t pixelSize = getPixelSize(pixelFormat);
            uint32_t pitch = newWidth * pixelSize;
            uint32_t bufferSize = pitch * newHeight;
            if (data.empty())
                levels.push_back({size, pitch, std::vector<uint8_t>(bufferSize)});
            else
                levels.push_back({size, pitch, data});

            uint32_t previousWidth = newWidth;
            uint32_t previousHeight = newHeight;
            uint32_t previousPitch = pitch;
            std::vector<float> previousData;

            decode(size, data, pixelFormat, previousData);

            std::vector<float> newData;
            std::vector<uint8_t> encodedData;

            while ((newWidth > 1 || newHeight > 1) &&
                   (mipmaps == 0 || levels.size() < mipmaps))
            {
                newWidth >>= 1;
                newHeight >>= 1;

                if (newWidth < 1) newWidth = 1;
                if (newHeight < 1) newHeight = 1;

                Size2U mipMapSize = Size2U(newWidth, newHeight);
                pitch = newWidth * pixelSize;
                bufferSize = pitch * newHeight;

                if (data.empty())
                    levels.push_back({mipMapSize, pitch, std::vector<uint8_t>(bufferSize)});
                else
                {

                    switch (pixelFormat)
                    {
                        case PixelFormat::RGBA8_UNORM:
                        case PixelFormat::RGBA8_UNORM_SRGB:
                            downsample2x2Rgba8(previousWidth, previousHeight, previousData, newData);
                            break;

                        case PixelFormat::RG8_UNORM:
                            downsample2x2Rg8(previousWidth, previousHeight, previousData, newData);
                            break;

                        case PixelFormat::R8_UNORM:
                            downsample2x2R8(previousWidth, previousHeight, previousData, newData);
                            break;

                        case PixelFormat::A8_UNORM:
                            downsample2x2A8(previousWidth, previousHeight, previousData, newData);
                            break;

                        default:
                            throw std::runtime_error("Invalid pixel format");
                    }

                    encode(mipMapSize, newData, pixelFormat, encodedData);
                    levels.push_back({mipMapSize, pitch, encodedData});

                    previousData = newData;
                }

                previousWidth = newWidth;
                previousHeight = newHeight;
                previousPitch = pitch;
            }

            return levels;
        }

        Texture::Texture(Renderer& initRenderer):
            resource(initRenderer)
        {
        }

        Texture::Texture(Renderer& initRenderer,
                         const Size2U& initSize,
                         uint32_t initFlags,
                         uint32_t initMipmaps,
                         uint32_t initSampleCount,
                         PixelFormat initPixelFormat):
            resource(initRenderer),
            size(initSize),
            flags(initFlags),
            mipmaps(initMipmaps),
            sampleCount(initSampleCount),
            pixelFormat(initPixelFormat)
        {
            if ((flags & BIND_RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            if (!initRenderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPowerOfTwo(size.v[0]) || !isPowerOfTwo(size.v[1])))
                mipmaps = 1;

            std::vector<Level> levels = calculateSizes(size, std::vector<uint8_t>(), mipmaps, pixelFormat);

            initRenderer.addCommand(std::unique_ptr<Command>(new InitTextureCommand(resource.getId(),
                                                                                    levels,
                                                                                    Dimensions::TWO,
                                                                                    flags,
                                                                                    sampleCount,
                                                                                    pixelFormat)));
        }

        Texture::Texture(Renderer& initRenderer,
                         const std::vector<uint8_t>& initData,
                         const Size2U& initSize,
                         uint32_t initFlags,
                         uint32_t initMipmaps,
                         PixelFormat initPixelFormat):
            resource(initRenderer),
            size(initSize),
            flags(initFlags),
            mipmaps(initMipmaps),
            sampleCount(1),
            pixelFormat(initPixelFormat)
        {
            if ((flags & BIND_RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            if (!initRenderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPowerOfTwo(size.v[0]) || !isPowerOfTwo(size.v[1])))
                mipmaps = 1;

            std::vector<Level> levels = calculateSizes(size, initData, mipmaps, pixelFormat);

            initRenderer.addCommand(std::unique_ptr<Command>(new InitTextureCommand(resource.getId(),
                                                                                    levels,
                                                                                    Dimensions::TWO,
                                                                                    flags,
                                                                                    sampleCount,
                                                                                    pixelFormat)));
        }

        Texture::Texture(Renderer& initRenderer,
                         const std::vector<Level>& initLevels,
                         const Size2U& initSize,
                         uint32_t initFlags,
                         PixelFormat initPixelFormat):
            resource(initRenderer),
            size(initSize),
            flags(initFlags),
            mipmaps(static_cast<uint32_t>(initLevels.size())),
            sampleCount(1),
            pixelFormat(initPixelFormat)
        {
            if ((flags & BIND_RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            std::vector<Level> levels = initLevels;

            if (!initRenderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPowerOfTwo(size.v[0]) || !isPowerOfTwo(size.v[1])))
            {
                mipmaps = 1;
                levels.resize(1);
            }

            initRenderer.addCommand(std::unique_ptr<Command>(new InitTextureCommand(resource.getId(),
                                                                                    levels,
                                                                                    Dimensions::TWO,
                                                                                    flags,
                                                                                    sampleCount,
                                                                                    pixelFormat)));
        }

        void Texture::setData(const std::vector<uint8_t>& newData)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::BIND_RENDER_TARGET)
                throw std::runtime_error("Texture is not dynamic");

            std::vector<Level> levels = calculateSizes(size, newData, mipmaps, pixelFormat);

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureDataCommand(resource.getId(),
                                                                                                      levels)));
        }

        void Texture::setData(const std::vector<uint8_t>& newData, CubeFace face)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::BIND_RENDER_TARGET)
                throw std::runtime_error("Texture is not dynamic");

            std::vector<Level> levels = calculateSizes(size, newData, mipmaps, pixelFormat);

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureDataCommand(resource.getId(),
                                                                                                      levels,
                                                                                                      face)));
        }

        void Texture::setFilter(Filter newFilter)
        {
            filter = newFilter;

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource.getId(),
                                                                                                            filter,
                                                                                                            addressX,
                                                                                                            addressY,
                                                                                                            addressZ,
                                                                                                            maxAnisotropy)));
        }

        void Texture::setAddressX(Address newAddressX)
        {
            addressX = newAddressX;

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource.getId(),
                                                                                                            filter,
                                                                                                            addressX,
                                                                                                            addressY,
                                                                                                            addressZ,
                                                                                                            maxAnisotropy)));
        }

        void Texture::setAddressY(Address newAddressY)
        {
            addressY = newAddressY;

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource.getId(),
                                                                                                            filter,
                                                                                                            addressX,
                                                                                                            addressY,
                                                                                                            addressZ,
                                                                                                            maxAnisotropy)));
        }

        void Texture::setAddressZ(Address newAddressZ)
        {
            addressZ = newAddressZ;

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource.getId(),
                                                                                                            filter,
                                                                                                            addressX,
                                                                                                            addressY,
                                                                                                            addressZ,
                                                                                                            maxAnisotropy)));
        }

        void Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource.getId(),
                                                                                                            filter,
                                                                                                            addressX,
                                                                                                            addressY,
                                                                                                            addressZ,
                                                                                                            maxAnisotropy)));
        }
    } // namespace graphics
} // namespace ouzel
