// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "OpenGLView.h"
#include "Engine.h"
#include "RendererOGL.h"

using namespace ouzel;

@implementation OpenGLView

-(id)initWithFrame:(NSRect)frameRect engine:(ouzel::Engine*)engine
{
    self = [super initWithFrame:frameRect];
    if (self != nil)
    {
        _engine = engine;
        _renderer = _engine->getRenderer();
        
        NSTimer *updateTimer = [NSTimer timerWithTimeInterval:1.0f/60.0f target:self selector:@selector(idle:) userInfo:nil repeats:YES];
        [[NSRunLoop currentRunLoop] addTimer:updateTimer forMode:NSDefaultRunLoopMode];
        
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
        
        _pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        // Create OpenGL context
        _openGLContext = [[NSOpenGLContext alloc] initWithFormat:_pixelFormat shareContext:NULL];
        [_openGLContext setView:self];
        [_openGLContext makeCurrentContext];
    }
    
    return self;
}

-(void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];
    
    [self update];
}

-(void)update
{
    _renderer->resize(Size2(_frame.size.width, _frame.size.height));
    
    [_openGLContext update];
}

-(void)lockFocus
{
    [super lockFocus];
    if ([_openGLContext view] != self)
    {
        [_openGLContext setView:self];
    }
    
    [self makeOpenGLContextCurrent];
}

-(void)prepareOpenGL
{
    RendererOGL* renderer = static_cast<RendererOGL*>(_renderer);
    renderer->initOpenGL(_frame.size.width, _frame.size.height);
}

-(void)idle:(NSTimer*)timer
{
    [self setNeedsDisplay:YES];
}

-(void)drawRect:(NSRect)bounds
{
    [_openGLContext makeCurrentContext];
    
    _engine->run();
    
    [_openGLContext flushBuffer];
}

-(void)setOpenGLContext:(NSOpenGLContext*)context
{
    _openGLContext = context;
}
-(NSOpenGLContext*)openGLContext
{
    return _openGLContext;
}
-(void)clearGLContext
{
    [_openGLContext clearDrawable];
}

-(void)setPixelFormat:(NSOpenGLPixelFormat*)pixelFormat
{
    _pixelFormat = pixelFormat;
}
-(NSOpenGLPixelFormat*)pixelFormat
{
    return _pixelFormat;
}

-(BOOL)acceptsFirstResponder
{
    return YES;
}

-(void)makeOpenGLContextCurrent
{
    [_openGLContext makeCurrentContext];
}

