// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "OpenGLView.h"
#include "Engine.h"
#include "RendererOGL.h"
#include "Input.h"
#include "Window.h"
#include "Utils.h"

static CVReturn renderCallback(CVDisplayLinkRef displayLink,
                               const CVTimeStamp* inNow,
                               const CVTimeStamp* inOutputTime,
                               CVOptionFlags flagsIn,
                               CVOptionFlags* flagsOut,
                               void* displayLinkContext)
{
    OUZEL_UNUSED(displayLink);
    OUZEL_UNUSED(inNow);
    OUZEL_UNUSED(inOutputTime);
    OUZEL_UNUSED(flagsIn);
    OUZEL_UNUSED(flagsOut);

    [(__bridge OpenGLView*)displayLinkContext draw];

    return kCVReturnSuccess;
}

using namespace ouzel;

@implementation OpenGLView

-(id)initWithFrame:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        // Create pixel format
        NSOpenGLPixelFormatAttribute attributes[] =
        {
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core, // ensure we're using 3.2
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADepthSize, 32, // set depth buffer size
            0
        };

        NSOpenGLPixelFormat* pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];

        // Create OpenGL context
        _openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:NULL];
        [_openGLContext setView:self];
        [_openGLContext makeCurrentContext];

        std::shared_ptr<graphics::RendererOGL> renderer = std::static_pointer_cast<graphics::RendererOGL>(sharedEngine->getRenderer());

        GLint swapInt = 1;
        [_openGLContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

        _displayId = (CGDirectDisplayID)[[[[_window screen] deviceDescription]objectForKey:@"NSScreenNumber"] unsignedIntValue];

        CVDisplayLinkCreateWithCGDisplay(_displayId, &_displayLink);
        CVDisplayLinkSetOutputCallback(_displayLink, renderCallback, (__bridge void *)self);

        CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_displayLink, [_openGLContext CGLContextObj], [pixelFormat CGLPixelFormatObj]);

        CVDisplayLinkStart(_displayLink);
    }

    return self;
}

-(void)dealloc
{
    [self close];

    [super dealloc];
}

-(BOOL)isFlipped
{
    return YES;
}

-(BOOL)acceptsFirstMouse:(NSEvent*)event
{
    OUZEL_UNUSED(event);
    return YES;
}

-(void)prepareOpenGL
{
    _running = YES;
}

-(void)close
{
    _running = NO;

    if (_displayLink)
    {
        CGLLockContext([_openGLContext CGLContextObj]);
        [_openGLContext makeCurrentContext];

        CVDisplayLinkRelease(_displayLink);
        _displayLink = Nil;

        CGLUnlockContext([_openGLContext CGLContextObj]);
    }

    if (_openGLContext)
    {
        [_openGLContext release];
        _openGLContext = Nil;
    }
}

-(void)changeDisplay
{
    CGDirectDisplayID displayId = (CGDirectDisplayID)[[[[_window screen] deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

    if(displayId != 0 && _displayId != displayId)
    {
        CVDisplayLinkSetCurrentCGDisplay(_displayLink, displayId);
        _displayId = displayId;
    }
}

-(void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];

    if (!_running) return;

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];

    [_openGLContext update];

    sharedEngine->getWindow()->setSize(Size2(static_cast<float>(newSize.width),
                                             static_cast<float>(newSize.height)));

    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)lockFocus
{
    [super lockFocus];
    if ([_openGLContext view] != self)
    {
        [_openGLContext setView:self];
    }

    [_openGLContext makeCurrentContext];
}

-(void)draw
{
    if (!_running) return;

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];

    bool quit = !sharedEngine->run();

    [_openGLContext flushBuffer];

    CGLUnlockContext([_openGLContext CGLContextObj]);

    if (quit)
    {
        if ([NSThread isMainThread])
        {
            [self.window close];
        }
        else
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                [self.window close];
            });
        }
    }
}

-(BOOL)acceptsFirstResponder
{
    return YES;
}

