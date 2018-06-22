// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CursorResourceMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        CursorResourceMacOS::CursorResourceMacOS(InputManager& initInputManager):
            CursorResource(initInputManager)
        {
        }

        CursorResourceMacOS::~CursorResourceMacOS()
        {
            if (cursor) [cursor release];
        }

        void CursorResourceMacOS::init(SystemCursor newSystemCursor)
        {
            CursorResource::init(newSystemCursor);

            if (cursor)
            {
                [cursor release];
                cursor = nil;
            }

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

            reactivate();
        }

        void CursorResourceMacOS::init(const std::vector<uint8_t>& newData,
                                       const Size2& newSize,
                                       graphics::PixelFormat newPixelFormat,
                                       const Vector2& newHotSpot)
        {
            CursorResource::init(newData,
                                 newSize,
                                 newPixelFormat,
                                 newHotSpot);

            if (cursor)
            {
                [cursor release];
                cursor = nil;
            }

            if (!data.empty())
            {
                NSInteger bytesPerPixel = graphics::getPixelSize(pixelFormat);
                NSInteger channelSize = graphics::getChannelSize(pixelFormat);
                NSInteger channelCount = graphics::getChannelCount(pixelFormat);
                NSInteger width = static_cast<NSInteger>(size.width);
                NSInteger height = static_cast<NSInteger>(size.height);

                unsigned char* rgba = data.data();

                NSImage* image = [[NSImage alloc] initWithSize:NSMakeSize(width, height)];
                NSBitmapImageRep* rep = [[NSBitmapImageRep alloc]
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
                                                 hotSpot:NSMakePoint(hotSpot.x, size.height - hotSpot.y - 1.0F)];

                [image release];
                [rep release];
            }

            reactivate();
        }
    } // namespace input
} // namespace ouzel
