// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "MainMenu.hpp"
#include "SpritesSample.hpp"
#include "GUISample.hpp"
#include "RTSample.hpp"
#include "AnimationsSample.hpp"
#include "InputSample.hpp"
#include "SoundSample.hpp"
#include "PerspectiveSample.hpp"

using namespace ouzel;
using namespace input;

MainMenu::MainMenu():
    gitHubButton("button.png", "button_selected.png", "button_down.png", "", "GitHub", "Arial", 1.0F, Color(20, 0, 0, 255), Color::black(), Color::black()),
    spritesButton("button.png", "button_selected.png", "button_down.png", "", "Sprites", "Arial", 1.0F,Color(20, 0, 0, 255), Color::black(), Color::black()),
    guiButton("button.png", "button_selected.png", "button_down.png", "", "GUI", "Arial", 1.0F, Color(20, 0, 0, 255), Color::black(), Color::black()),
    renderTargetButton("button.png", "button_selected.png", "button_down.png", "", "Render target", "Arial", 1.0F, Color(20, 0, 0, 255), Color::black(), Color::black()),
    animationsButton("button.png", "button_selected.png", "button_down.png", "", "Animations", "Arial", 1.0F, Color(20, 0, 0, 255), Color::black(), Color::black()),
    inputButton("button.png", "button_selected.png", "button_down.png", "", "Input", "Arial", 1.0F, Color(20, 0, 0, 255), Color::black(), Color::black()),
    soundButton("button.png", "button_selected.png", "button_down.png", "", "Sound", "Arial", 1.0F, Color(20, 0, 0, 255), Color::black(), Color::black()),
    perspectiveButton("button.png", "button_selected.png", "button_down.png", "", "Perspective", "Arial", 1.0F, Color(20, 0, 0, 255), Color::black(), Color::black())
{
    handler.uiHandler = std::bind(&MainMenu::handleUI, this, std::placeholders::_1);
    handler.keyboardHandler = std::bind(&MainMenu::handleKeyboard, this, std::placeholders::_1);
    engine->getEventDispatcher().addEventHandler(handler);

    addLayer(&layer);

    camera.setClearColorBuffer(true);
    camera.setClearColor(ouzel::Color(64, 0, 0));
    camera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera.setTargetContentSize(Size2F(400.0F, 600.0F));
    cameraActor.addComponent(&camera);
    layer.addChild(&cameraActor);

    layer.addChild(&menu);

    gitHubButton.setPosition(Vector2F(0.0F, 120.0F));
    menu.addWidget(&gitHubButton);

    spritesButton.setPosition(Vector2F(0.0F, 80.0F));
    menu.addWidget(&spritesButton);

    guiButton.setPosition(Vector2F(0.0F, 40.0F));
    menu.addWidget(&guiButton);

    renderTargetButton.setPosition(Vector2F(0.0F, 0.0F));
    menu.addWidget(&renderTargetButton);

    animationsButton.setPosition(Vector2F(0.0F, -40.0F));
    menu.addWidget(&animationsButton);

    inputButton.setPosition(Vector2F(0.0F, -80.0F));
    menu.addWidget(&inputButton);

    soundButton.setPosition(Vector2F(0.0F, -120.0F));
    menu.addWidget(&soundButton);

    perspectiveButton.setPosition(Vector2F(0.0F, -160.0F));
    menu.addWidget(&perspectiveButton);
}

bool MainMenu::handleKeyboard(const KeyboardEvent& event)
{
    if (event.type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
                engine->exit();
                break;
            case Keyboard::Key::MENU:
            case Keyboard::Key::BACK:
                return false;
            default:
                break;
        }
    }
    else if (event.type == Event::Type::KEY_RELEASE)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
            case Keyboard::Key::BACK:
                return false;
            default:
                break;
        }
    }

    return true;
}

bool MainMenu::handleUI(const UIEvent& event)
{
    if (event.type == Event::Type::ACTOR_CLICK)
    {
        if (event.actor == &gitHubButton)
            engine->openURL("https://github.com/elnormous/ouzel");
        else if (event.actor == &spritesButton)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new SpritesSample()));
        else if (event.actor == &guiButton)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new GUISample()));
        else if (event.actor == &renderTargetButton)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new RTSample()));
        else if (event.actor == &animationsButton)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new AnimationsSample()));
        else if (event.actor == &inputButton)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new InputSample()));
        else if (event.actor == &soundButton)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new SoundSample()));
        else if (event.actor == &perspectiveButton)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new PerspectiveSample()));
    }

    return false;
}
