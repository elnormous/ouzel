// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace audio;
using namespace input;

SoundSample::SoundSample():
    submix(*engine->getAudio()),
    listener(*engine->getAudio()),
    soundGain(*engine->getAudio()),
    test8BitSubmix(*engine->getAudio()),
    test8BitVoice(*engine->getAudio(), engine->getCache().getSound("8-bit.wav")),
    test8BitPitch(*engine->getAudio()),
    test24BitSubmix(*engine->getAudio()),
    test24BitVoice(*engine->getAudio(), engine->getCache().getSound("24-bit.wav")),
    test24BitPitch(*engine->getAudio()),
    jumpSubmix(*engine->getAudio()),
    jumpVoice(*engine->getAudio(), engine->getCache().getSound("jump.wav")),
    jumpPanner(*engine->getAudio()),
    ambientVoice(*engine->getAudio(), engine->getCache().getSound("ambient.wav")),
    music(*engine->getAudio(), engine->getCache().getSound("music.ogg")),
    tone(std::make_shared<ToneSound>(*engine->getAudio(), 200.0F, ToneSound::Type::SINE, 0.5F, 1.0F)),
    toneVoice(*engine->getAudio(), tone),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    test8BitButton("button.png", "button_selected.png", "button_down.png", "", "8-bit", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    test24BitButton("button.png", "button_selected.png", "button_down.png", "", "24-bit", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    jumpButton("button.png", "button_selected.png", "button_down.png", "", "Jump", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    ambientButton("button.png", "button_selected.png", "button_down.png", "", "Ambient", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    musicButton("button.png", "button_selected.png", "button_down.png", "", "Music", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    toneButton("button.png", "button_selected.png", "button_down.png", "", "Tone", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK)
{
    handler.gamepadHandler = std::bind(&SoundSample::handleGamepad, this, std::placeholders::_1);
    handler.uiHandler = std::bind(&SoundSample::handleUI, this, std::placeholders::_1);
    handler.keyboardHandler = std::bind(&SoundSample::handleKeyboard, this, std::placeholders::_1);
    engine->getEventDispatcher().addEventHandler(&handler);

    submix.setOutput(&engine->getAudio()->getMasterMix());

    listener.setMix(&submix);

    soundGain.setMix(&submix);
    soundGain.setGain(1.0F);

    test8BitPitch.setPitch(2.0F);
    test8BitVoice.setOutput(&test8BitSubmix);
    test8BitPitch.setMix(&test8BitSubmix);
    test8BitSubmix.setOutput(&submix);

    test24BitPitch.setPitch(0.5F);
    test24BitVoice.setOutput(&test24BitSubmix);
    test24BitPitch.setMix(&test24BitSubmix);
    test24BitSubmix.setOutput(&submix);

    guiLayer.addChild(&soundActor);
    soundActor.addComponent(&jumpPanner);
    soundActor.setPosition(Vector3(8.0F, 0.0F, 10.0F));
    jumpVoice.setOutput(&jumpSubmix);
    jumpPanner.setMix(&jumpSubmix);
    jumpSubmix.setOutput(&submix);

    ambientVoice.setOutput(&submix);
    music.setOutput(&submix);
    toneVoice.setOutput(&submix);

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

    toneButton.setPosition(Vector2(0.0F, -120.0F));
    menu.addWidget(&toneButton);

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
             test8BitVoice.isPlaying() ? test8BitVoice.pause() : test8BitVoice.play();
        else if (event.actor == &test24BitButton)
             test24BitVoice.isPlaying() ? test24BitVoice.pause() : test24BitVoice.play();
        else if (event.actor == &jumpButton)
             jumpVoice.isPlaying() ? jumpVoice.pause() : jumpVoice.play(true);
        else if (event.actor == &ambientButton)
             ambientVoice.isPlaying() ? ambientVoice.pause() : ambientVoice.play();
        else if (event.actor == &musicButton)
            music.isPlaying() ? music.pause() : music.play();
        else if (event.actor == &toneButton)
            toneVoice.isPlaying() ? toneVoice.pause() : toneVoice.play();
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
