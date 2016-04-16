// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Application.h"

void ouzelMain(const std::vector<std::string>& args)
{
    OUZEL_UNUSED(args);

    ouzel::Settings settings;
    settings.size = ouzel::Size2(800.0f, 600.0f);
    settings.resizable = true;
    ouzel::sharedEngine->init(settings);

    std::shared_ptr<Application> application = std::make_shared<Application>();
    ouzel::sharedEngine->setApp(application);
}
