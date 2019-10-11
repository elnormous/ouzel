// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "Texture.hpp"
#include "Renderer.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace
        {
            constexpr float GAMMA = 2.2F;
            const float GAMMA_DECODE[256] = {
                0.0F, 5.077051355e-06F, 2.33280025e-05F, 5.692175546e-05F, 0.0001071873558F, 0.0001751239615F, 0.0002615437261F, 0.0003671362065F,
                0.0004925037501F, 0.0006381827989F, 0.0008046584553F, 0.0009923742618F, 0.001201739418F, 0.001433134428F, 0.001686915057F, 0.001963415882F,
                0.002262953203F, 0.00258582551F, 0.002932318253F, 0.003302702913F, 0.003697239328F, 0.004116177093F, 0.00455975486F, 0.00502820313F,
                0.00552174449F, 0.006040593144F, 0.006584956776F, 0.007155036554F, 0.007751026656F, 0.008373117074F, 0.009021490812F, 0.009696328081F,
                0.01039780304F, 0.01112608239F, 0.01188133471F, 0.01266372018F, 0.01347339712F, 0.01431051921F, 0.01517523825F, 0.01606770046F,
                0.01698805206F, 0.01793643273F, 0.0189129822F, 0.01991783828F, 0.02095113136F, 0.02201299369F, 0.02310355566F, 0.02422294207F,
                0.02537127584F, 0.02654868178F, 0.02775527909F, 0.02899118513F, 0.03025651723F, 0.03155139089F, 0.03287591413F, 0.03423020616F,
                0.03561436757F, 0.03702851385F, 0.03847274557F, 0.03994716704F, 0.04145189002F, 0.04298700765F, 0.04455262423F, 0.04614884034F,
                0.04777575657F, 0.04943346232F, 0.05112205446F, 0.05284162983F, 0.05459228158F, 0.05637409911F, 0.05818717927F, 0.06003161147F,
                0.06190747768F, 0.06381487101F, 0.06575388461F, 0.06772459298F, 0.06972708553F, 0.07176145166F, 0.07382776588F, 0.07592612505F,
                0.07805658877F, 0.08021926135F, 0.08241420984F, 0.08464150876F, 0.08690125495F, 0.08919350803F, 0.0915183574F, 0.09387587011F,
                0.09626612067F, 0.09868919849F, 0.1011451632F, 0.1036340967F, 0.1061560661F, 0.1087111533F, 0.1112994179F, 0.1139209345F,
                0.1165757775F, 0.1192640141F, 0.1219857112F, 0.1247409433F, 0.1275297701F, 0.1303522736F, 0.1332085133F, 0.1360985488F,
                0.1390224546F, 0.1419802904F, 0.1449721307F, 0.1479980201F, 0.151058048F, 0.1541522592F, 0.1572807282F, 0.1604435146F,
                0.163640663F, 0.166872263F, 0.170138374F, 0.1734390259F, 0.176774323F, 0.1801442802F, 0.1835489869F, 0.1869885027F,
                0.1904628724F, 0.1939721555F, 0.1975164264F, 0.2010957301F, 0.204710111F, 0.2083596438F, 0.2120443881F, 0.2157643884F,
                0.2195197344F, 0.2233104259F, 0.2271365523F, 0.2309981436F, 0.234895274F, 0.2388280034F, 0.2427963763F, 0.2468004376F,
                0.2508402467F, 0.2549158633F, 0.2590273619F, 0.2631747425F, 0.2673580945F, 0.2715774477F, 0.2758328617F, 0.2801243961F,
                0.2844520807F, 0.288816005F, 0.2932161689F, 0.2976526618F, 0.3021255136F, 0.3066347837F, 0.311180532F, 0.3157627583F,
                0.3203815818F, 0.3250369728F, 0.3297290504F, 0.3344578147F, 0.3392233551F, 0.3440256715F, 0.3488648534F, 0.3537409306F,
                0.3586539328F, 0.3636039197F, 0.368590951F, 0.3736150563F, 0.3786762655F, 0.383774668F, 0.3889102638F, 0.3940831423F,
                0.3992933333F, 0.4045408368F, 0.409825772F, 0.4151481092F, 0.4205079377F, 0.4259053171F, 0.4313402176F, 0.4368127584F,
                0.4423229694F, 0.4478708506F, 0.4534564912F, 0.4590799212F, 0.4647411406F, 0.4704402685F, 0.4761772752F, 0.48195225F,
                0.4877652228F, 0.4936162233F, 0.4995052814F, 0.5054324865F, 0.5113978386F, 0.5174013972F, 0.5234431624F, 0.5295232534F,
                0.5356416106F, 0.5417983532F, 0.5479935408F, 0.5542271137F, 0.5604991913F, 0.5668097734F, 0.5731588602F, 0.5795466304F,
                0.5859730244F, 0.5924380422F, 0.598941803F, 0.6054843068F, 0.6120656133F, 0.6186857224F, 0.6253447533F, 0.6320426464F,
                0.6387794614F, 0.6455552578F, 0.6523700953F, 0.6592240334F, 0.6661169529F, 0.6730490923F, 0.6800203323F, 0.6870308518F,
                0.6940805316F, 0.7011694908F, 0.7082977891F, 0.7154654264F, 0.7226724625F, 0.7299188972F, 0.7372047901F, 0.744530201F,
                0.7518950701F, 0.7592995763F, 0.7667436004F, 0.7742273211F, 0.781750679F, 0.7893137336F, 0.7969165444F, 0.8045591116F,
                0.8122414947F, 0.8199636936F, 0.8277258277F, 0.8355277777F, 0.8433697224F, 0.8512516618F, 0.8591735959F, 0.8671355247F,
                0.8751375675F, 0.8831797242F, 0.8912620544F, 0.8993844986F, 0.9075471759F, 0.9157501459F, 0.9239933491F, 0.932276845F,
                0.9406006932F, 0.9489649534F, 0.957369566F, 0.9658146501F, 0.9743002057F, 0.9828262329F, 0.9913928509F, 1.0F
            };

            void downsample2x2A8(uint32_t width, uint32_t height,
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

            void downsample2x2R8(uint32_t width, uint32_t height,
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

            void downsample2x2Rg8(uint32_t width, uint32_t height,
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

            void downsample2x2Rgba8(uint32_t width, uint32_t height,
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

            inline float gammaDecode(uint8_t value) noexcept
            {
                return GAMMA_DECODE[value]; // std::pow(value / 255.0F, GAMMA);
            }

            inline uint8_t gammaEncode(float value) noexcept
            {
                return static_cast<uint8_t>(std::round(std::pow(value, 1.0F / GAMMA) * 255.0F));
            }

            void decode(const Size2U& size,
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
                    case PixelFormat::RGBA8UNorm:
                    case PixelFormat::RGBA8UNormSRGB:
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

                    case PixelFormat::RG8UNorm:
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

                    case PixelFormat::R8UNorm:
                        for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                        {
                            const uint8_t* pixel = src;
                            for (uint32_t x = 0; x < size.width(); ++x, pixel += 4, dst += 1)
                            {
                                dst[0] = gammaDecode(pixel[0]); // red
                            }
                        }
                        break;

                    case PixelFormat::A8UNorm:
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

            void encode(const Size2U& size,
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
                    case PixelFormat::RGBA8UNorm:
                    case PixelFormat::RGBA8UNormSRGB:
                        for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                        {
                            const float* pixel = src;
                            for (uint32_t x = 0; x < size.width(); ++x, pixel += 4, dst += 4)
                            {
                                dst[0] = gammaEncode(pixel[0]); // red
                                dst[1] = gammaEncode(pixel[1]); // green
                                dst[2] = gammaEncode(pixel[2]); // blue
                                dst[3] = static_cast<uint8_t>(std::round(pixel[3] * 255.0F)); // alpha
                            }
                        }
                        break;

                    case PixelFormat::RG8UNorm:
                        for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                        {
                            const float* pixel = src;
                            for (uint32_t x = 0; x < size.width(); ++x, pixel += 2, dst += 2)
                            {
                                dst[0] = gammaEncode(pixel[0]); // red
                                dst[1] = gammaEncode(pixel[1]); // green
                            }
                        }
                        break;

                    case PixelFormat::R8UNorm:
                        for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                        {
                            const float* pixel = src;
                            for (uint32_t x = 0; x < size.width(); ++x, pixel += 1, dst += 1)
                            {
                                dst[0] = gammaEncode(pixel[0]); // red
                            }
                        }
                        break;

                    case PixelFormat::A8UNorm:
                        for (uint32_t y = 0; y < size.height(); ++y, src += pitch)
                        {
                            const float* pixel = src;
                            for (uint32_t x = 0; x < size.width(); ++x, pixel += 1, dst += 1)
                            {
                                dst[0] = static_cast<uint8_t>(std::round(pixel[0] * 255.0F)); // alpha
                            }
                        }
                        break;

                    default:
                        throw std::runtime_error("Invalid pixel format");
                }
            }

            std::vector<std::pair<Size2U, std::vector<uint8_t>>> calculateSizes(const Size2U& size,
                                                                                uint32_t mipmaps,
                                                                                PixelFormat pixelFormat)
            {
                std::vector<std::pair<Size2U, std::vector<uint8_t>>> levels;

                uint32_t newWidth = size.v[0];
                uint32_t newHeight = size.v[1];

                uint32_t pixelSize = getPixelSize(pixelFormat);
                uint32_t bufferSize = newWidth * newHeight * pixelSize;
                levels.emplace_back(size, std::vector<uint8_t>(bufferSize));

                while ((newWidth > 1 || newHeight > 1) &&
                    (mipmaps == 0 || levels.size() < mipmaps))
                {
                    newWidth >>= 1;
                    newHeight >>= 1;

                    if (newWidth < 1) newWidth = 1;
                    if (newHeight < 1) newHeight = 1;

                    auto mipMapSize = Size2U(newWidth, newHeight);
                    bufferSize = newWidth * newHeight * pixelSize;

                    levels.emplace_back(mipMapSize, std::vector<uint8_t>(bufferSize));
                }

                return levels;
            }

            std::vector<std::pair<Size2U, std::vector<uint8_t>>> calculateSizes(const Size2U& size,
                                                                                const std::vector<uint8_t>& data,
                                                                                uint32_t mipmaps,
                                                                                PixelFormat pixelFormat)
            {
                std::vector<std::pair<Size2U, std::vector<uint8_t>>> levels;

                uint32_t newWidth = size.v[0];
                uint32_t newHeight = size.v[1];

                uint32_t pixelSize = getPixelSize(pixelFormat);
                uint32_t bufferSize = newWidth * newHeight * pixelSize;
                levels.emplace_back(size, data);

                uint32_t previousWidth = newWidth;
                uint32_t previousHeight = newHeight;
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

                    auto mipMapSize = Size2U(newWidth, newHeight);
                    bufferSize = newWidth * newHeight * pixelSize;

                    switch (pixelFormat)
                    {
                        case PixelFormat::RGBA8UNorm:
                        case PixelFormat::RGBA8UNormSRGB:
                            downsample2x2Rgba8(previousWidth, previousHeight, previousData, newData);
                            break;

                        case PixelFormat::RG8UNorm:
                            downsample2x2Rg8(previousWidth, previousHeight, previousData, newData);
                            break;

                        case PixelFormat::R8UNorm:
                            downsample2x2R8(previousWidth, previousHeight, previousData, newData);
                            break;

                        case PixelFormat::A8UNorm:
                            downsample2x2A8(previousWidth, previousHeight, previousData, newData);
                            break;

                        default:
                            throw std::runtime_error("Invalid pixel format");
                    }

                    encode(mipMapSize, newData, pixelFormat, encodedData);
                    levels.emplace_back(mipMapSize, encodedData);

                    previousData = newData;

                    previousWidth = newWidth;
                    previousHeight = newHeight;
                }

                return levels;
            }
        }

        Texture::Texture(Renderer& initRenderer):
            renderer(&initRenderer),
            resource(initRenderer.getDevice()->createResource())
        {
        }

        Texture::Texture(Renderer& initRenderer,
                         const Size2U& initSize,
                         uint32_t initFlags,
                         uint32_t initMipmaps,
                         uint32_t initSampleCount,
                         PixelFormat initPixelFormat):
            renderer(&initRenderer),
            resource(initRenderer.getDevice()->createResource()),
            size(initSize),
            flags(initFlags),
            mipmaps(initMipmaps),
            sampleCount(initSampleCount),
            pixelFormat(initPixelFormat)
        {
            if ((flags & Flags::BindRenderTarget) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            if (!initRenderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPowerOfTwo(size.v[0]) || !isPowerOfTwo(size.v[1])))
                mipmaps = 1;

            std::vector<std::pair<Size2U, std::vector<uint8_t>>> levels = calculateSizes(size, mipmaps, pixelFormat);

            initRenderer.addCommand(std::make_unique<InitTextureCommand>(resource,
                                                                        levels,
                                                                        TextureType::TwoDimensional,
                                                                        flags,
                                                                        sampleCount,
                                                                        pixelFormat));
        }

        Texture::Texture(Renderer& initRenderer,
                         const std::vector<uint8_t>& initData,
                         const Size2U& initSize,
                         uint32_t initFlags,
                         uint32_t initMipmaps,
                         PixelFormat initPixelFormat):
            renderer(&initRenderer),
            resource(initRenderer.getDevice()->createResource()),
            size(initSize),
            flags(initFlags),
            mipmaps(initMipmaps),
            sampleCount(1),
            pixelFormat(initPixelFormat)
        {
            if ((flags & Flags::BindRenderTarget) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            if (!initRenderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPowerOfTwo(size.v[0]) || !isPowerOfTwo(size.v[1])))
                mipmaps = 1;

            std::vector<std::pair<Size2U, std::vector<uint8_t>>> levels = calculateSizes(size, initData, mipmaps, pixelFormat);

            initRenderer.addCommand(std::make_unique<InitTextureCommand>(resource,
                                                                         levels,
                                                                         TextureType::TwoDimensional,
                                                                         flags,
                                                                         sampleCount,
                                                                         pixelFormat));
        }

        Texture::Texture(Renderer& initRenderer,
                         const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& initLevels,
                         const Size2U& initSize,
                         uint32_t initFlags,
                         PixelFormat initPixelFormat):
            renderer(&initRenderer),
            resource(initRenderer.getDevice()->createResource()),
            size(initSize),
            flags(initFlags),
            mipmaps(static_cast<uint32_t>(initLevels.size())),
            sampleCount(1),
            pixelFormat(initPixelFormat)
        {
            if ((flags & Flags::BindRenderTarget) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            std::vector<std::pair<Size2U, std::vector<uint8_t>>> levels = initLevels;

            if (!initRenderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPowerOfTwo(size.v[0]) || !isPowerOfTwo(size.v[1])))
            {
                mipmaps = 1;
                levels.resize(1);
            }

            initRenderer.addCommand(std::make_unique<InitTextureCommand>(resource,
                                                                         levels,
                                                                         TextureType::TwoDimensional,
                                                                         flags,
                                                                         sampleCount,
                                                                         pixelFormat));
        }

        void Texture::setData(const std::vector<uint8_t>& newData, CubeFace face)
        {
            if (!(flags & Flags::Dynamic) || flags & Flags::BindRenderTarget)
                throw std::runtime_error("Texture is not dynamic");

            std::vector<std::pair<Size2U, std::vector<uint8_t>>> levels = calculateSizes(size, newData, mipmaps, pixelFormat);

            if (resource)
                renderer->addCommand(std::make_unique<SetTextureDataCommand>(resource,
                                                                             levels,
                                                                             face));
        }

        void Texture::setFilter(SamplerFilter newFilter)
        {
            filter = newFilter;

            if (resource)
                renderer->addCommand(std::make_unique<SetTextureParametersCommand>(resource,
                                                                                   filter,
                                                                                   addressX,
                                                                                   addressY,
                                                                                   addressZ,
                                                                                   borderColor,
                                                                                   maxAnisotropy));
        }

        void Texture::setAddressX(SamplerAddressMode newAddressX)
        {
            addressX = newAddressX;

            if (resource)
                renderer->addCommand(std::make_unique<SetTextureParametersCommand>(resource,
                                                                                   filter,
                                                                                   addressX,
                                                                                   addressY,
                                                                                   addressZ,
                                                                                   borderColor,
                                                                                   maxAnisotropy));
        }

        void Texture::setAddressY(SamplerAddressMode newAddressY)
        {
            addressY = newAddressY;

            if (resource)
                renderer->addCommand(std::make_unique<SetTextureParametersCommand>(resource,
                                                                                   filter,
                                                                                   addressX,
                                                                                   addressY,
                                                                                   addressZ,
                                                                                   borderColor,
                                                                                   maxAnisotropy));
        }

        void Texture::setAddressZ(SamplerAddressMode newAddressZ)
        {
            addressZ = newAddressZ;

            if (resource)
                renderer->addCommand(std::make_unique<SetTextureParametersCommand>(resource,
                                                                                   filter,
                                                                                   addressX,
                                                                                   addressY,
                                                                                   addressZ,
                                                                                   borderColor,
                                                                                   maxAnisotropy));
        }

        void Texture::setBorderColor(Color newBorderColor)
        {
            borderColor = newBorderColor;

            if (resource)
                renderer->addCommand(std::make_unique<SetTextureParametersCommand>(resource,
                                                                                   filter,
                                                                                   addressX,
                                                                                   addressY,
                                                                                   addressZ,
                                                                                   borderColor,
                                                                                   maxAnisotropy));
        }

        void Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;

            if (resource)
                renderer->addCommand(std::make_unique<SetTextureParametersCommand>(resource,
                                                                                   filter,
                                                                                   addressX,
                                                                                   addressY,
                                                                                   addressZ,
                                                                                   borderColor,
                                                                                   maxAnisotropy));
        }
    } // namespace graphics
} // namespace ouzel