static uint32_t getModifiers(NSEvent* theEvent, bool includeMouse = false)
{
    uint32_t modifiers = 0;

    if (theEvent.modifierFlags & NSShiftKeyMask) modifiers |= Event::SHIFT_DOWN;
    if (theEvent.modifierFlags & NSAlternateKeyMask) modifiers |= Event::ALT_DOWN;
    if (theEvent.modifierFlags & NSControlKeyMask) modifiers |= Event::CONTROL_DOWN;
    if (theEvent.modifierFlags & NSCommandKeyMask) modifiers |= Event::COMMAND_DOWN;
    if (theEvent.modifierFlags & NSFunctionKeyMask) modifiers |= Event::FUNCTION_DOWN;

    if (includeMouse)
    {
        if (NSEvent.pressedMouseButtons & (1 << 0)) modifiers |= Event::LEFT_MOUSE_DOWN;
        if (NSEvent.pressedMouseButtons & (1 << 1)) modifiers |= Event::RIGHT_MOUSE_DOWN;
        if (NSEvent.pressedMouseButtons & (1 << 2)) modifiers |= Event::MIDDLE_MOUSE_DOWN;
    }

    return modifiers;
}

// defines from Carbon
enum {
    kVK_ANSI_A                    = 0x00,
    kVK_ANSI_S                    = 0x01,
    kVK_ANSI_D                    = 0x02,
    kVK_ANSI_F                    = 0x03,
    kVK_ANSI_H                    = 0x04,
    kVK_ANSI_G                    = 0x05,
    kVK_ANSI_Z                    = 0x06,
    kVK_ANSI_X                    = 0x07,
    kVK_ANSI_C                    = 0x08,
    kVK_ANSI_V                    = 0x09,
    kVK_ANSI_B                    = 0x0B,
    kVK_ANSI_Q                    = 0x0C,
    kVK_ANSI_W                    = 0x0D,
    kVK_ANSI_E                    = 0x0E,
    kVK_ANSI_R                    = 0x0F,
    kVK_ANSI_Y                    = 0x10,
    kVK_ANSI_T                    = 0x11,
    kVK_ANSI_1                    = 0x12,
    kVK_ANSI_2                    = 0x13,
    kVK_ANSI_3                    = 0x14,
    kVK_ANSI_4                    = 0x15,
    kVK_ANSI_6                    = 0x16,
    kVK_ANSI_5                    = 0x17,
    kVK_ANSI_Equal                = 0x18,
    kVK_ANSI_9                    = 0x19,
    kVK_ANSI_7                    = 0x1A,
    kVK_ANSI_Minus                = 0x1B,
    kVK_ANSI_8                    = 0x1C,
    kVK_ANSI_0                    = 0x1D,
    kVK_ANSI_RightBracket         = 0x1E,
    kVK_ANSI_O                    = 0x1F,
    kVK_ANSI_U                    = 0x20,
    kVK_ANSI_LeftBracket          = 0x21,
    kVK_ANSI_I                    = 0x22,
    kVK_ANSI_P                    = 0x23,
    kVK_ANSI_L                    = 0x25,
    kVK_ANSI_J                    = 0x26,
    kVK_ANSI_Quote                = 0x27,
    kVK_ANSI_K                    = 0x28,
    kVK_ANSI_Semicolon            = 0x29,
    kVK_ANSI_Backslash            = 0x2A,
    kVK_ANSI_Comma                = 0x2B,
    kVK_ANSI_Slash                = 0x2C,
    kVK_ANSI_N                    = 0x2D,
    kVK_ANSI_M                    = 0x2E,
    kVK_ANSI_Period               = 0x2F,
    kVK_ANSI_Grave                = 0x32,
    kVK_ANSI_KeypadDecimal        = 0x41,
    kVK_ANSI_KeypadMultiply       = 0x43,
    kVK_ANSI_KeypadPlus           = 0x45,
    kVK_ANSI_KeypadClear          = 0x47,
    kVK_ANSI_KeypadDivide         = 0x4B,
    kVK_ANSI_KeypadEnter          = 0x4C,
    kVK_ANSI_KeypadMinus          = 0x4E,
    kVK_ANSI_KeypadEquals         = 0x51,
    kVK_ANSI_Keypad0              = 0x52,
    kVK_ANSI_Keypad1              = 0x53,
    kVK_ANSI_Keypad2              = 0x54,
    kVK_ANSI_Keypad3              = 0x55,
    kVK_ANSI_Keypad4              = 0x56,
    kVK_ANSI_Keypad5              = 0x57,
    kVK_ANSI_Keypad6              = 0x58,
    kVK_ANSI_Keypad7              = 0x59,
    kVK_ANSI_Keypad8              = 0x5B,
    kVK_ANSI_Keypad9              = 0x5C
};

