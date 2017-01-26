// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Samples.h"
#include "MainMenu.h"
#include "SpritesSample.h"
#include "GUISample.h"
#include "RTSample.h"
#include "AnimationsSample.h"
#include "InputSample.h"
#include "SoundSample.h"
#include "PerspectiveSample.h"

using namespace std;
using namespace ouzel;

void Samples::begin(const std::string& sample)
{
    sharedEngine->getInput()->startGamepadDiscovery();

    sharedApplication->getFileSystem()->addResourcePath("Resources");

    sharedEngine->getRenderer()->setClearColor(Color(64, 0, 0));
    sharedEngine->getWindow()->setTitle("Samples");

    if (!sample.empty())
    {
        if (sample == "sprites")
        {
            currentScene.reset(new SpritesSample(*this));
        }
        else if (sample == "gui")
        {
            currentScene.reset(new GUISample(*this));
        }
        else if (sample == "render_target")
        {
            currentScene.reset(new RTSample(*this));
        }
        else if (sample == "animations")
        {
            currentScene.reset(new AnimationsSample(*this));
        }
        else if (sample == "input")
        {
            currentScene.reset(new InputSample(*this));
        }
        else if (sample == "sound")
        {
            currentScene.reset(new SoundSample(*this));
        }
        else if (sample == "perspective")
        {
            currentScene.reset(new PerspectiveSample(*this));
        }
    }

    if (!currentScene)
    {
        currentScene.reset(new MainMenu(*this));
    }

    sharedEngine->getSceneManager()->setScene(currentScene.get());
}

void Samples::setScene(std::unique_ptr<ouzel::scene::Scene>&& scene)
{
    currentScene = std::forward<std::unique_ptr<scene::Scene>>(scene);

    if (currentScene)
    {
        sharedEngine->getSceneManager()->setScene(currentScene.get());
    }
}
