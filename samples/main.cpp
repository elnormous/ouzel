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
                ouzel::log("No sample specified");
            }
        }
        else if (*arg == "-renderer")
        {
            auto nextArg = ++arg;

            if (nextArg != args.end())
            {
                if (*nextArg == "opengl")
                {
                    settings.driver = ouzel::graphics::Renderer::Driver::OPENGL;
                }
                else if (*nextArg == "direct3d11")
                {
                    settings.driver = ouzel::graphics::Renderer::Driver::DIRECT3D11;
                }
                else if (*nextArg == "metal")
                {
                    settings.driver = ouzel::graphics::Renderer::Driver::METAL;
                }
            }
            else
            {
                ouzel::log("No renderer specified");
            }
        }
        else
        {
            ouzel::log("Invalid argument \"%s\"", arg->c_str());
        }
    }

    engine.init(settings, std::bind(&Samples::begin, &samples, sample));
}
