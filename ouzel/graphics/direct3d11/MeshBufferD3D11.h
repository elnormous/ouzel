// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <d3d11.h>
#include "graphics/MeshBufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferD3D11: public MeshBufferResource
        {
        public:
            MeshBufferD3D11();
            virtual ~MeshBufferD3D11();

            DXGI_FORMAT getIndexFormat() const { return indexFormat; }

        protected:
            virtual bool upload() override;

            DXGI_FORMAT indexFormat = DXGI_FORMAT_UNKNOWN;
        };
    } // namespace graphics
} // namespace ouzel
