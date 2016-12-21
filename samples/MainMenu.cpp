// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

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

MainMenu::MainMenu(Samples& aSamples):
    samples(aSamples),
    spritesButton("button.png", "button_selected.png", "button_down.png", "", "Sprites", Color(20, 0, 0, 255), "arial.fnt"),
    guiButton("button.png", "button_selected.png", "button_down.png", "", "GUI", Color(20, 0, 0, 255), "arial.fnt"),
    renderTargetButton("button.png", "button_selected.png", "button_down.png", "", "Render target", Color(20, 0, 0, 255), "arial.fnt"),
    animationsButton("button.png", "button_selected.png", "button_down.png", "", "Animations", Color(20, 0, 0, 255), "arial.fnt"),
    inputButton("button.png", "button_selected.png", "button_down.png", "", "Input", Color(20, 0, 0, 255), "arial.fnt"),
    soundButton("button.png", "button_selected.png", "button_down.png", "", "Sound", Color(20, 0, 0, 255), "arial.fnt"),
    perspectiveButton("button.png", "button_selected.png", "button_down.png", "", "Perspective", Color(20, 0, 0, 255), "arial.fnt")
{
    eventHandler.uiHandler = bind(&MainMenu::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&MainMenu::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    addLayer(&layer);

    camera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera.setTargetContentSize(Size2(400.0f, 600.0f));
    layer.addCamera(&camera);
    layer.addChild(&menu);

    spritesButton.setPosition(Vector2(0.0f, 80.0f));
    menu.addWidget(&spritesButton);

    guiButton.setPosition(Vector2(0.0f, 40.0f));
    menu.addWidget(&guiButton);

    renderTargetButton.setPosition(Vector2(0.0f, 0.0f));
    menu.addWidget(&renderTargetButton);

    animationsButton.setPosition(Vector2(0.0f, -40.0f));
    menu.addWidget(&animationsButton);

    inputButton.setPosition(Vector2(0.0f, -80.0f));
    menu.addWidget(&inputButton);

    soundButton.setPosition(Vector2(0.0f, -120.0f));
    menu.addWidget(&soundButton);

    perspectiveButton.setPosition(Vector2(0.0f, -160.0f));
    menu.addWidget(&perspectiveButton);
}

bool MainMenu::handleKeyboard(Event::Type type, const KeyboardEvent& event)
{
    if (event.key == ouzel::input::KeyboardKey::ESCAPE)
    {
        if (type == Event::Type::KEY_DOWN)
        {
            sharedEngine->exit();
        }

        return false;
    }

    return true;
}

bool MainMenu::handleUI(Event::Type type, const UIEvent& event)
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == &spritesButton)
        {
            samples.setScene(std::unique_ptr<scene::Scene>(new SpritesSample(samples)));
        }
        else if (event.node == &guiButton)
        {
            samples.setScene(std::unique_ptr<scene::Scene>(new GUISample(samples)));
        }
        else if (event.node == &renderTargetButton)
        {
            samples.setScene(std::unique_ptr<scene::Scene>(new RTSample(samples)));
        }
        else if (event.node == &animationsButton)
        {
            samples.setScene(std::unique_ptr<scene::Scene>(new AnimationsSample(samples)));
        }
        else if (event.node == &inputButton)
        {
            samples.setScene(std::unique_ptr<scene::Scene>(new InputSample(samples)));
        }
        else if (event.node == &soundButton)
        {
            samples.setScene(std::unique_ptr<scene::Scene>(new SoundSample(samples)));
        }
        else if (event.node == &perspectiveButton)
        {
            samples.setScene(std::unique_ptr<scene::Scene>(new PerspectiveSample(samples)));
        }
    }

    return true;
}
