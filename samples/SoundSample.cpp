// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "SoundSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace audio;
using namespace input;

SoundSample::SoundSample():
    submix(*engine->getAudio()),
    listener(*engine->getAudio()),
    soundGain(*engine->getAudio()),
    test8Bit(*engine->getAudio(), *engine->getCache().getCue("8-bit.json")),
    test24Bit(*engine->getAudio(), *engine->getCache().getCue("24-bit.json")),
    jump(*engine->getAudio(), *engine->getCache().getCue("jump.json")),
    ambient(*engine->getAudio(), *engine->getCache().getCue("ambient.json")),
    music(*engine->getAudio(), *engine->getCache().getCue("music.json")),
    tone(*engine->getAudio(), *engine->getCache().getCue("tone.json")),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
    test8BitButton("button.png", "button_selected.png", "button_down.png", "", "8-bit", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
    test24BitButton("button.png", "button_selected.png", "button_down.png", "", "24-bit", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
    jumpButton("button.png", "button_selected.png", "button_down.png", "", "Jump", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
    ambientButton("button.png", "button_selected.png", "button_down.png", "", "Ambient", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
    musicButton("button.png", "button_selected.png", "button_down.png", "", "Music", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
    toneButton("button.png", "button_selected.png", "button_down.png", "", "Tone", "Arial", 1.0F, Color::black(), Color::black(), Color::black())
{
    handler.gamepadHandler = [](const GamepadEvent& event) {
        if (event.type == Event::Type::GamepadButtonChange)
        {
            if (event.pressed &&
                event.button == Gamepad::Button::FaceRight)
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());
        }

        return false;
    };

    handler.uiHandler = [this](const UIEvent& event) {
        if (event.type == Event::Type::ActorClick)
        {
            if (event.actor == &backButton)
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());
            else if (event.actor == &test8BitButton)
                test8Bit.isPlaying() ? test8Bit.pause() : test8Bit.play();
            else if (event.actor == &test24BitButton)
                test24Bit.isPlaying() ? test24Bit.pause() : test24Bit.play();
            else if (event.actor == &jumpButton)
                jump.isPlaying() ? jump.pause() : jump.play();
            else if (event.actor == &ambientButton)
                ambient.isPlaying() ? ambient.pause() : ambient.play();
            else if (event.actor == &musicButton)
                music.isPlaying() ? music.pause() : music.play();
            else if (event.actor == &toneButton)
                tone.isPlaying() ? tone.pause() : tone.play();
        }

        return false;
    };

    handler.keyboardHandler = [](const KeyboardEvent& event) {
        if (event.type == Event::Type::KeyboardKeyPress)
        {
            switch (event.key)
            {
                case Keyboard::Key::Escape:
                case Keyboard::Key::Menu:
                case Keyboard::Key::Back:
                    engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                    return true;
                default:
                    break;
            }
        }
        else if (event.type == Event::Type::KeyboardKeyRelease)
        {
            switch (event.key)
            {
                case Keyboard::Key::Escape:
                case Keyboard::Key::Menu:
                case Keyboard::Key::Back:
                    return true;
                default:
                    break;
            }
        }

        return false;
    };

    engine->getEventDispatcher().addEventHandler(handler);

    submix.setOutput(&engine->getAudio()->getMasterMix());

    listener.setMix(&submix);
    //submix.addEffect(&soundGain);
    soundGain.setGain(1.0F);

    test8Bit.setOutput(&submix);
    test24Bit.setOutput(&submix);

    layer.addChild(&soundActor);
    //soundActor.addComponent(&jump);
    soundActor.setPosition(Vector3F(8.0F, 0.0F, 10.0F));
    jump.setOutput(&submix);

    ambient.setOutput(&submix);
    music.setOutput(&submix);
    tone.setOutput(&submix);

    camera.setClearColorBuffer(true);
    camera.setClearColor(ouzel::Color(64, 0, 0));
    camera.setScaleMode(scene::Camera::ScaleMode::ShowAll);
    camera.setTargetContentSize(Size2F(800.0F, 600.0F));
    cameraActor.addComponent(&camera);
    layer.addChild(&cameraActor);
    addLayer(&layer);

    layer.addChild(&menu);

    test8BitButton.setPosition(Vector2F(0.0F, 80.0F));
    menu.addWidget(&test8BitButton);

    test24BitButton.setPosition(Vector2F(0.0F, 40.0F));
    menu.addWidget(&test24BitButton);

    jumpButton.setPosition(Vector2F(0.0F, 0.0F));
    menu.addWidget(&jumpButton);

    ambientButton.setPosition(Vector2F(0.0F, -40.0F));
    menu.addWidget(&ambientButton);

    musicButton.setPosition(Vector2F(0.0F, -80.0F));
    menu.addWidget(&musicButton);

    toneButton.setPosition(Vector2F(0.0F, -120.0F));
    menu.addWidget(&toneButton);

    backButton.setPosition(Vector2F(-200.0F, -200.0F));
    menu.addWidget(&backButton);
}
