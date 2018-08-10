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

ouzel::Archive archive;

void ouzelMain(const std::vector<std::string>& args)
{
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
    archive = ouzel::Archive("gui.zip");
    engine->getFileSystem()->addArchive(&archive);
#endif

    // GUI
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "arial.png");
    engine->getCache()->loadAsset(assets::Loader::FONT, "arial.fnt");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "ArialBlack.png");
    engine->getCache()->loadAsset(assets::Loader::FONT, "ArialBlack.fnt");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "button_down.png");
    engine->getCache()->preloadSpriteData("button_down.png");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "button_selected.png");
    engine->getCache()->preloadSpriteData("button_selected.png");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "button.png");
    engine->getCache()->preloadSpriteData("button.png");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "checkbox.png");
    engine->getCache()->preloadSpriteData("checkbox.png");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "edit_box.png");
    engine->getCache()->preloadSpriteData("edit_box.png");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "radio_button.png");
    engine->getCache()->preloadSpriteData("radio_button.png");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "tick.png");
    engine->getCache()->preloadSpriteData("tick.png");

    // assets
    engine->getCache()->loadAsset(assets::Loader::SOUND, "8-bit.wav");
    engine->getCache()->loadAsset(assets::Loader::SOUND, "24-bit.wav");
    engine->getCache()->loadAsset(assets::Loader::SOUND, "ambient.wav");
    engine->getCache()->loadAsset(assets::Loader::FONT, "AmosisTechnik.ttf");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "ball.png");
    engine->getCache()->preloadSpriteData("ball.png");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "cube.png");
    engine->getCache()->loadAsset(assets::Loader::MATERIAL, "cube.mtl");
    engine->getCache()->loadAsset(assets::Loader::MESH, "cube.obj");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "fire.png");
    engine->getCache()->loadAsset(assets::Loader::SPRITE, "fire.json");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "flame.png");
    engine->getCache()->loadAsset(assets::Loader::PARTICLE_SYSTEM, "flame.json");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "floor.jpg");
    engine->getCache()->preloadSpriteData("floor.jpg");
    engine->getCache()->loadAsset(assets::Loader::SOUND, "jump.wav");
    engine->getCache()->loadAsset(assets::Loader::SOUND, "music.ogg");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "run.png");
    engine->getCache()->loadAsset(assets::Loader::SPRITE, "run.json");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "triangle.png");
    engine->getCache()->loadAsset(assets::Loader::SPRITE, "triangle.json");
    engine->getCache()->loadAsset(assets::Loader::IMAGE, "witch.png");
    engine->getCache()->preloadSpriteData("witch.png");

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
