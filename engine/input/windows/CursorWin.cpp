// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <system_error>
#include "CursorWin.hpp"
#include "../../core/windows/DeviceContext.hpp"

namespace ouzel::input::windows
{
    namespace
    {
        class Bitmap final
        {
        public:
            Bitmap(HBITMAP b) noexcept:
                bitmap{b}
            {
            }

            ~Bitmap()
            {
                if (bitmap) DeleteObject(bitmap);
            }

            Bitmap(Bitmap&& other) noexcept:
                bitmap{other.bitmap},
            {
                other.bitmap = nullptr;
            }

            Bitmap& operator=(Bitmap&& other) noexcept
            {
                if (&other == this) return *this;
                if (bitmap) DeleteObject(bitmap);
                bitmap = other.bitmap;
                other.bitmap = nullptr;
                return *this;
            }

            operator HBITMAP() const noexcept { return bitmap; }

        private:
            HBITMAP bitmap = nullptr;
        };
    }

    Cursor::Cursor(SystemCursor systemCursor)
    {
        switch (systemCursor)
        {
            case SystemCursor::arrow:
                cursor = LoadCursor(nullptr, IDC_ARROW);
                break;
            case SystemCursor::hand:
                cursor = LoadCursor(nullptr, IDC_HAND);
                break;
            case SystemCursor::horizontalResize:
                cursor = LoadCursor(nullptr, IDC_SIZEWE);
                break;
            case SystemCursor::verticalResize:
                cursor = LoadCursor(nullptr, IDC_SIZENS);
                break;
            case SystemCursor::cross:
                cursor = LoadCursor(nullptr, IDC_CROSS);
                break;
            case SystemCursor::iBeam:
                cursor = LoadCursor(nullptr, IDC_IBEAM);
                break;
            default:
                throw std::runtime_error("Invalid cursor");
        }

        if (!cursor)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to load cursor");
    }

    Cursor::Cursor(const std::vector<std::uint8_t>& data,
                   const Size2F& size,
                   graphics::PixelFormat pixelFormat,
                   const Vector2F& hotSpot)
    {
        if (!data.empty())
        {
            const auto width = static_cast<LONG>(size.v[0]);
            const auto height = static_cast<LONG>(size.v[1]);

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

            core::windows::DeviceContext deviceContext;
            void* targetPointer = nullptr;
            HBITMAP color = CreateDIBSection(deviceContext,
                                             reinterpret_cast<BITMAPINFO*>(&bitmapHeader),
                                             DIB_RGB_COLORS,
                                             &targetPointer,
                                             nullptr,
                                             DWORD{0});

            if (!color)
                throw std::runtime_error("Failed to create RGBA bitmap");

            Bitmap colorBitmap = color;

            auto target = static_cast<unsigned char*>(targetPointer);

            HBITMAP mask = CreateBitmap(width, height, 1, 1, nullptr);
            if (!mask)
                throw std::runtime_error("Failed to create mask bitmap");

            Bitmap maskBitmap = mask;

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
            iconInfo.hbmMask = maskBitmap;
            iconInfo.hbmColor = colorBitmap;

            ownedCursor = CreateIconIndirect(&iconInfo);
            if (!ownedCursor)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to create cursor");

            cursor = ownedCursor;
        }
    }

    Cursor::~Cursor()
    {
        if (ownedCursor) DestroyCursor(cursor);
    }
}
