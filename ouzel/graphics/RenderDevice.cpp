// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDevice::RenderDevice(Renderer::Driver aDriver):
            driver(aDriver)
        {
        }

        RenderDevice::~RenderDevice()
        {
        }
    } // namespace graphics
} // namespace ouzel
