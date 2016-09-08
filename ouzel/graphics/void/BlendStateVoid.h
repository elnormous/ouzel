// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "graphics/BlendState.h"

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class RendererVoid;

        class BlendStateVoid: public BlendState
        {
            friend RendererVoid;
        public:
            virtual ~BlendStateVoid();

        protected:
            BlendStateVoid();
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
