// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MainMenu.h"
#include "SpritesSample.h"
#include "GUISample.h"
#include "RTSample.h"
#include "AnimationsSample.h"
#include "InputSample.h"
#include "SoundSample.h"
#include "PerspectiveSample.h"

ouzel::Engine engine;

void ouzelMain(const std::vector<std::string>& args)
{
    // disable screen saver
    ouzel::sharedApplication->setScreenSaverEnabled(false);

    ouzel::Settings settings;
    settings.size = ouzel::Size2(800.0f, 600.0f);
    settings.resizable = true;
    settings.sampleCount = 4;
    settings.textureFilter = ouzel::graphics::Texture::Filter::TRILINEAR;
    settings.depth = true;
    settings.title = "Samples";

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
                ouzel::Log(ouzel::Log::Level::WARN) << "No sample specified";
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
                else
                {
                    ouzel::Log(ouzel::Log::Level::WARN) << "Invalid renderer specified";
                }
            }
            else
            {
                ouzel::Log(ouzel::Log::Level::WARN) << "No renderer specified";
            }
        }
        else
        {
            ouzel::Log(ouzel::Log::Level::WARN) << "Invalid argument \"" << *arg << "\"";
        }
    }

    if (engine.init(settings))
    {
        ouzel::sharedEngine->getInput()->startGamepadDiscovery();

        ouzel::sharedApplication->getFileSystem()->addResourcePath("Resources");

        ouzel::sharedEngine->getRenderer()->setClearColor(ouzel::Color(64, 0, 0));

        std::unique_ptr<ouzel::scene::Scene> currentScene;

        if (!sample.empty())
        {
            if (sample == "sprites")
            {
                currentScene.reset(new SpritesSample());
            }
            else if (sample == "gui")
            {
                currentScene.reset(new GUISample());
            }
            else if (sample == "render_target")
            {
                currentScene.reset(new RTSample());
            }
            else if (sample == "animations")
            {
                currentScene.reset(new AnimationsSample());
            }
            else if (sample == "input")
            {
                currentScene.reset(new InputSample());
            }
            else if (sample == "sound")
            {
                currentScene.reset(new SoundSample());
            }
            else if (sample == "perspective")
            {
                currentScene.reset(new PerspectiveSample());
            }
        }
        
        if (!currentScene)
        {
            currentScene.reset(new MainMenu());
        }
        
        ouzel::sharedEngine->getSceneManager()->setScene(std::move(currentScene));
    }
}
