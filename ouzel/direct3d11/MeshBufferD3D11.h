// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <d3d11.h>
#include "MeshBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11;

        class MeshBufferD3D11: public MeshBuffer
        {
            friend RendererD3D11;
        public:
            virtual ~MeshBufferD3D11();
            virtual void free() override;

            virtual bool initFromData(const void* newIndices, uint32_t newIndexSize, uint32_t newIndexCount, bool newDynamicIndexBuffer, const void* newVertices, uint32_t newVertexAttributes, uint32_t newVertexCount, bool newDynamicVertexBuffer) override;

            virtual bool setIndexSize(uint32_t indexSize) override;

            virtual bool uploadIndices(const void* indices, uint32_t indexCount) override;
            virtual bool uploadVertices(const void* vertices, uint32_t vertexCount) override;

            ID3D11Buffer* getIndexBuffer() const { return indexBuffer; }
            ID3D11Buffer* getVertexBuffer() const { return vertexBuffer; }

            DXGI_FORMAT getIndexFormat() const { return indexFormat; }

        protected:
            MeshBufferD3D11();

            bool updateIndexFormat();
            bool createIndexBuffer(const void* indices, uint32_t size);
            bool createVertexBuffer(const void* vertices, uint32_t size);
            bool uploadData(ID3D11Buffer* buffer, const void* data, uint32_t size);

            ID3D11Buffer* indexBuffer = nullptr;
            uint32_t indexBufferSize = 0;

            ID3D11Buffer* vertexBuffer = nullptr;
            uint32_t vertexBufferSize = 0;

            DXGI_FORMAT indexFormat = DXGI_FORMAT_UNKNOWN;
        };
    } // namespace graphics
} // namespace ouzel
