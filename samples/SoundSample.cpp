// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

SoundSample::SoundSample()
{
    eventHandler.gamepadHandler = bind(&SoundSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&SoundSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&SoundSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    test8BitSound.reset(new audio::Sound());
    test8BitSound->init(sharedEngine->getCache()->getSoundData("8-bit.wav"));

    test24BitSound.reset(new audio::Sound());
    test24BitSound->init(sharedEngine->getCache()->getSoundData("24-bit.wav"));

    jumpSound.reset(new audio::Sound());
    jumpSound->init(sharedEngine->getCache()->getSoundData("jump.wav"));

    ambientSound.reset(new audio::Sound());
    ambientSound->init(sharedEngine->getCache()->getSoundData("ambient.wav"));

    music.reset(new audio::Sound());
    music->init(sharedEngine->getCache()->getSoundData("music.ogg"));

    guiCamera.reset(new scene::Camera());
    guiCamera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera->setTargetContentSize(Size2(800.0f, 600.0f));

    guiLayer.reset(new scene::Layer());
    guiLayer->addChild(guiCamera.get());
    addLayer(guiLayer.get());

    menu.reset(new gui::Menu());
    guiLayer->addChild(menu.get());

    test8BitButton.reset(new ouzel::gui::Button("button.png", "button_selected.png", "button_down.png", "", "8-bit", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK));
    test8BitButton->setPosition(Vector2(0.0f, 80.0f));
    menu->addWidget(test8BitButton.get());

    test24BitButton.reset(new ouzel::gui::Button("button.png", "button_selected.png", "button_down.png", "", "24-bit", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK));
    test24BitButton->setPosition(Vector2(0.0f, 40.0f));
    menu->addWidget(test24BitButton.get());

    jumpButton.reset(new ouzel::gui::Button("button.png", "button_selected.png", "button_down.png", "", "Jump", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK));
    jumpButton->setPosition(Vector2(0.0f, 0.0f));
    menu->addWidget(jumpButton.get());

    ambientButton.reset(new ouzel::gui::Button("button.png", "button_selected.png", "button_down.png", "", "Ambient", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK));
    ambientButton->setPosition(Vector2(0.0f, -40.0f));
    menu->addWidget(ambientButton.get());

    musicButton.reset(new ouzel::gui::Button("button.png", "button_selected.png", "button_down.png", "", "Music", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK));
    musicButton->setPosition(Vector2(0.0f, -80.0f));
    menu->addWidget(musicButton.get());

    backButton.reset(new ouzel::gui::Button("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK));
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton.get());
}

bool SoundSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == input::GamepadButton::FACE2)
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        }
    }

    return true;
}

bool SoundSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::CLICK_NODE)
    {
        if (event.node == backButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        }
        else if (event.node == test8BitButton.get())
        {
            test8BitSound->play();
        }
        else if (event.node == test24BitButton.get())
        {
            test24BitSound->play();
        }
        else if (event.node == jumpButton.get())
        {
            jumpSound->play();
        }
        else if (event.node == ambientButton.get())
        {
            ambientSound->play();
        }
        else if (event.node == musicButton.get())
        {
            music->play();
        }
    }

    return true;
}

bool SoundSample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
{
    if (type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case input::KeyboardKey::ESCAPE:
                sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                break;
            default:
                break;
        }
    }

    return true;
}
