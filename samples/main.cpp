// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Samples.h"

ouzel::Engine engine;
Samples samples;

void ouzelMain(const std::vector<std::string>& args)
{
    ouzel::Settings settings;
    settings.size = ouzel::Size2(800.0f, 600.0f);
    settings.resizable = true;
    settings.sampleCount = 4;
    settings.textureFiltering = ouzel::graphics::Renderer::TextureFiltering::TRILINEAR;

    std::string sample;

    for (auto arg = args.begin(); arg != args.end(); ++arg)
    {
        if (arg == args.begin())
        {
            // skip the first parameter
            continue;
        }

        if (*arg == "-sample")
        {
            auto nextArg = ++arg;

            if (nextArg != args.end())
            {
                sample = *nextArg;
            }
            else
            {
                ouzel::log(ouzel::LOG_LEVEL_WARNING, "No sample specified");
            }
        }
        else if (*arg == "-renderer")
        {
            auto nextArg = ++arg;

            if (nextArg != args.end())
            {
                if (*nextArg == "empty")
                {
                    settings.renderDriver = ouzel::graphics::Renderer::Driver::EMPTY;
                }
                else if (*nextArg == "opengl")
                {
                    settings.renderDriver = ouzel::graphics::Renderer::Driver::OPENGL;
                }
                else if (*nextArg == "direct3d11")
                {
                    settings.renderDriver = ouzel::graphics::Renderer::Driver::DIRECT3D11;
                }
                else if (*nextArg == "metal")
                {
                    settings.renderDriver = ouzel::graphics::Renderer::Driver::METAL;
                }
            }
            else
            {
                ouzel::log(ouzel::LOG_LEVEL_WARNING, "No renderer specified");
            }
        }
        else
        {
            ouzel::log(ouzel::LOG_LEVEL_WARNING, "Invalid argument \"%s\"", arg->c_str());
        }
    }

    engine.init(settings);

    samples.begin(sample);
}
