// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include <atomic>
#define NOMINMAX
#include <d3d11.h>
#include "graphics/MeshBuffer.h"

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

            virtual bool init(bool newDynamicIndexBuffer = true, bool newDynamicVertexBuffer = true) override;
            virtual bool initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                        uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                        const void* newVertices, uint32_t newVertexAttributes,
                                        uint32_t newVertexCount, bool newDynamicVertexBuffer) override;

            virtual bool setIndexSize(uint32_t indexSize) override;

            virtual bool uploadIndices(const void* newIndices, uint32_t newIndexCount) override;
            virtual bool uploadVertices(const void* newVertices, uint32_t newVertexCount) override;

            ID3D11Buffer* getIndexBuffer() const { return indexBuffer; }
            ID3D11Buffer* getVertexBuffer() const { return vertexBuffer; }

            DXGI_FORMAT getIndexFormat() const { return indexFormat; }

        protected:
            MeshBufferD3D11();
            virtual bool update() override;

            bool uploadData(ID3D11Buffer* buffer, const std::vector<uint8_t>& data);

            ID3D11Buffer* indexBuffer = nullptr;
            UINT indexBufferSize = 0;

            ID3D11Buffer* vertexBuffer = nullptr;
            UINT vertexBufferSize = 0;

            DXGI_FORMAT indexFormat = DXGI_FORMAT_UNKNOWN;

            std::vector<uint8_t> indexData;
            std::vector<uint8_t> vertexData;
            std::atomic<bool> indexBufferDirty;
            std::atomic<bool> vertexBufferDirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
