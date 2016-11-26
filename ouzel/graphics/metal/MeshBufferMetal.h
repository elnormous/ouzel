// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/MeshBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferMetal: public MeshBuffer
        {
        public:
            MeshBufferMetal();
            virtual ~MeshBufferMetal();

        protected:
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
