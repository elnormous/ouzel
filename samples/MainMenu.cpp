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

using namespace std;
using namespace ouzel;

MainMenu::MainMenu():
    gitHubButton("button.png", "button_selected.png", "button_down.png", "", "GitHub", "arial.fnt", 1.0F, Color(20, 0, 0, 255), Color::BLACK, Color::BLACK),
    spritesButton("button.png", "button_selected.png", "button_down.png", "", "Sprites", "arial.fnt", 1.0F,Color(20, 0, 0, 255), Color::BLACK, Color::BLACK),
    guiButton("button.png", "button_selected.png", "button_down.png", "", "GUI", "arial.fnt", 1.0F, Color(20, 0, 0, 255), Color::BLACK, Color::BLACK),
    renderTargetButton("button.png", "button_selected.png", "button_down.png", "", "Render target", "arial.fnt", 1.0F, Color(20, 0, 0, 255), Color::BLACK, Color::BLACK),
    animationsButton("button.png", "button_selected.png", "button_down.png", "", "Animations", "arial.fnt", 1.0F, Color(20, 0, 0, 255), Color::BLACK, Color::BLACK),
    inputButton("button.png", "button_selected.png", "button_down.png", "", "Input", "arial.fnt", 1.0F, Color(20, 0, 0, 255), Color::BLACK, Color::BLACK),
    soundButton("button.png", "button_selected.png", "button_down.png", "", "Sound", "arial.fnt", 1.0F, Color(20, 0, 0, 255), Color::BLACK, Color::BLACK),
    perspectiveButton("button.png", "button_selected.png", "button_down.png", "", "Perspective", "arial.fnt", 1.0F, Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)
{
    handler.uiHandler = bind(&MainMenu::handleUI, this, placeholders::_1, placeholders::_2);
    handler.keyboardHandler = bind(&MainMenu::handleKeyboard, this, placeholders::_1, placeholders::_2);
    engine->getEventDispatcher()->addEventHandler(&handler);

    addLayer(&layer);

    camera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera.setTargetContentSize(Size2(400.0F, 600.0F));
    cameraActor.addComponent(&camera);
    layer.addChild(&cameraActor);

    layer.addChild(&menu);

    gitHubButton.setPosition(Vector2(0.0F, 120.0F));
    menu.addWidget(&gitHubButton);

    spritesButton.setPosition(Vector2(0.0F, 80.0F));
    menu.addWidget(&spritesButton);

    guiButton.setPosition(Vector2(0.0F, 40.0F));
    menu.addWidget(&guiButton);

    renderTargetButton.setPosition(Vector2(0.0F, 0.0F));
    menu.addWidget(&renderTargetButton);

    animationsButton.setPosition(Vector2(0.0F, -40.0F));
    menu.addWidget(&animationsButton);

    inputButton.setPosition(Vector2(0.0F, -80.0F));
    menu.addWidget(&inputButton);

    soundButton.setPosition(Vector2(0.0F, -120.0F));
    menu.addWidget(&soundButton);

    perspectiveButton.setPosition(Vector2(0.0F, -160.0F));
    menu.addWidget(&perspectiveButton);
}

bool MainMenu::handleKeyboard(Event::Type type, const KeyboardEvent& event)
{
    if (event.key == ouzel::input::KeyboardKey::ESCAPE)
    {
        if (type == Event::Type::KEY_PRESS)
        {
            engine->exit();
        }

        return false;
    }

    return true;
}

bool MainMenu::handleUI(Event::Type type, const UIEvent& event)
{
    if (type == Event::Type::ACTOR_CLICK)
    {
        if (event.actor == &gitHubButton)
            engine->openURL("https://github.com/elnormous/ouzel");
        else if (event.actor == &spritesButton)
            engine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new SpritesSample()));
        else if (event.actor == &guiButton)
            engine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new GUISample()));
        else if (event.actor == &renderTargetButton)
            engine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new RTSample()));
        else if (event.actor == &animationsButton)
            engine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new AnimationsSample()));
        else if (event.actor == &inputButton)
            engine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new InputSample()));
        else if (event.actor == &soundButton)
            engine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new SoundSample()));
        else if (event.actor == &perspectiveButton)
            engine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new PerspectiveSample()));
    }

    return true;
}
