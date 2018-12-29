// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OpenGLView.h"
#include "OGLRenderDeviceMacOS.hpp"
#include "core/Engine.hpp"

@implementation OpenGLView

-(void)setLayer:(CALayer*)layer
{
    [super setLayer:layer];

    ouzel::graphics::OGLRenderDeviceMacOS* renderDevice = static_cast<ouzel::graphics::OGLRenderDeviceMacOS*>(ouzel::engine->getRenderer()->getDevice());

    [renderDevice->getOpenGLContext() update];
}

@end

#endif
