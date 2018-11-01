// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <d3d11.h>
#include "graphics/d3d11/RenderResourceD3D11.hpp"
#include "graphics/Buffer.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11;

        class BufferResourceD3D11 final: public RenderResourceD3D11
        {
        public:
            explicit BufferResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11);
            ~BufferResourceD3D11();

            void init(Buffer::Usage newUsage, uint32_t newFlags,
                      const std::vector<uint8_t>& data,
                      uint32_t newSize);

            void setData(const std::vector<uint8_t>& data);

            inline uint32_t getFlags() const { return flags; }
            inline Buffer::Usage getUsage() const { return usage; }
            inline UINT getSize() const { return size; }

            ID3D11Buffer* getBuffer() const { return buffer; }

        private:
            void createBuffer(UINT newSize, const std::vector<uint8_t>& data);

            Buffer::Usage usage;
            uint32_t flags = 0;

            ID3D11Buffer* buffer = nullptr;
            UINT size = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
