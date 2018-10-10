// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundSample.hpp"
#include "MainMenu.hpp"

using namespace std;
using namespace ouzel;
using namespace audio;
using namespace input;

SoundSample::SoundSample():
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    test8BitButton("button.png", "button_selected.png", "button_down.png", "", "8-bit", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    test24BitButton("button.png", "button_selected.png", "button_down.png", "", "24-bit", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    jumpButton("button.png", "button_selected.png", "button_down.png", "", "Jump", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    ambientButton("button.png", "button_selected.png", "button_down.png", "", "Ambient", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    musicButton("button.png", "button_selected.png", "button_down.png", "", "Music", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK)
{
    handler.gamepadHandler = bind(&SoundSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    handler.uiHandler = bind(&SoundSample::handleUI, this, placeholders::_1, placeholders::_2);
    handler.keyboardHandler = bind(&SoundSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    engine->getEventDispatcher().addEventHandler(&handler);

    engine->getAudio()->addListener(&listener);

    soundMixer.setOutput(&listener);
    soundMixer.setGain(1.2F);

    test8BitSound = Sound(engine->getCache().getSoundData("8-bit.wav"));
    test8BitSound.setPitch(2.0F);
    test8BitSound.setOutput(&soundMixer);

    test24BitSound = Sound(engine->getCache().getSoundData("24-bit.wav"));
    test24BitSound.setPitch(0.5F);
    test24BitSound.setOutput(&soundMixer);

    jumpSound = Sound(engine->getCache().getSoundData("jump.wav"));

    guiLayer.addChild(&soundActor);
    soundActor.addComponent(&jumpSound);
    soundActor.setPosition(Vector3(8.0F, 0.0F, 10.0F));
    jumpSound.setOutput(&soundMixer);

    ambientSound = Sound(engine->getCache().getSoundData("ambient.wav"));
    ambientSound.setOutput(&soundMixer);

    music = Sound(engine->getCache().getSoundData("music.ogg"));
    music.setOutput(&listener);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2(800.0F, 600.0F));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    test8BitButton.setPosition(Vector2(0.0F, 80.0F));
    menu.addWidget(&test8BitButton);

    test24BitButton.setPosition(Vector2(0.0F, 40.0F));
    menu.addWidget(&test24BitButton);

    jumpButton.setPosition(Vector2(0.0F, 0.0F));
    menu.addWidget(&jumpButton);

    ambientButton.setPosition(Vector2(0.0F, -40.0F));
    menu.addWidget(&ambientButton);

    musicButton.setPosition(Vector2(0.0F, -80.0F));
    menu.addWidget(&musicButton);

    backButton.setPosition(Vector2(-200.0F, -200.0F));
    menu.addWidget(&backButton);
}

bool SoundSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == Gamepad::Button::FACE_RIGHT)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return false;
}

bool SoundSample::handleUI(Event::Type type, const UIEvent& event)
{
    if (type == Event::Type::ACTOR_CLICK)
    {
        if (event.actor == &backButton)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        else if (event.actor == &test8BitButton)
            test8BitSound.play();
        else if (event.actor == &test24BitButton)
            test24BitSound.play();
        else if (event.actor == &jumpButton)
            jumpSound.play(true);
        else if (event.actor == &ambientButton)
            ambientSound.play();
        else if (event.actor == &musicButton)
            music.play();
    }

    return false;
}

bool SoundSample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
{
    if (type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
                engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                return true;
            default:
                break;
        }
    }
    else if (type == Event::Type::KEY_RELEASE)
    {
        switch (event.key)
        {
            case Keyboard::Key::MENU:
                return true;
            default:
                break;
        }
    }

    return false;
}
