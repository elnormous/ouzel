// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>

namespace ouzel
{
    namespace graphics
    {
        class VertexBufferResource;
        
        class VertexBuffer
        {
        public:
            VertexBuffer();

        private:
            std::unique_ptr<VertexBufferResource> resource;
        };
    } // namespace graphics
} // namespace ouzel
