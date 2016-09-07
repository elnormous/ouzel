// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include "graphics/MeshBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererMetal;

        class MeshBufferMetal: public MeshBuffer
        {
            friend RendererMetal;
        public:
            virtual ~MeshBufferMetal();

        protected:
            MeshBufferMetal();
        };
    } // namespace graphics
} // namespace ouzel
