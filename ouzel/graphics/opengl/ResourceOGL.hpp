// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

namespace ouzel
{
    namespace graphics
    {
        class ResourceOGL
        {
        public:
            virtual bool reload() = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
