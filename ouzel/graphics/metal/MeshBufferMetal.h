// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/MeshBufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferMetal: public MeshBufferResource
        {
        public:
            MeshBufferMetal();
            virtual ~MeshBufferMetal();

        protected:
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
