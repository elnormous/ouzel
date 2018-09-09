// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <d3d11.h>
#include "graphics/RenderResource.hpp"
#include "graphics/Buffer.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11;

        class BufferResourceD3D11: public RenderResource
        {
        public:
            explicit BufferResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11);
            virtual ~BufferResourceD3D11();

            void init(Buffer::Usage newUsage, uint32_t newFlags,
                      const std::vector<uint8_t>& newData,
                      uint32_t newSize);

            void setData(const std::vector<uint8_t>& newData);

            inline uint32_t getFlags() const { return flags; }
            inline Buffer::Usage getUsage() const { return usage; }
            inline uint32_t getSize() const { return static_cast<uint32_t>(data.size()); }

            ID3D11Buffer* getBuffer() const { return buffer; }

        private:
            void createBuffer(UINT newSize);

            Buffer::Usage usage;
            uint32_t flags = 0;
            std::vector<uint8_t> data;

            ID3D11Buffer* buffer = nullptr;
            UINT bufferSize = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
