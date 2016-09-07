// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferMetal.h"
#include "core/Engine.h"
#include "RendererMetal.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferMetal::MeshBufferMetal()
        {
        }

        MeshBufferMetal::~MeshBufferMetal()
        {
            if (indexBuffer)
            {
                [indexBuffer release];
            }

            if (vertexBuffer)
            {
                [vertexBuffer release];
            }
        }

        void MeshBufferMetal::free()
        {
            MeshBuffer::free();

            if (indexBuffer)
            {
                [indexBuffer release];
                indexBuffer = Nil;
            }

            if (vertexBuffer)
            {
                [vertexBuffer release];
                vertexBuffer = Nil;
            }
        }
    } // namespace graphics
} // namespace ouzel