/* keycodes for keys that are independent of keyboard layout*/
enum {
    kVK_Return                    = 0x24,
    kVK_Tab                       = 0x30,
    kVK_Space                     = 0x31,
    kVK_Delete                    = 0x33,
    kVK_Escape                    = 0x35,
    kVK_Command                   = 0x37,
    kVK_Shift                     = 0x38,
    kVK_CapsLock                  = 0x39,
    kVK_Option                    = 0x3A,
    kVK_Control                   = 0x3B,
    kVK_RightShift                = 0x3C,
    kVK_RightOption               = 0x3D,
    kVK_RightControl              = 0x3E,
    kVK_Function                  = 0x3F,
    kVK_F17                       = 0x40,
    kVK_VolumeUp                  = 0x48,
    kVK_VolumeDown                = 0x49,
    kVK_Mute                      = 0x4A,
    kVK_F18                       = 0x4F,
    kVK_F19                       = 0x50,
    kVK_F20                       = 0x5A,
    kVK_F5                        = 0x60,
    kVK_F6                        = 0x61,
    kVK_F7                        = 0x62,
    kVK_F3                        = 0x63,
    kVK_F8                        = 0x64,
    kVK_F9                        = 0x65,
    kVK_F11                       = 0x67,
    kVK_F13                       = 0x69,
    kVK_F16                       = 0x6A,
    kVK_F14                       = 0x6B,
    kVK_F10                       = 0x6D,
    kVK_F12                       = 0x6F,
    kVK_F15                       = 0x71,
    kVK_Help                      = 0x72,
    kVK_Home                      = 0x73,
    kVK_PageUp                    = 0x74,
    kVK_ForwardDelete             = 0x75,
    kVK_F4                        = 0x76,
    kVK_End                       = 0x77,
    kVK_F2                        = 0x78,
    kVK_PageDown                  = 0x79,
    kVK_F1                        = 0x7A,
    kVK_LeftArrow                 = 0x7B,
    kVK_RightArrow                = 0x7C,
    kVK_DownArrow                 = 0x7D,
    kVK_UpArrow                   = 0x7E
};

/* ISO keyboards only*/
enum {
    kVK_ISO_Section               = 0x0A
};

/* JIS keyboards only*/
enum {
    kVK_JIS_Yen                   = 0x5D,
    kVK_JIS_Underscore            = 0x5E,
    kVK_JIS_KeypadComma           = 0x5F,
    kVK_JIS_Eisu                  = 0x66,
    kVK_JIS_Kana                  = 0x68
};

