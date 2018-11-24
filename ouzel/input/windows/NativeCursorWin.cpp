// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <system_error>
#include "NativeCursorWin.hpp"

namespace ouzel
{
    namespace input
    {
        NativeCursorWin::NativeCursorWin(SystemCursor systemCursor)
        {
            switch (systemCursor)
            {
                case SystemCursor::DEFAULT:
                case SystemCursor::ARROW:
                    cursor = LoadCursor(nullptr, IDC_ARROW);
                    break;
                case SystemCursor::HAND:
                    cursor = LoadCursor(nullptr, IDC_HAND);
                    break;
                case SystemCursor::HORIZONTAL_RESIZE:
                    cursor = LoadCursor(nullptr, IDC_SIZEWE);
                    break;
                case SystemCursor::VERTICAL_RESIZE:
                    cursor = LoadCursor(nullptr, IDC_SIZENS);
                    break;
                case SystemCursor::CROSS:
                    cursor = LoadCursor(nullptr, IDC_CROSS);
                    break;
                case SystemCursor::I_BEAM:
                    cursor = LoadCursor(nullptr, IDC_IBEAM);
                    break;
                default:
                    throw std::runtime_error("Invalid cursor");
                    break;
            }

            if (!cursor)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to load cursor");

            shared = true;
        }

        NativeCursorWin::NativeCursorWin(const std::vector<uint8_t>& data,
                                         const Size2& size,
                                         graphics::PixelFormat pixelFormat,
                                         const Vector2& hotSpot)
        {
            if (!data.empty())
            {
                LONG width = static_cast<LONG>(size.width);
                LONG height = static_cast<LONG>(size.height);

                BITMAPV5HEADER bitmapHeader;
                ZeroMemory(&bitmapHeader, sizeof(bitmapHeader));
                bitmapHeader.bV5Size = sizeof(BITMAPV5HEADER);
                bitmapHeader.bV5Width = width;
                bitmapHeader.bV5Height = -height;
                bitmapHeader.bV5Planes = 1;
                bitmapHeader.bV5BitCount = 32;
                bitmapHeader.bV5Compression = BI_BITFIELDS;
                bitmapHeader.bV5RedMask = 0x00ff0000;
                bitmapHeader.bV5GreenMask = 0x000ff00;
                bitmapHeader.bV5BlueMask = 0x000000ff;
                bitmapHeader.bV5AlphaMask = 0xff000000;

                HDC dc = GetDC(nullptr);
                unsigned char* target = nullptr;
                HBITMAP color = CreateDIBSection(dc,
                                                 reinterpret_cast<BITMAPINFO*>(&bitmapHeader),
                                                 DIB_RGB_COLORS,
                                                 reinterpret_cast<void**>(&target),
                                                 nullptr,
                                                 static_cast<DWORD>(0));
                ReleaseDC(nullptr, dc);

                if (!color)
                    throw std::runtime_error("Failed to create RGBA bitmap");

                HBITMAP mask = CreateBitmap(width, height, 1, 1, nullptr);
                if (!mask)
                {
                    DeleteObject(color);
                    throw std::runtime_error("Failed to create mask bitmap");
                }

                for (LONG i = 0; i < width * height; ++i)
                {
                    target[i * 4 + 0] = data[i * 4 + 2];
                    target[i * 4 + 1] = data[i * 4 + 1];
                    target[i * 4 + 2] = data[i * 4 + 0];
                    target[i * 4 + 3] = data[i * 4 + 3];
                }

                ICONINFO iconInfo;
                ZeroMemory(&iconInfo, sizeof(iconInfo));
                iconInfo.fIcon = FALSE;
                iconInfo.xHotspot = static_cast<DWORD>(hotSpot.x);
                iconInfo.yHotspot = static_cast<int>(size.height) - static_cast<DWORD>(hotSpot.y) - 1;
                iconInfo.hbmMask = mask;
                iconInfo.hbmColor = color;

                cursor = CreateIconIndirect(&iconInfo);

                DeleteObject(color);
                DeleteObject(mask);

                if (!cursor)
                    throw std::runtime_error("Failed to create cursor");

                shared = false;
            }
        }

        NativeCursorWin::~NativeCursorWin()
        {
            if (cursor && !shared) DestroyCursor(cursor);
        }
    } // namespace input
} // namespace ouzel
