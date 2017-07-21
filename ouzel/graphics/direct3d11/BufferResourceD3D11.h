// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECT3D11

#include <d3d11.h>
#include "graphics/BufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class BufferResourceD3D11: public BufferResource
        {
        public:
            BufferResourceD3D11();
            virtual ~BufferResourceD3D11();

            virtual bool init(Buffer::Usage newUsage, bool newDynamic = true) override;
            virtual bool init(Buffer::Usage newUsage, const void* newData, uint32_t newSize, bool newDynamic) override;

            virtual bool setData(const void* newData, uint32_t newSize) override;

            ID3D11Buffer* getBuffer() const { return buffer; }

        protected:
            virtual bool upload() override;

            ID3D11Buffer* buffer = nullptr;
            UINT bufferSize = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