static input::KeyboardKey convertKeyCode(unsigned short keyCode)
{
    switch (keyCode)
    {
        case kVK_UpArrow: return input::KeyboardKey::UP;
        case kVK_DownArrow: return input::KeyboardKey::DOWN;
        case kVK_LeftArrow: return input::KeyboardKey::LEFT;
        case kVK_RightArrow: return input::KeyboardKey::RIGHT;
        case kVK_F1: return input::KeyboardKey::F1;
        case kVK_F2: return input::KeyboardKey::F2;
        case kVK_F3: return input::KeyboardKey::F3;
        case kVK_F4: return input::KeyboardKey::F4;
        case kVK_F5: return input::KeyboardKey::F5;
        case kVK_F6: return input::KeyboardKey::F6;
        case kVK_F7: return input::KeyboardKey::F7;
        case kVK_F8: return input::KeyboardKey::F8;
        case kVK_F9: return input::KeyboardKey::F9;
        case kVK_F10: return input::KeyboardKey::F10;
        case kVK_F11: return input::KeyboardKey::F11;
        case kVK_F12: return input::KeyboardKey::F12;
        case kVK_F13: return input::KeyboardKey::F13;
        case kVK_F14: return input::KeyboardKey::F14;
        case kVK_F15: return input::KeyboardKey::F15;
        case kVK_F16: return input::KeyboardKey::F16;
        case kVK_F17: return input::KeyboardKey::F17;
        case kVK_F18: return input::KeyboardKey::F18;
        case kVK_F19: return input::KeyboardKey::F19;
        case kVK_F20: return input::KeyboardKey::F20;
        case kVK_Home: return input::KeyboardKey::HOME;
        case kVK_End: return input::KeyboardKey::END;
        case NSInsertFunctionKey: return input::KeyboardKey::INSERT;
        case kVK_ForwardDelete: return input::KeyboardKey::DEL;
        case kVK_Help: return input::KeyboardKey::HELP;
        case NSSelectFunctionKey: return input::KeyboardKey::SELECT;
        case NSPrintFunctionKey: return input::KeyboardKey::PRINT;
        case NSExecuteFunctionKey: return input::KeyboardKey::EXECUT;
        case NSPrintScreenFunctionKey: return input::KeyboardKey::SNAPSHOT;
        case NSPauseFunctionKey: return input::KeyboardKey::PAUSE;
        case NSScrollLockFunctionKey: return input::KeyboardKey::SCROLL;
        case kVK_Delete: return input::KeyboardKey::BACKSPACE;
        case kVK_Tab: return input::KeyboardKey::TAB;
        case kVK_Return: return input::KeyboardKey::RETURN;
        case kVK_Escape: return input::KeyboardKey::ESCAPE;
        case kVK_Control: return input::KeyboardKey::CONTROL;
        case kVK_RightControl: return input::KeyboardKey::RCONTROL;
        case kVK_Command: return input::KeyboardKey::MENU;
        case kVK_Shift: return input::KeyboardKey::SHIFT;
        case kVK_RightShift: return input::KeyboardKey::RSHIFT;
        case kVK_Space: return input::KeyboardKey::SPACE;

        case kVK_ANSI_A: return input::KeyboardKey::KEY_A;
        case kVK_ANSI_B: return input::KeyboardKey::KEY_B;
        case kVK_ANSI_C: return input::KeyboardKey::KEY_C;
        case kVK_ANSI_D: return input::KeyboardKey::KEY_D;
        case kVK_ANSI_E: return input::KeyboardKey::KEY_E;
        case kVK_ANSI_F: return input::KeyboardKey::KEY_F;
        case kVK_ANSI_G: return input::KeyboardKey::KEY_G;
        case kVK_ANSI_H: return input::KeyboardKey::KEY_H;
        case kVK_ANSI_I: return input::KeyboardKey::KEY_I;
        case kVK_ANSI_J: return input::KeyboardKey::KEY_J;
        case kVK_ANSI_K: return input::KeyboardKey::KEY_K;
        case kVK_ANSI_L: return input::KeyboardKey::KEY_L;
        case kVK_ANSI_M: return input::KeyboardKey::KEY_M;
        case kVK_ANSI_N: return input::KeyboardKey::KEY_N;
        case kVK_ANSI_O: return input::KeyboardKey::KEY_O;
        case kVK_ANSI_P: return input::KeyboardKey::KEY_P;
        case kVK_ANSI_Q: return input::KeyboardKey::KEY_Q;
        case kVK_ANSI_R: return input::KeyboardKey::KEY_R;
        case kVK_ANSI_S: return input::KeyboardKey::KEY_S;
        case kVK_ANSI_T: return input::KeyboardKey::KEY_T;
        case kVK_ANSI_U: return input::KeyboardKey::KEY_U;
        case kVK_ANSI_V: return input::KeyboardKey::KEY_V;
        case kVK_ANSI_W: return input::KeyboardKey::KEY_W;
        case kVK_ANSI_X: return input::KeyboardKey::KEY_X;
        case kVK_ANSI_Y: return input::KeyboardKey::KEY_Y;
        case kVK_ANSI_Z: return input::KeyboardKey::KEY_Z;

        case kVK_ANSI_0: return input::KeyboardKey::KEY_0;
        case kVK_ANSI_1: return input::KeyboardKey::KEY_1;
        case kVK_ANSI_2: return input::KeyboardKey::KEY_2;
        case kVK_ANSI_3: return input::KeyboardKey::KEY_3;
        case kVK_ANSI_4: return input::KeyboardKey::KEY_4;
        case kVK_ANSI_5: return input::KeyboardKey::KEY_5;
        case kVK_ANSI_6: return input::KeyboardKey::KEY_6;
        case kVK_ANSI_7: return input::KeyboardKey::KEY_7;
        case kVK_ANSI_8: return input::KeyboardKey::KEY_8;
        case kVK_ANSI_9: return input::KeyboardKey::KEY_9;

        case kVK_ANSI_Slash: return input::KeyboardKey::DIVIDE;
        case kVK_ANSI_Comma: return input::KeyboardKey::COMMA;
        case kVK_ANSI_Period: return input::KeyboardKey::PERIOD;
        case kVK_PageUp: return input::KeyboardKey::PRIOR;
        case kVK_PageDown: return input::KeyboardKey::NEXT;

        case kVK_ANSI_Keypad0: return input::KeyboardKey::NUMPAD0;
        case kVK_ANSI_Keypad1: return input::KeyboardKey::NUMPAD1;
        case kVK_ANSI_Keypad2: return input::KeyboardKey::NUMPAD2;
        case kVK_ANSI_Keypad3: return input::KeyboardKey::NUMPAD3;
        case kVK_ANSI_Keypad4: return input::KeyboardKey::NUMPAD4;
        case kVK_ANSI_Keypad5: return input::KeyboardKey::NUMPAD5;
        case kVK_ANSI_Keypad6: return input::KeyboardKey::NUMPAD6;
        case kVK_ANSI_Keypad7: return input::KeyboardKey::NUMPAD7;
        case kVK_ANSI_Keypad8: return input::KeyboardKey::NUMPAD8;
        case kVK_ANSI_Keypad9: return input::KeyboardKey::NUMPAD9;

        case kVK_ANSI_KeypadDecimal: return input::KeyboardKey::DECIMAL;
        case kVK_ANSI_KeypadMultiply: return input::KeyboardKey::MULTIPLY;
        case kVK_ANSI_KeypadPlus: return input::KeyboardKey::PLUS;
        case kVK_ANSI_KeypadClear: return input::KeyboardKey::OEM_CLEAR;
        case kVK_ANSI_KeypadDivide: return input::KeyboardKey::DIVIDE;
        case kVK_ANSI_KeypadEnter: return input::KeyboardKey::RETURN;
        case kVK_ANSI_KeypadMinus: return input::KeyboardKey::SUBTRACT;
        default: return input::KeyboardKey::NONE;
    }
}

