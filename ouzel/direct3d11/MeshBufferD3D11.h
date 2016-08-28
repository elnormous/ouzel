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

            ID3D11Buffer* getIndexBuffer() const { return indexBuffer; }
            ID3D11Buffer* getVertexBuffer() const { return vertexBuffer; }

            DXGI_FORMAT getIndexFormat() const { return indexFormat; }

        protected:
            MeshBufferD3D11();
            virtual bool update() override;
            virtual bool upload() override;

            bool uploadData(ID3D11Buffer* buffer, const std::vector<uint8_t>& data);

            ID3D11Buffer* indexBuffer = nullptr;
            UINT indexBufferSize = 0;

            ID3D11Buffer* vertexBuffer = nullptr;
            UINT vertexBufferSize = 0;

            DXGI_FORMAT indexFormat = DXGI_FORMAT_UNKNOWN;
            UINT bytesPerIndex = 0;
        };
    } // namespace graphics
} // namespace ouzel
