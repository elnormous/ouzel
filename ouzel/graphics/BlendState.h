// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "utils/Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class BlendStateResource;

        class BlendState: public Noncopyable
        {
        public:
            BlendState();

        private:
            std::unique_ptr<BlendStateResource> resource;
        };
    } // namespace graphics
} // namespace ouzel
