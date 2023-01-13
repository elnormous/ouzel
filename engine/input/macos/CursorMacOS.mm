// Ouzel by Elviss Strazdins

#include "CursorMacOS.hpp"

namespace ouzel::input::macos
{
    Cursor::Cursor(SystemCursor systemCursor)
    {
        switch (systemCursor)
        {
            case SystemCursor::arrow:
                cursor = [[NSCursor arrowCursor] retain];
                break;
            case SystemCursor::hand:
                cursor = [[NSCursor openHandCursor] retain];
                break;
            case SystemCursor::horizontalResize:
                cursor = [[NSCursor resizeLeftRightCursor] retain];
                break;
            case SystemCursor::verticalResize:
                cursor = [[NSCursor resizeUpDownCursor] retain];
                break;
            case SystemCursor::cross:
                cursor = [[NSCursor crosshairCursor] retain];
                break;
            case SystemCursor::iBeam:
                cursor = [[NSCursor IBeamCursor] retain];
                break;
        }
    }

    Cursor::Cursor(const std::vector<std::uint8_t>& newData,
                   const math::Size<std::uint32_t, 2>& size,
                   graphics::PixelFormat pixelFormat,
                   const math::Vector<float, 2>& hotSpot):
        data(newData)
    {
        if (!data.empty())
        {
            const NSInteger bytesPerPixel = graphics::getPixelSize(pixelFormat);
            const NSInteger channelSize = graphics::getChannelSize(pixelFormat);
            const NSInteger channelCount = graphics::getChannelCount(pixelFormat);
            const auto width = static_cast<NSInteger>(size.v[0]);
            const auto height = static_cast<NSInteger>(size.v[1]);

            unsigned char* rgba = data.data();

            NSImage* image = [[[NSImage alloc]
                                initWithSize:NSMakeSize(width, height)] autorelease];
            NSBitmapImageRep* imageRep = [[[NSBitmapImageRep alloc]
                                            initWithBitmapDataPlanes:&rgba
                                            pixelsWide:width
                                            pixelsHigh:height
                                            bitsPerSample:channelSize * 8
                                            samplesPerPixel:channelCount
                                            hasAlpha:YES
                                            isPlanar:NO
                                            colorSpaceName:NSDeviceRGBColorSpace
                                            bitmapFormat:NSAlphaNonpremultipliedBitmapFormat
                                            bytesPerRow:width * bytesPerPixel
                                            bitsPerPixel:bytesPerPixel * 8] autorelease];

            [image addRepresentation:imageRep];
            cursor = [[NSCursor alloc] initWithImage:image
                                             hotSpot:NSMakePoint(static_cast<double>(hotSpot.v[0]),
                                                                 static_cast<double>(size.v[1]) - static_cast<double>(hotSpot.v[1]) - 1.0)];
        }
    }

    Cursor::~Cursor()
    {
        if (cursor) [cursor release];
    }
}
