// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "utils/Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class BufferResource;

        class Buffer: public Noncopyable
        {
        public:
            enum class Usage
            {
                NONE,
                INDEX,
                VERTEX
            };

            Buffer();

        private:
            std::unique_ptr<BufferResource> resource;
        };
    } // namespace graphics
} // namespace ouzel
