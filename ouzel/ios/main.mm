// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
#include <vector>
#include "core/Engine.h"
#include "utils/Utils.h"

int main(int argc, char* argv[])
{
    ouzel::setArgs(argc, argv);

    @autoreleasepool
    {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
