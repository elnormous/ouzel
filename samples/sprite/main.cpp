// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Application.h"

ouzel::Application* application;

void OuzelInit(ouzel::Settings& settings)
{
    settings.size = ouzel::Size2(640.0f, 480.0f);
}

void OuzelBegin(ouzel::Engine* engine)
{
    application = new ouzel::Application(engine);
}

void OuzelEnd()
{
    delete application;
}
