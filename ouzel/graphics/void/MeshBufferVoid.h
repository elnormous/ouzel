// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/MeshBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererVoid;

        class MeshBufferVoid: public MeshBuffer
        {
            friend RendererVoid;
        public:
            virtual ~MeshBufferVoid();

        protected:
            MeshBufferVoid();
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
