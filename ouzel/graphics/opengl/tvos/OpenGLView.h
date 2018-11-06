// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#import <QuartzCore/QuartzCore.h>
#import "core/tvos/ViewTVOS.h"

@interface OpenGLView: ViewTVOS

@end

#endif

#endif // OPENGLVIEW_H
