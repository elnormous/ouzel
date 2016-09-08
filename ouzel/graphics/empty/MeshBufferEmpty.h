// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/MeshBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererEmpty;

        class MeshBufferEmpty: public MeshBuffer
        {
            friend RendererEmpty;
        public:
            virtual ~MeshBufferEmpty();

        protected:
            MeshBufferEmpty();
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
