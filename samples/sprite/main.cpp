// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Application.h"

std::shared_ptr<ouzel::App> ouzelMain()
{
    std::shared_ptr<ouzel::Application> application(new ouzel::Application());
    
    return application;
}
