// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <d3d11.h>
#include "graphics/BufferResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11;

        class BufferResourceD3D11: public BufferResource
        {
        public:
            explicit BufferResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11);
            virtual ~BufferResourceD3D11();

            virtual void init(Buffer::Usage newUsage, uint32_t newFlags,
                              const std::vector<uint8_t>& newData,
                              uint32_t newSize) override;

            virtual void setData(const std::vector<uint8_t>& newData) override;

            ID3D11Buffer* getBuffer() const { return buffer; }

        private:
            void createBuffer(UINT newSize);

            ID3D11Buffer* buffer = nullptr;
            UINT bufferSize = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
