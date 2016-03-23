// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#include <vector>
#include "Engine.h"
#include "Utils.h"

int main(int argc, const char* argv[])
{
    std::vector<std::string> args;

    for (int32_t i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }

    ouzel::setArgs(args);

    @autoreleasepool
    {
        NSApplication* application = [NSApplication sharedApplication];

        AppDelegate* appDelegate = [[AppDelegate alloc] init];
        [application setDelegate:appDelegate];
        [application run];
    }

    return 0;
}
