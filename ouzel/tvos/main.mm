// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
#include <vector>
#include "Engine.h"

int main(int argc, char *argv[])
{
    std::vector<std::string> args;
    
    for (int32_t i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }
    
    ouzel::Engine::getInstance()->setArgs(args);
    
    @autoreleasepool
    {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
