// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.hpp"

#if OUZEL_SUPPORTS_DIRECT3D11

#include <d3d11.h>
#include "graphics/MeshBufferInterface.hpp"

namespace ouzel
{
    namespace graphics
    {
        class BufferInterfaceD3D11;

        class MeshBufferInterfaceD3D11: public MeshBufferInterface
        {
        public:
            MeshBufferInterfaceD3D11();
            virtual ~MeshBufferInterfaceD3D11();

            virtual bool init(uint32_t newIndexSize, BufferInterface* newIndexBuffer,
                              const std::vector<VertexAttribute>& newVertexAttributes, BufferInterface* newVertexBuffer) override;

            virtual bool setIndexSize(uint32_t newIndexSize) override;
            virtual bool setIndexBuffer(BufferInterface* newIndexBuffer) override;
            virtual bool setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes) override;
            virtual bool setVertexBuffer(BufferInterface* newVertexBuffer) override;

            DXGI_FORMAT getIndexFormat() const { return indexFormat; }

            BufferInterfaceD3D11* getIndexBufferD3D11() const { return indexBufferD3D11; }
            BufferInterfaceD3D11* getVertexBufferD3D11() const { return vertexBufferD3D11; }

        protected:
            DXGI_FORMAT indexFormat = DXGI_FORMAT_UNKNOWN;

            BufferInterfaceD3D11* indexBufferD3D11 = nullptr;
            BufferInterfaceD3D11* vertexBufferD3D11 = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif
