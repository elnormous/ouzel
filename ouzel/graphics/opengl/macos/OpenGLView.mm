// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OpenGLView.h"
#include "RenderDeviceOGLMacOS.hpp"
#include "core/Engine.hpp"

@implementation OpenGLView

-(void)setLayer:(CALayer*)layer
{
    [super setLayer:layer];

    ouzel::graphics::RenderDeviceOGLMacOS* renderDevice = static_cast<ouzel::graphics::RenderDeviceOGLMacOS*>(ouzel::engine->getRenderer()->getDevice());

    [renderDevice->getOpenGLContext() update];
}

@end

#endif
