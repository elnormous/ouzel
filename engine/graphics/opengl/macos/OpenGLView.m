// Ouzel by Elviss Strazdins

#include "../../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OpenGLView.h"

@implementation OpenGLView
{
    NSOpenGLContext* openGlContext;
}

- (void)setOpenGLContext:(NSOpenGLContext*)context
{
    openGlContext = context;
}

- (void)setLayer:(CALayer*)layer
{
    [super setLayer:layer];

    [openGlContext update];
}
@end

#endif
