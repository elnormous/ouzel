// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Application.h"

ouzel::SharedPtr<ouzel::Application> application;

void ouzelInit(ouzel::Settings& settings)
{
    settings.size = ouzel::Size2(640.0f, 480.0f);
    settings.resizable = true;
}

void ouzelBegin()
{
    application = new ouzel::Application();
}

void ouzelEnd()
{
    
}
