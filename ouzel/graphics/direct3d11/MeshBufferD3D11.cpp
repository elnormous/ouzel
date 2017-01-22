// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferD3D11.h"

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
            if (!MeshBufferResource::upload())
            {
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
