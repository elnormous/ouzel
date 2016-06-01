// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "opengl/RendererOGL.h"

namespace ouzel
{
    class Engine;
    
    namespace graphics
    {
        class RendererOGLLinux: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLLinux() {}
        };
    } // namespace graphics
} // namespace ouzel