void updateModifiers(NSEvent* theEvent, Event& event)
{
    event.keyboardEvent.shiftDown = theEvent.modifierFlags & NSShiftKeyMask;
    event.keyboardEvent.controlDown = theEvent.modifierFlags & NSControlKeyMask;
    event.keyboardEvent.commandDown = theEvent.modifierFlags & NSCommandKeyMask;
    event.keyboardEvent.functionDown = theEvent.modifierFlags & NSFunctionKeyMask;
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

KeyboardKey convertKeyCode(unsigned short keyCode)
{
    switch (keyCode)
    {
        case kVK_UpArrow: return KeyboardKey::UP;
        case kVK_DownArrow: return KeyboardKey::DOWN;
        case kVK_LeftArrow: return KeyboardKey::LEFT;
        case kVK_RightArrow: return KeyboardKey::RIGHT;
        case kVK_F1: return KeyboardKey::F1;
        case kVK_F2: return KeyboardKey::F2;
        case kVK_F3: return KeyboardKey::F3;
        case kVK_F4: return KeyboardKey::F4;
        case kVK_F5: return KeyboardKey::F5;
        case kVK_F6: return KeyboardKey::F6;
        case kVK_F7: return KeyboardKey::F7;
        case kVK_F8: return KeyboardKey::F8;
        case kVK_F9: return KeyboardKey::F9;
        case kVK_F10: return KeyboardKey::F10;
        case kVK_F11: return KeyboardKey::F11;
        case kVK_F12: return KeyboardKey::F12;
        case kVK_F13: return KeyboardKey::F13;
        case kVK_F14: return KeyboardKey::F14;
        case kVK_F15: return KeyboardKey::F15;
        case kVK_F16: return KeyboardKey::F16;
        case kVK_F17: return KeyboardKey::F17;
        case kVK_F18: return KeyboardKey::F18;
        case kVK_F19: return KeyboardKey::F19;
        case kVK_F20: return KeyboardKey::F20;
        case kVK_Home: return KeyboardKey::HOME;
        case kVK_End: return KeyboardKey::END;
        case NSInsertFunctionKey: return KeyboardKey::INSERT;
        case kVK_ForwardDelete: return KeyboardKey::DELETE;
        case kVK_Help: return KeyboardKey::HELP;
        case NSSelectFunctionKey: return KeyboardKey::SELECT;
        case NSPrintFunctionKey: return KeyboardKey::PRINT;
        case NSExecuteFunctionKey: return KeyboardKey::EXECUT;
        case NSPrintScreenFunctionKey: return KeyboardKey::SNAPSHOT;
        case NSPauseFunctionKey: return KeyboardKey::PAUSE;
        case NSScrollLockFunctionKey: return KeyboardKey::SCROLL;
        case kVK_Delete: return KeyboardKey::BACKSPACE;
        case kVK_Tab: return KeyboardKey::TAB;
        case kVK_Return: return KeyboardKey::RETURN;
        case kVK_Escape: return KeyboardKey::ESCAPE;
        case kVK_Control: return KeyboardKey::CONTROL;
        case kVK_RightControl: return KeyboardKey::RCONTROL;
        case kVK_Command: return KeyboardKey::MENU;
        case kVK_Shift: return KeyboardKey::SHIFT;
        case kVK_RightShift: return KeyboardKey::RSHIFT;
        case kVK_Space: return KeyboardKey::SPACE;
            
        case kVK_ANSI_A: return KeyboardKey::KEY_A;
        case kVK_ANSI_B: return KeyboardKey::KEY_B;
        case kVK_ANSI_C: return KeyboardKey::KEY_C;
        case kVK_ANSI_D: return KeyboardKey::KEY_D;
        case kVK_ANSI_E: return KeyboardKey::KEY_E;
        case kVK_ANSI_F: return KeyboardKey::KEY_F;
        case kVK_ANSI_G: return KeyboardKey::KEY_G;
        case kVK_ANSI_H: return KeyboardKey::KEY_H;
        case kVK_ANSI_I: return KeyboardKey::KEY_I;
        case kVK_ANSI_J: return KeyboardKey::KEY_J;
        case kVK_ANSI_K: return KeyboardKey::KEY_K;
        case kVK_ANSI_L: return KeyboardKey::KEY_L;
        case kVK_ANSI_M: return KeyboardKey::KEY_M;
        case kVK_ANSI_N: return KeyboardKey::KEY_N;
        case kVK_ANSI_O: return KeyboardKey::KEY_O;
        case kVK_ANSI_P: return KeyboardKey::KEY_P;
        case kVK_ANSI_Q: return KeyboardKey::KEY_Q;
        case kVK_ANSI_R: return KeyboardKey::KEY_R;
        case kVK_ANSI_S: return KeyboardKey::KEY_S;
        case kVK_ANSI_T: return KeyboardKey::KEY_T;
        case kVK_ANSI_U: return KeyboardKey::KEY_U;
        case kVK_ANSI_V: return KeyboardKey::KEY_V;
        case kVK_ANSI_W: return KeyboardKey::KEY_W;
        case kVK_ANSI_X: return KeyboardKey::KEY_X;
        case kVK_ANSI_Y: return KeyboardKey::KEY_Y;
        case kVK_ANSI_Z: return KeyboardKey::KEY_Z;
            
        case kVK_ANSI_0: return KeyboardKey::KEY_0;
        case kVK_ANSI_1: return KeyboardKey::KEY_1;
        case kVK_ANSI_2: return KeyboardKey::KEY_2;
        case kVK_ANSI_3: return KeyboardKey::KEY_3;
        case kVK_ANSI_4: return KeyboardKey::KEY_4;
        case kVK_ANSI_5: return KeyboardKey::KEY_5;
        case kVK_ANSI_6: return KeyboardKey::KEY_6;
        case kVK_ANSI_7: return KeyboardKey::KEY_7;
        case kVK_ANSI_8: return KeyboardKey::KEY_8;
        case kVK_ANSI_9: return KeyboardKey::KEY_9;
            
        case kVK_ANSI_Slash: return KeyboardKey::DIVIDE;
        case kVK_ANSI_Comma: return KeyboardKey::COMMA;
        case kVK_ANSI_Period: return KeyboardKey::PERIOD;
        case kVK_PageUp: return KeyboardKey::PRIOR;
        case kVK_PageDown: return KeyboardKey::NEXT;
            
        case kVK_ANSI_Keypad0: return KeyboardKey::NUMPAD0;
        case kVK_ANSI_Keypad1: return KeyboardKey::NUMPAD1;
        case kVK_ANSI_Keypad2: return KeyboardKey::NUMPAD2;
        case kVK_ANSI_Keypad3: return KeyboardKey::NUMPAD3;
        case kVK_ANSI_Keypad4: return KeyboardKey::NUMPAD4;
        case kVK_ANSI_Keypad5: return KeyboardKey::NUMPAD5;
        case kVK_ANSI_Keypad6: return KeyboardKey::NUMPAD6;
        case kVK_ANSI_Keypad7: return KeyboardKey::NUMPAD7;
        case kVK_ANSI_Keypad8: return KeyboardKey::NUMPAD8;
        case kVK_ANSI_Keypad9: return KeyboardKey::NUMPAD9;
            
        case kVK_ANSI_KeypadDecimal: return KeyboardKey::DECIMAL;
        case kVK_ANSI_KeypadMultiply: return KeyboardKey::MULTIPLY;
        case kVK_ANSI_KeypadPlus: return KeyboardKey::PLUS;
        case kVK_ANSI_KeypadClear: return KeyboardKey::OEM_CLEAR;
        case kVK_ANSI_KeypadDivide: return KeyboardKey::DIVIDE;
        case kVK_ANSI_KeypadEnter: return KeyboardKey::RETURN;
        case kVK_ANSI_KeypadMinus: return KeyboardKey::SUBTRACT;
        default: return KeyboardKey::NONE;
    }
}

-(void)keyDown:(NSEvent*)theEvent
{
    Event event;
    event.type = Event::Type::KEY_DOWN;
    event.keyboardEvent.key = convertKeyCode(theEvent.keyCode);
    
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)keyUp:(NSEvent*)theEvent
{
    Event event;
    event.type = Event::Type::KEY_UP;
    event.keyboardEvent.key = convertKeyCode(theEvent.keyCode);
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)mouseDown:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DOWN;
    event.mouseEvent.button = MouseButton::LBUTTON;
    event.mouseEvent.position = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)mouseUp:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_UP;
    event.mouseEvent.button = MouseButton::LBUTTON;
    event.mouseEvent.position = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)rightMouseDown:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DOWN;
    event.mouseEvent.button = MouseButton::RBUTTON;
    event.mouseEvent.position = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)rightMouseUp:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_UP;
    event.mouseEvent.button = MouseButton::RBUTTON;
    event.mouseEvent.position = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)otherMouseDown:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DOWN;
    event.mouseEvent.button = MouseButton::MBUTTON;
    event.mouseEvent.position = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)otherMouseUp:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_UP;
    event.mouseEvent.button = MouseButton::MBUTTON;
    event.mouseEvent.position = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)mouseMoved:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_MOVE;
    event.mouseEvent.position = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)mouseDragged:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DRAG;
    event.mouseEvent.button = MouseButton::LBUTTON;
    event.mouseEvent.position = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)rightMouseDragged:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DRAG;
    event.mouseEvent.button = MouseButton::RBUTTON;
    event.mouseEvent.position = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)otherMouseDragged:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DRAG;
    event.mouseEvent.button = MouseButton::MBUTTON;
    event.mouseEvent.position = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)scrollWheel:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_SCROLL;
    event.mouseEvent.position = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    event.mouseEvent.scroll = Vector2(theEvent.scrollingDeltaX, theEvent.scrollingDeltaY);
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)swipeWithEvent:(NSEvent*)theEvent
{
    
}

-(void)rotateWithEvent:(NSEvent*)theEvent
{
    
}

-(void)magnifyWithEvent:(NSEvent*)theEvent
{
    
}

@end
