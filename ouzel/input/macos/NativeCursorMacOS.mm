// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "NativeCursorMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        NativeCursorMacOS::NativeCursorMacOS(SystemCursor systemCursor)
        {
            switch (systemCursor)
            {
                case SystemCursor::DEFAULT:
                case SystemCursor::ARROW:
                    cursor = [NSCursor arrowCursor];
                    break;
                case SystemCursor::HAND:
                    cursor = [NSCursor openHandCursor];
                    break;
                case SystemCursor::HORIZONTAL_RESIZE:
                    cursor = [NSCursor resizeLeftRightCursor];
                    break;
                case SystemCursor::VERTICAL_RESIZE:
                    cursor = [NSCursor resizeUpDownCursor];
                    break;
                case SystemCursor::CROSS:
                    cursor = [NSCursor crosshairCursor];
                    break;
                case SystemCursor::I_BEAM:
                    cursor = [NSCursor IBeamCursor];
                    break;
            }

            if (cursor) [cursor retain];
        }

        NativeCursorMacOS::NativeCursorMacOS(const std::vector<uint8_t>& data,
                                             const Size2& size,
                                             graphics::PixelFormat pixelFormat,
                                             const Vector2& hotSpot)
        {
            if (!data.empty())
            {
                NSInteger bytesPerPixel = graphics::getPixelSize(pixelFormat);
                NSInteger channelSize = graphics::getChannelSize(pixelFormat);
                NSInteger channelCount = graphics::getChannelCount(pixelFormat);
                NSInteger width = static_cast<NSInteger>(size.width);
                NSInteger height = static_cast<NSInteger>(size.height);

                std::vector<uint8_t> copy = data;
                unsigned char* rgba = copy.data();

                NSImage* image = [[[NSImage alloc] initWithSize:NSMakeSize(width, height)] autorelease];
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
                                                 hotSpot:NSMakePoint(hotSpot.x, size.height - hotSpot.y - 1.0F)];
            }
        }

        NativeCursorMacOS::~NativeCursorMacOS()
        {
            if (cursor) [cursor release];
        }
    } // namespace input
} // namespace ouzel
