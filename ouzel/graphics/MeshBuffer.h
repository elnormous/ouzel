// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferResource;

        class MeshBuffer: public Noncopyable
        {
        public:
            MeshBuffer();

        private:
            std::unique_ptr<MeshBufferResource> resource;
        };
    } // namespace graphics
} // namespace ouzel
