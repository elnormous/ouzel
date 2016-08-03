// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ApplicationIOS.h"
#import <UIKit/UIKit.h>
#import "AppDelegate.h"

namespace ouzel
{
    ApplicationIOS::ApplicationIOS(int pArgc, char* pArgv[]):
        Application(pArgc, pArgv)
    {
    }

    bool ApplicationIOS::run()
    {
        @autoreleasepool
        {
            return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
        }

        return true;
    }
}