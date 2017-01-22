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

            MTLIndexType getIndexType() const { return indexType; }
            NSUInteger getBytesPerIndex() const { return bytesPerIndex; }

        protected:
            virtual bool upload() override;

            MTLIndexType indexType;
            NSUInteger bytesPerIndex = 0;
        };
    } // namespace graphics
} // namespace ouzel
