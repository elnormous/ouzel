// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>

namespace ouzel
{
    namespace graphics
    {
        class IndexBufferResource;

        class IndexBuffer
        {
        public:
            IndexBuffer();

        private:
            std::unique_ptr<IndexBufferResource> resource;
        };
    } // namespace graphics
} // namespace ouzel
