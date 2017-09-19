// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OpenGLView.h"

@implementation OpenGLView

+(Class)layerClass
{
    return [CAEAGLLayer class];
}

@end

#endif
