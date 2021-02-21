// Ouzel by Elviss Strazdins

#include <system_error>
#include "CursorWin.hpp"
#include "../../platform/winapi/DeviceContext.hpp"

namespace ouzel::input::windows
{
    namespace
    {
        class Bitmap final
        {
        public:
            Bitmap(int width, int height, UINT planes, UINT bitCount, const void* bits):
                bitmap{CreateBitmap(width, height, planes, bitCount, bits)}
            {
                if (!bitmap)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create mask bitmap");
            }

            Bitmap(const platform::winapi::DeviceContext& deviceContext,
                   const BITMAPINFO* bitmapInfo,
                   UINT usage,
                   void** bits,
                   HANDLE section,
                   DWORD offset):
                bitmap{CreateDIBSection(deviceContext,
                                        bitmapInfo,
                                        usage,
                                        bits,
                                        section,
                                        offset)}
            {
                if (!bitmap)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create mask bitmap");
            }

            ~Bitmap()
            {
                if (bitmap) DeleteObject(bitmap);
            }

            Bitmap(Bitmap&& other) noexcept:
                bitmap{other.bitmap}
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

    namespace
    {
        HCURSOR loadCursor(SystemCursor systemCursor)
        {
            switch (systemCursor)
            {
            case SystemCursor::arrow: return LoadCursor(nullptr, IDC_ARROW);
            case SystemCursor::hand: return LoadCursor(nullptr, IDC_HAND);
            case SystemCursor::horizontalResize: return LoadCursor(nullptr, IDC_SIZEWE);
            case SystemCursor::verticalResize: return LoadCursor(nullptr, IDC_SIZENS);
            case SystemCursor::cross: return LoadCursor(nullptr, IDC_CROSS);
            case SystemCursor::iBeam: return LoadCursor(nullptr, IDC_IBEAM);
            default: throw std::runtime_error("Invalid cursor");
            }
        }
    }

    Cursor::Cursor(SystemCursor systemCursor):
        cursor{loadCursor(systemCursor)}
    {
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

            BITMAPINFO bitmapInfo;
            bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
            bitmapInfo.bmiHeader.biWidth = width;
            bitmapInfo.bmiHeader.biHeight = -height;
            bitmapInfo.bmiHeader.biPlanes = 1;
            bitmapInfo.bmiHeader.biBitCount = 32;
            bitmapInfo.bmiHeader.biCompression = BI_RGB;

            platform::winapi::DeviceContext deviceContext;
            void* targetPointer = nullptr;
            Bitmap colorBitmap{deviceContext,
                               &bitmapInfo,
                               DIB_RGB_COLORS,
                               &targetPointer,
                               nullptr,
                               DWORD{0}};

            auto target = static_cast<unsigned char*>(targetPointer);

            for (LONG i = 0; i < width * height; ++i)
            {
                target[i * 4 + 0] = data[i * 4 + 2];
                target[i * 4 + 1] = data[i * 4 + 1];
                target[i * 4 + 2] = data[i * 4 + 0];
                target[i * 4 + 3] = data[i * 4 + 3];
            }
            
            Bitmap maskBitmap{width, height, 1, 1, nullptr};

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
