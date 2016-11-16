// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "graphics/BlendState.h"

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class BlendStateEmpty: public BlendState
        {
        public:
            BlendStateEmpty();

        protected:
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
