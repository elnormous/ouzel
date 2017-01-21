// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class ShaderResource;

        class Shader: public Noncopyable
        {
        public:
            Shader();

        private:
            std::unique_ptr<ShaderResource> resource;
        };
    } // namespace graphics
} // namespace ouzel
