#ifndef FLUXION_GRAPHICS_OPENGLVIEW_H
#define FLUXION_GRAPHICS_OPENGLVIEW_H

#include "../../../core/Setup.h"

#if FLUXION_COMPILE_OPENGL

#import <AppKit/NSOpenGL.h>
#import "../../../core/macos/ViewMacOS.h"

@interface OpenGLView: View

- (void)setOpenGLContext:(NSOpenGLContext*)context;
@end

#endif

#endif // FLUXION_GRAPHICS_OPENGLVIEW_H
