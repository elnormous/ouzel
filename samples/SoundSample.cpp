// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundSample.hpp"
#include "MainMenu.hpp"

using namespace std;
using namespace ouzel;

SoundSample::SoundSample():
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", 1.0f, Color::BLACK, Color::BLACK, Color::BLACK),
    test8BitButton("button.png", "button_selected.png", "button_down.png", "", "8-bit", "arial.fnt", 1.0f, Color::BLACK, Color::BLACK, Color::BLACK),
    test24BitButton("button.png", "button_selected.png", "button_down.png", "", "24-bit", "arial.fnt", 1.0f, Color::BLACK, Color::BLACK, Color::BLACK),
    jumpButton("button.png", "button_selected.png", "button_down.png", "", "Jump", "arial.fnt", 1.0f, Color::BLACK, Color::BLACK, Color::BLACK),
    ambientButton("button.png", "button_selected.png", "button_down.png", "", "Ambient", "arial.fnt", 1.0f, Color::BLACK, Color::BLACK, Color::BLACK),
    musicButton("button.png", "button_selected.png", "button_down.png", "", "Music", "arial.fnt", 1.0f, Color::BLACK, Color::BLACK, Color::BLACK)
{
    eventHandler.gamepadHandler = bind(&SoundSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&SoundSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&SoundSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    sharedEngine->getAudio()->addListener(&listener);

    soundMixer.setOutput(&listener);
    soundMixer.setGain(1.2f);

    test8BitSound.reset(new audio::Sound());
    test8BitSound->init(sharedEngine->getCache()->getSoundData("8-bit.wav"));
    test8BitSound->setPitch(2.0f);
    test8BitSound->setOutput(&soundMixer);

    test24BitSound.reset(new audio::Sound());
    test24BitSound->init(sharedEngine->getCache()->getSoundData("24-bit.wav"));
    test24BitSound->setPitch(0.5f);
    test24BitSound->setOutput(&soundMixer);

    jumpSound.reset(new audio::Sound());
    jumpSound->init(sharedEngine->getCache()->getSoundData("jump.wav"));
    jumpSound->setPosition(Vector3(8.0f, 0.0f, 10.0f));
    jumpSound->setOutput(&soundMixer);

    ambientSound.reset(new audio::Sound());
    ambientSound->init(sharedEngine->getCache()->getSoundData("ambient.wav"));
    ambientSound->setOutput(&soundMixer);

    music.reset(new audio::Sound());
    music->init(sharedEngine->getCache()->getSoundData("music.ogg"));
    music->setOutput(&listener);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2(800.0f, 600.0f));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    test8BitButton.setPosition(Vector2(0.0f, 80.0f));
    menu.addWidget(&test8BitButton);

    test24BitButton.setPosition(Vector2(0.0f, 40.0f));
    menu.addWidget(&test24BitButton);

    jumpButton.setPosition(Vector2(0.0f, 0.0f));
    menu.addWidget(&jumpButton);

    ambientButton.setPosition(Vector2(0.0f, -40.0f));
    menu.addWidget(&ambientButton);

    musicButton.setPosition(Vector2(0.0f, -80.0f));
    menu.addWidget(&musicButton);

    backButton.setPosition(Vector2(-200.0f, -200.0f));
    menu.addWidget(&backButton);
}

bool SoundSample::handleGamepad(Event::Type type, const GamepadEvent& event)
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

bool SoundSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::ACTOR_CLICK)
    {
        if (event.actor == &backButton)
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        }
        else if (event.actor == &test8BitButton)
        {
            test8BitSound->play();
        }
        else if (event.actor == &test24BitButton)
        {
            test24BitSound->play();
        }
        else if (event.actor == &jumpButton)
        {
            jumpSound->play(true);
        }
        else if (event.actor == &ambientButton)
        {
            ambientSound->play();
        }
        else if (event.actor == &musicButton)
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
            case input::KeyboardKey::MENU:
                sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                break;
            default:
                break;
        }
    }

    return true;
}
