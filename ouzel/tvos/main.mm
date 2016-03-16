// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
#include <vector>
#include "Engine.h"
#include "Utils.h"

int main(int argc, char* argv[])
{
    std::vector<std::string> args;
    
    for (int32_t i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }
    
    ouzel::setArgs(args);
    
    @autoreleasepool
    {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