-(void)keyDown:(NSEvent*)event
{
    if (!_running) return;

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->keyDown(convertKeyCode(event.keyCode), getModifiers(event));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)keyUp:(NSEvent*)event
{
    if (!_running) return;

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->keyUp(convertKeyCode(event.keyCode), getModifiers(event));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)mouseDown:(NSEvent*)event
{
    if (!_running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseDown(input::MouseButton::LEFT,
                                                 sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                    static_cast<float>(location.y))),
                                                 getModifiers(event));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)mouseUp:(NSEvent*)event
{
    if (!_running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseUp(input::MouseButton::LEFT,
                                               sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                  static_cast<float>(location.y))),
                                               getModifiers(event));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)rightMouseDown:(NSEvent*)event
{
    if (!_running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseDown(input::MouseButton::RIGHT,
                                                 sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                    static_cast<float>(location.y))),
                                                 getModifiers(event));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)rightMouseUp:(NSEvent*)event
{
    if (!_running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseUp(input::MouseButton::RIGHT,
                                               sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                  static_cast<float>(location.y))),
                                               getModifiers(event));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)otherMouseDown:(NSEvent*)event
{
    if (!_running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseDown(input::MouseButton::MIDDLE,
                                                 sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                    static_cast<float>(location.y))),
                                                 getModifiers(event));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)otherMouseUp:(NSEvent*)event
{
    if (!_running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseUp(input::MouseButton::MIDDLE,
                                               sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                  static_cast<float>(location.y))),
                                               getModifiers(event));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)mouseMoved:(NSEvent*)event
{
    if (!_running) return;

    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                    static_cast<float>(location.y))),
                                                 getModifiers(event));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)mouseDragged:(NSEvent*)event
{
    if (!_running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                    static_cast<float>(location.y))),
                                                 getModifiers(event, true));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)rightMouseDragged:(NSEvent*)event
{
    if (!_running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                    static_cast<float>(location.y))),
                                                 getModifiers(event, true));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)otherMouseDragged:(NSEvent*)event
{
    if (!_running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                    static_cast<float>(location.y))),
                                                 getModifiers(event, true));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)scrollWheel:(NSEvent*)event
{
    if (!_running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([_openGLContext CGLContextObj]);
    [_openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseScroll(Vector2(static_cast<float>(event.scrollingDeltaX),
                                                           static_cast<float>(event.scrollingDeltaY)),
                                                   sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                      static_cast<float>(location.y))),
                                                   getModifiers(event));
    CGLUnlockContext([_openGLContext CGLContextObj]);
}

-(void)swipeWithEvent:(NSEvent*)event
{
    OUZEL_UNUSED(event);
}

-(void)rotateWithEvent:(NSEvent*)event
{
    OUZEL_UNUSED(event);
}

-(void)magnifyWithEvent:(NSEvent*)event
{
    OUZEL_UNUSED(event);
}

@end
