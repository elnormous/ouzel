// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Cocoa/Cocoa.h>

@class OpenGLView;

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
    OpenGLView* _openGLView;
}

@property (strong, nonatomic) NSWindow* window;

@end
