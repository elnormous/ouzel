// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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
            explicit BufferResourceD3D11(RenderDeviceD3D11* initRenderDeviceD3D11);
            virtual ~BufferResourceD3D11();

            virtual bool init(Buffer::Usage newUsage, uint32_t newFlags = 0, uint32_t newSize = 0) override;
            virtual bool init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, uint32_t newFlags = 0) override;

            virtual bool setData(const std::vector<uint8_t>& newData) override;

            ID3D11Buffer* getBuffer() const { return buffer; }

        protected:
            bool createBuffer();

            RenderDeviceD3D11* renderDeviceD3D11;

            ID3D11Buffer* buffer = nullptr;
            UINT bufferSize = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
