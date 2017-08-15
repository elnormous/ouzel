// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "utils/Noncopyable.hpp"

namespace ouzel
{
    namespace graphics
    {
        class BlendState;
        class Texture;

        class Material: public Noncopyable
        {
        public:
            std::shared_ptr<BlendState> blendState;
            std::shared_ptr<Texture> texture;
            bool twoSided = false;
        };
    } // namespace graphics
} // namespace ouzel
