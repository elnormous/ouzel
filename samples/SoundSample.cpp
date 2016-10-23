// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

SoundSample::SoundSample(Samples& aSamples):
    samples(aSamples)
{
    eventHandler.uiHandler = bind(&SoundSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&SoundSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    audio::SoundDataPtr jumpData = sharedEngine->getAudio()->createSoundData();
    jumpData->initFromFile("jump.wav");

    jumpSound = sharedEngine->getAudio()->createSound();
    jumpSound->init(jumpData);

    audio::SoundDataPtr ambientData = sharedEngine->getAudio()->createSoundData();
    ambientData->initFromFile("ambient.wav");

    ambientSound = sharedEngine->getAudio()->createSound();
    ambientSound->init(ambientData);

    guiLayer.reset(new scene::Layer());
    guiCamera.reset(new scene::Camera());
    guiLayer->addCamera(guiCamera.get());
    addLayer(guiLayer.get());

    menu.reset(new gui::Menu());
    guiLayer->addChild(menu.get());

    jumpButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Jump", graphics::Color::BLACK, "arial.fnt"));
    jumpButton->setPosition(Vector2(0.0f, 0.0f));
    menu->addWidget(jumpButton.get());

    ambientButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Ambient", graphics::Color::BLACK, "arial.fnt"));
    ambientButton->setPosition(Vector2(0.0f, -40.0f));
    menu->addWidget(ambientButton.get());

    backButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Back", graphics::Color::BLACK, "arial.fnt"));
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton.get());
}

bool SoundSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == backButton.get())
        {
            samples.setSample("");
        }
        else if (event.node == jumpButton.get())
        {
            jumpSound->play();
        }
        else if (event.node == ambientButton.get())
        {
            ambientSound->play();
        }
    }

    return true;
}

bool SoundSample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
{
    if (type == Event::Type::KEY_DOWN)
    {
        switch (event.key)
        {
            case input::KeyboardKey::ESCAPE:
                samples.setSample("");
                break;
            default:
                break;
        }
    }

    return true;
}
