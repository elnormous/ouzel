// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Samples.h"
#include "MainMenu.h"
#include "SpritesSample.h"
#include "GUISample.h"
#include "RTSample.h"
#include "AnimationsSample.h"
#include "InputSample.h"
#include "SoundSample.h"

using namespace std;
using namespace ouzel;

void Samples::begin(const std::string& sample)
{
    sharedEngine->getInput()->startGamepadDiscovery();

#if OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
    sharedApplication->getFileSystem()->addResourcePath("Resources");
#elif OUZEL_PLATFORM_WINDOWS
    sharedApplication->getFileSystem()->addResourcePath("Resources");
    sharedApplication->getFileSystem()->addResourcePath("../Resources");
    sharedApplication->getFileSystem()->addResourcePath("../../Resources");
#endif

    sharedEngine->getRenderer()->setClearColor(graphics::Color(64, 0, 0));
    sharedEngine->getWindow()->setTitle("Samples");

    scene::ScenePtr newScene;

    if (!sample.empty())
    {
        if (sample == "sprites")
        {
            newScene = make_shared<SpritesSample>();
        }
        else if (sample == "gui")
        {
            newScene = make_shared<GUISample>();
        }
        else if (sample == "render_target")
        {
            newScene = make_shared<RTSample>();
        }
        else if (sample == "animations")
        {
            newScene = make_shared<AnimationsSample>();
        }
        else if (sample == "input")
        {
            newScene = make_shared<InputSample>();
        }
        else if (sample == "sound")
        {
            newScene = make_shared<SoundSample>();
        }
    }

    if (!newScene)
    {
        newScene = make_shared<MainMenu>();
    }

    sharedEngine->getSceneManager()->setScene(newScene);
}
