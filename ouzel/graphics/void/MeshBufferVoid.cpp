// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferVoid.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferVoid::MeshBufferVoid()
        {
        }

        MeshBufferVoid::~MeshBufferVoid()
        {
        }

        bool MeshBufferVoid::upload()
        {
            ready = true;
            dirty = false;
            return true;
        }
    } // namespace graphics
} // namespace ouzel
