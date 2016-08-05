// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Samples.h"

ouzel::Engine engine;
Samples samples;

void ouzelMain(const std::vector<std::string>& args)
{
    std::string sample;

    if (args.size() >= 2)
    {
        sample = args[1];
    }

    ouzel::Settings settings;
    settings.size = ouzel::Size2(800.0f, 600.0f);
    settings.resizable = true;
    settings.sampleCount = 4;
    settings.textureFiltering = ouzel::graphics::Renderer::TextureFiltering::TRILINEAR;
    engine.init(settings, std::bind(&Samples::begin, &samples, sample));
}
