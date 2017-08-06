// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECT3D11

#include <d3d11.h>
#include "graphics/BufferInterface.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11;

        class BufferInterfaceD3D11: public BufferInterface
        {
        public:
            BufferInterfaceD3D11(RendererD3D11* aRendererD3D11);
            virtual ~BufferInterfaceD3D11();

            virtual bool init(Buffer::Usage newUsage, uint32_t newFlags = 0, uint32_t newSize = 0) override;
            virtual bool init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, uint32_t newFlags = 0) override;

            virtual bool setData(const std::vector<uint8_t>& newData) override;

            ID3D11Buffer* getBuffer() const { return buffer; }

        protected:
            bool createBuffer();

            RendererD3D11* rendererD3D11;

            ID3D11Buffer* buffer = nullptr;
            UINT bufferSize = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
