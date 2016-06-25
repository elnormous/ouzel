// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundSample.h"

using namespace std;
using namespace ouzel;


SoundSample::SoundSample(Application& app):
    application(app)
{
    eventHandler = make_shared<EventHandler>();
    eventHandler->uiHandler = bind(&SoundSample::handleUI, this, placeholders::_1, placeholders::_2, placeholders::_3);
    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    scene::LayerPtr layer = make_shared<scene::Layer>();
    addLayer(layer);
    layer->setCamera(make_shared<scene::Camera>());

    soundButton = make_shared<gui::Button>("button.png", "button.png", "button_down.png", "", "Sound", graphics::Color(255, 0, 0, 255), "arial.fnt");
    soundButton->setPosition(Vector2(-200.0f, 100.0f));
    layer->addChild(soundButton);

    audio::SoundDataPtr soundData = sharedEngine->getAudio()->createSoundData();
    soundData->initFromFile("jump.wav");

    sound = sharedEngine->getAudio()->createSound();
    sound->init(soundData);

    scene::LayerPtr guiLayer = make_shared<scene::Layer>();
    guiLayer->setCamera(make_shared<scene::Camera>());
    addLayer(guiLayer);

    backButton = make_shared<gui::Button>("button.png", "button.png", "button_down.png", "", "Back", graphics::Color(0, 0, 0, 255), "arial.fnt");
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    guiLayer->addChild(backButton);
}

SoundSample::~SoundSample()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
}

bool SoundSample::handleUI(Event::Type type, const UIEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(event);

    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (sender == backButton)
        {
            application.back();
        }
        else if (sender == soundButton)
        {
            sound->play();
        }
    }

    return true;
}
