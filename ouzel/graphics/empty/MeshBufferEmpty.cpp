// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferEmpty.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferEmpty::MeshBufferEmpty()
        {
        }

        MeshBufferEmpty::~MeshBufferEmpty()
        {
        }

        bool MeshBufferEmpty::upload()
        {
            uploadData.dirty = false;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
