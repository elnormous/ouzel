// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Noncopyable.h"

#pragma once

namespace ouzel
{
    namespace video
    {
        class BlendState: public Noncopyable
        {
        public:
            virtual ~BlendState() {}
        };
    } // namespace video
} // namespace ouzel
