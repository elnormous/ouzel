// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <d3d11.h>
#include "MeshBuffer.h"

namespace ouzel
{
    namespace video
    {
        class RendererD3D11;

        class MeshBufferD3D11: public MeshBuffer
        {
            friend RendererD3D11;
        public:
            virtual ~MeshBufferD3D11();

            virtual bool initFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes) override;

            virtual bool uploadIndices(const void* indices, uint32_t indexCount) override;
            virtual bool uploadVertices(const void* vertices, uint32_t vertexCount) override;

            ID3D11Buffer* getIndexBuffer() const { return _indexBuffer; }
            ID3D11Buffer* getVertexBuffer() const { return _vertexBuffer; }

            DXGI_FORMAT getIndexFormat() const { return _indexFormat; }

        protected:
            MeshBufferD3D11();

            void clean();
            bool createIndexBuffer(const void* indices, uint32_t size);
            bool createVertexBuffer(const void* vertices, uint32_t size);
            bool uploadData(ID3D11Buffer* buffer, const void* data, uint32_t size);

            ID3D11Buffer* _indexBuffer = nullptr;
            uint32_t _indexBufferSize = 0;

            ID3D11Buffer* _vertexBuffer = nullptr;
            uint32_t _vertexBufferSize = 0;

            DXGI_FORMAT _indexFormat = DXGI_FORMAT_UNKNOWN;
        };
    } // namespace video
} // namespace ouzel
