// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferResource;

        class MeshBuffer
        {
        public:
            MeshBuffer();

        private:
            std::unique_ptr<MeshBufferResource> resource;
        };
    } // namespace graphics
} // namespace ouzel
