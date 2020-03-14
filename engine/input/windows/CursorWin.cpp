// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <system_error>
#include "CursorWin.hpp"

namespace ouzel
{
    namespace input
    {
        CursorWin::CursorWin(SystemCursor systemCursor)
        {
            switch (systemCursor)
            {
                case SystemCursor::Default:
                case SystemCursor::Arrow:
                    cursor = LoadCursor(nullptr, IDC_ARROW);
                    break;
                case SystemCursor::Hand:
                    cursor = LoadCursor(nullptr, IDC_HAND);
                    break;
                case SystemCursor::HorizontalResize:
                    cursor = LoadCursor(nullptr, IDC_SIZEWE);
                    break;
                case SystemCursor::VerticalResize:
                    cursor = LoadCursor(nullptr, IDC_SIZENS);
                    break;
                case SystemCursor::Cross:
                    cursor = LoadCursor(nullptr, IDC_CROSS);
                    break;
                case SystemCursor::IBeam:
                    cursor = LoadCursor(nullptr, IDC_IBEAM);
                    break;
                default:
                    throw std::runtime_error("Invalid cursor");
            }

            if (!cursor)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to load cursor");
        }

        CursorWin::CursorWin(const std::vector<std::uint8_t>& data,
                             const Size2F& size,
                             graphics::PixelFormat pixelFormat,
                             const Vector2F& hotSpot)
        {
            if (!data.empty())
            {
                auto width = static_cast<LONG>(size.v[0]);
                auto height = static_cast<LONG>(size.v[1]);

                BITMAPV5HEADER bitmapHeader = {};
                bitmapHeader.bV5Size = sizeof(BITMAPV5HEADER);
                bitmapHeader.bV5Width = width;
                bitmapHeader.bV5Height = -height;
                bitmapHeader.bV5Planes = 1;
                bitmapHeader.bV5BitCount = 32;
                bitmapHeader.bV5Compression = BI_BITFIELDS;
                bitmapHeader.bV5RedMask = 0x00FF0000;
                bitmapHeader.bV5GreenMask = 0x0000FF00;
                bitmapHeader.bV5BlueMask = 0x000000FF;
                bitmapHeader.bV5AlphaMask = 0xFF000000;

                dc = GetDC(nullptr);
                void* targetPointer = nullptr;
                color = CreateDIBSection(dc,
                                         reinterpret_cast<BITMAPINFO*>(&bitmapHeader),
                                         DIB_RGB_COLORS,
                                         &targetPointer,
                                         nullptr,
                                         DWORD{0});

                if (!color)
                    throw std::runtime_error("Failed to create RGBA bitmap");

                unsigned char* target = static_cast<unsigned char*>(targetPointer);

                mask = CreateBitmap(width, height, 1, 1, nullptr);
                if (!mask)
                    throw std::runtime_error("Failed to create mask bitmap");

                for (LONG i = 0; i < width * height; ++i)
                {
                    target[i * 4 + 0] = data[i * 4 + 2];
                    target[i * 4 + 1] = data[i * 4 + 1];
                    target[i * 4 + 2] = data[i * 4 + 0];
                    target[i * 4 + 3] = data[i * 4 + 3];
                }

                ICONINFO iconInfo = {};
                iconInfo.fIcon = FALSE;
                iconInfo.xHotspot = static_cast<DWORD>(hotSpot.v[0]);
                iconInfo.yHotspot = static_cast<int>(size.v[1]) - static_cast<DWORD>(hotSpot.v[1]) - 1;
                iconInfo.hbmMask = mask;
                iconInfo.hbmColor = color;

                ownedCursor = CreateIconIndirect(&iconInfo);
                if (!ownedCursor)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create cursor");

                cursor = ownedCursor;

                if (dc) ReleaseDC(nullptr, dc);
                dc = nullptr;
                if (color) DeleteObject(color);
                color = nullptr;
                if (mask) DeleteObject(mask);
                mask = nullptr;
            }
        }

        CursorWin::~CursorWin()
        {
            if (ownedCursor) DestroyCursor(cursor);
            if (dc) ReleaseDC(nullptr, dc);
            if (color) DeleteObject(color);
            if (mask) DeleteObject(mask);
        }
    } // namespace input
} // namespace ouzel
