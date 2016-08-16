// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundSample.h"

using namespace std;
using namespace ouzel;


SoundSample::SoundSample(Samples& pSamples):
    samples(pSamples)
{
    eventHandler.uiHandler = bind(&SoundSample::handleUI, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    scene::LayerPtr layer = make_shared<scene::Layer>();
    addLayer(layer);
    layer->setCamera(make_shared<scene::Camera>());

    jumpButton = make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Jump", graphics::Color(0, 0, 0, 255), "arial.fnt");
    jumpButton->setPosition(Vector2(0.0f, 0.0f));
    layer->addChild(jumpButton);

    ambientButton = make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Ambient", graphics::Color(0, 0, 0, 255), "arial.fnt");
    ambientButton->setPosition(Vector2(0.0f, -40.0f));
    layer->addChild(ambientButton);

    audio::SoundDataPtr jumpData = sharedEngine->getAudio()->createSoundData();
    jumpData->initFromFile("jump.wav");

    jumpSound = sharedEngine->getAudio()->createSound();
    jumpSound->init(jumpData);

    audio::SoundDataPtr ambientData = sharedEngine->getAudio()->createSoundData();
    ambientData->initFromFile("ambient.wav");

    ambientSound = sharedEngine->getAudio()->createSound();
    ambientSound->init(ambientData);

    scene::LayerPtr guiLayer = make_shared<scene::Layer>();
    guiLayer->setCamera(make_shared<scene::Camera>());
    addLayer(guiLayer);

    backButton = make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Back", graphics::Color(0, 0, 0, 255), "arial.fnt");
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    guiLayer->addChild(backButton);
}

SoundSample::~SoundSample()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
}

bool SoundSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == backButton)
        {
            samples.back();
        }
        else if (event.node == jumpButton)
        {
            jumpSound->play();
        }
        else if (event.node == ambientButton)
        {
            ambientSound->play();
        }
    }

    return true;
}
