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
        class MeshBufferD3D11: public MeshBuffer
        {
        public:
            MeshBufferD3D11();
            virtual ~MeshBufferD3D11();

        protected:
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
