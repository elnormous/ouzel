// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/BufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class BufferResourceEmpty: public BufferResource
        {
        public:
            BufferResourceEmpty();

            virtual bool upload() override;
        };
    }
}
