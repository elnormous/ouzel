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

        protected:
            MeshBufferD3D11();
        };
    } // namespace graphics
} // namespace ouzel
