// Ouzel by Elviss Strazdins

#include "../../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OpenGLView.h"

@implementation OpenGLView
{
    NSOpenGLContext* openGLContext;
}

- (void)setOpenGLContext:(NSOpenGLContext*)context
{
    openGLContext = context;
}

- (void)setLayer:(CALayer*)layer
{
    [super setLayer:layer];

    [openGLContext update];
}
@end

#endif
