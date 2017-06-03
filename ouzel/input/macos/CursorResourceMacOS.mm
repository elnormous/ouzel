// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CursorResourceMacOS.h"

namespace ouzel
{
    namespace input
    {
        CursorResourceMacOS::~CursorResourceMacOS()
        {
            if (cursor) [cursor release];
        }

        bool CursorResourceMacOS::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                if (cursor)
                {
                    [cursor release];
                    cursor = Nil;
                }

                if (data.empty())
                {
                    switch (systemCursor)
                    {
                        case SystemCursor::DEFAULT:
                            cursor = [NSCursor arrowCursor];
                            break;
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
                else
                {
                    NSInteger bytesPerPixel = graphics::getPixelSize(pixelFormat);
                    NSInteger channelSize = graphics::getChannelSize(pixelFormat);
                    NSInteger channelCount = graphics::getChannelCount(pixelFormat);
                    NSInteger width = static_cast<NSInteger>(size.v[0]);
                    NSInteger height = static_cast<NSInteger>(size.v[1]);

                    unsigned char* rgba = reinterpret_cast<unsigned char*>(data.data());

                    NSImage *image = [[NSImage alloc] initWithSize:NSMakeSize(width, height)];
                    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc]
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
                                             bitsPerPixel:bytesPerPixel * 8];

                    [image addRepresentation:rep];
                    cursor = [[NSCursor alloc] initWithImage:image
                                                     hotSpot:NSMakePoint(hotSpot.v[0], size.v[1] - hotSpot.v[1] - 1.0f)];

                    [image release];
                    [rep release];
                }

                dirty = false;
            }

            return true;
        }
    } // namespace input
} // namespace ouzel
