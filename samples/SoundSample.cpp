// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace audio;
using namespace input;

SoundSample::SoundSample():
    listener(*engine->getAudio()),
    soundGain(*engine->getAudio()),
    test8BitPitch(*engine->getAudio()),
    test24BitPitch(*engine->getAudio()),
    jumpPanner(*engine->getAudio()),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    test8BitButton("button.png", "button_selected.png", "button_down.png", "", "8-bit", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    test24BitButton("button.png", "button_selected.png", "button_down.png", "", "24-bit", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    jumpButton("button.png", "button_selected.png", "button_down.png", "", "Jump", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    ambientButton("button.png", "button_selected.png", "button_down.png", "", "Ambient", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    musicButton("button.png", "button_selected.png", "button_down.png", "", "Music", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK)
{
    handler.gamepadHandler = std::bind(&SoundSample::handleGamepad, this, std::placeholders::_1);
    handler.uiHandler = std::bind(&SoundSample::handleUI, this, std::placeholders::_1);
    handler.keyboardHandler = std::bind(&SoundSample::handleKeyboard, this, std::placeholders::_1);
    engine->getEventDispatcher().addEventHandler(&handler);

    //listener.setOutput(&engine->getAudio()->getDestination());

    //soundGain.setOutput(&listener);
    soundGain.setGain(1.2F);

    test8BitSound = Sound(engine->getCache().getSoundData("8-bit.wav"));
    test8BitPitch.setPitch(2.0F);
    //test8BitSound.setOutput(&test8BitPitch);
    //test8BitPitch.setOutput(&soundGain);

    test24BitSound = Sound(engine->getCache().getSoundData("24-bit.wav"));
    test24BitPitch.setPitch(0.5F);
    //test24BitSound.setOutput(&test24BitPitch);
    //test24BitPitch.setOutput(&soundGain);

    jumpSound = Sound(engine->getCache().getSoundData("jump.wav"));

    guiLayer.addChild(&soundActor);
    soundActor.addComponent(&jumpPanner);
    soundActor.setPosition(Vector3(8.0F, 0.0F, 10.0F));
    //jumpSound.setOutput(&jumpPanner);
    //jumpPanner.setOutput(&soundGain);

    ambientSound = Sound(engine->getCache().getSoundData("ambient.wav"));
    //ambientSound.setOutput(&soundGain);

    music = Sound(engine->getCache().getSoundData("music.ogg"));
    //music.setOutput(&listener);

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

bool SoundSample::handleGamepad(const GamepadEvent& event)
{
    if (event.type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == Gamepad::Button::FACE_RIGHT)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return false;
}

bool SoundSample::handleUI(const UIEvent& event)
{
    if (event.type == Event::Type::ACTOR_CLICK)
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

bool SoundSample::handleKeyboard(const KeyboardEvent& event) const
{
    if (event.type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
            case Keyboard::Key::BACK:
                engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                return true;
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
                return true;
            default:
                break;
        }
    }

    return false;
}
