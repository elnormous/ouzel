// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GUISample.hpp"
#include "MainMenu.hpp"

using namespace std;
using namespace ouzel;

GUISample::GUISample():
    button("button.png", "button_selected.png", "button_down.png", "", "Button", "arial.fnt", 1.0f, Color::RED, Color::RED, Color::BLACK),
    fullscreenButton("button.png", "button_selected.png", "button_down.png", "", "Fullscreen", "arial.fnt", 1.0f, Color::BLACK, Color::BLACK, Color::BLACK),
    checkBox("checkbox.png", "", "", "", "tick.png"),
    label1("checkbox", "AmosisTechnik.ttf", 24.0f),
    label2(sharedEngine->getLocalization()->getString("Ouzel"), "ArialBlack.fnt"),
    label3("UTF-8 ĀāČč\nNew line", "ArialBlack.fnt", 1.0f, Color::WHITE, Vector2(0.0f, 0.5f)),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", 1.0f, Color::BLACK, Color::BLACK, Color::BLACK)
{
    eventHandler.gamepadHandler = bind(&GUISample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&GUISample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&GUISample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2(800.0f, 600.0f));

    guiLayer.addChild(&guiCamera);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    button.setPosition(Vector2(-200.0f, 100.0f));
    menu.addWidget(&button);

    checkBox.setPosition(Vector2(-100.0f, 100.0f));
    guiLayer.addChild(&checkBox);

    fullscreenButton.setPosition(Vector2(-200.0f, 40.0f));
    menu.addWidget(&fullscreenButton);

    label1.getLabelDrawable()->setColor(Color::CYAN);
    label1.setPosition(Vector2(-28.0f, 84.0f));
    guiLayer.addChild(&label1);

    sharedEngine->getLocalization()->addLanguage("latvian", "lv.mo");
    sharedEngine->getLocalization()->setLanguage("latvian");

    label2.setPosition(Vector2(10.0f, 0.0f));
    guiLayer.addChild(&label2);

    label3.getLabelDrawable()->setColor(Color::BLUE);
    label3.setPosition(Vector2(-100.0f, -100.0f));
    label3.setScale(Vector3(0.5f, 0.5f, 1.0f));
    guiLayer.addChild(&label3);

    backButton.setPosition(Vector2(-200.0f, -200.0f));
    menu.addWidget(&backButton);
}

bool GUISample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == input::GamepadButton::FACE_RIGHT)
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        }
    }

    return true;
}

bool GUISample::handleUI(Event::Type type, const UIEvent& event)
{
    if (type == Event::Type::ACTOR_CLICK)
    {
        if (event.actor == &backButton)
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        }
        else if (event.actor == &button)
        {
            checkBox.setChecked(!checkBox.isChecked());
        }
        else if (event.actor == &fullscreenButton)
        {
            bool fullscreen = ouzel::sharedEngine->getWindow()->isFullscreen();
            ouzel::sharedEngine->getWindow()->setFullscreen(!fullscreen);
        }
    }

    return true;
}

bool GUISample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
{
    if (type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case input::KeyboardKey::ESCAPE:
            case input::KeyboardKey::MENU:
                sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                break;
            default:
                break;
        }
    }

    return true;
}
