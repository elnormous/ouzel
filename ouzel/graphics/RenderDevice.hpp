// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Renderer.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDevice
        {
        public:
            virtual ~RenderDevice();

        protected:
            RenderDevice(Renderer::Driver aDriver);

            Renderer::Driver driver;
        };
    } // namespace graphics
} // namespace ouzel
