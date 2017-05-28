// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/ShaderResource.h"

namespace ouzel
{
    namespace graphics
    {
        class ShaderResourceEmpty: public ShaderResource
        {
        public:
            ShaderResourceEmpty();

            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
