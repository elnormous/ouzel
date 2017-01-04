// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferD3D11.h"
#include "core/Engine.h"
#include "RendererD3D11.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferD3D11::MeshBufferD3D11()
        {
        }

        MeshBufferD3D11::~MeshBufferD3D11()
        {
        }

        bool MeshBufferD3D11::upload()
        {
            if (!MeshBuffer::upload())
            {
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
