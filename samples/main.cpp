// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MainMenu.hpp"
#include "SpritesSample.hpp"
#include "GUISample.hpp"
#include "RTSample.hpp"
#include "AnimationsSample.hpp"
#include "InputSample.hpp"
#include "SoundSample.hpp"
#include "PerspectiveSample.hpp"

std::string DEVELOPER_NAME = "org.ouzel";
std::string APPLICATION_NAME = "samples";

using namespace ouzel;

#if !OUZEL_PLATFORM_ANDROID
std::unique_ptr<ouzel::Archive> archive;
#endif
std::unique_ptr<assets::Bundle> bundle;

void ouzelMain(const std::vector<std::string>& args)
{
    bundle.reset(new assets::Bundle(*engine->getCache()));

    // disable screen saver
    engine->setScreenSaverEnabled(false);

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
                sample = *nextArg;
            else
                ouzel::Log(ouzel::Log::Level::WARN) << "No sample specified";
        }
        else
            ouzel::Log(ouzel::Log::Level::WARN) << "Invalid argument \"" << *arg << "\"";
    }

    engine->getFileSystem()->addResourcePath("Resources");

#if !OUZEL_PLATFORM_ANDROID
    archive.reset(new ouzel::Archive(*engine->getFileSystem(), "gui.zip"));
#endif

    bundle->loadAssets("assets.json");

    engine->getRenderer()->setClearColor(ouzel::Color(64, 0, 0));

    std::unique_ptr<ouzel::scene::Scene> currentScene;

    if (!sample.empty())
    {
        if (sample == "sprites") currentScene.reset(new SpritesSample());
        else if (sample == "gui") currentScene.reset(new GUISample());
        else if (sample == "render_target") currentScene.reset(new RTSample());
        else if (sample == "animations") currentScene.reset(new AnimationsSample());
        else if (sample == "input") currentScene.reset(new InputSample());
        else if (sample == "sound") currentScene.reset(new SoundSample());
        else if (sample == "perspective") currentScene.reset(new PerspectiveSample());
    }

    if (!currentScene) currentScene.reset(new MainMenu());

    engine->getSceneManager()->setScene(std::move(currentScene));
}
