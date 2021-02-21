// Ouzel by Elviss Strazdins

#include "../../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OpenGLView.h"

@implementation OpenGLView

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}
@end

#endif
