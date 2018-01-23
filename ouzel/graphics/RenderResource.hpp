// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Noncopyable.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderResource: public Noncopyable
        {
        public:
            RenderResource() {}
            virtual ~RenderResource() {}

            virtual bool reload() { return true; }
        };
    } // namespace graphics
} // namespace ouzel
