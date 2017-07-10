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

std::string DEVELOPER_NAME = "org.ouzelengine";
std::string APPLICATION_NAME = "samples";

void ouzelMain(const std::vector<std::string>& args)
{
    // disable screen saver
    ouzel::sharedApplication->setScreenSaverEnabled(false);

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
        else
        {
            ouzel::Log(ouzel::Log::Level::WARN) << "Invalid argument \"" << *arg << "\"";
        }
    }

    ouzel::sharedEngine->getInput()->startGamepadDiscovery();

    ouzel::sharedEngine->getFileSystem()->addResourcePath("Resources");

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
