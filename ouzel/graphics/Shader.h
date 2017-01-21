// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>

namespace ouzel
{
    namespace graphics
    {
        class ShaderResource;

        class Shader
        {
        public:
            Shader();

        private:
            std::unique_ptr<ShaderResource> resource;
        };
    } // namespace graphics
} // namespace ouzel
