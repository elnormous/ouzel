// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SpritesSample.h"

using namespace std;
using namespace ouzel;

SpritesSample::SpritesSample(Samples& pSamples):
    samples(pSamples)
{
    eventHandler.uiHandler = bind(&SpritesSample::handleUI, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    ouzel::scene::LayerPtr layer = make_shared<scene::Layer>();
    addLayer(layer);
    layer->setCamera(make_shared<scene::Camera>());

    scene::SpritePtr characterSprite = make_shared<scene::Sprite>("run.json");
    characterSprite->play(true);

    character = make_shared<scene::Node>();
    character->addComponent(characterSprite);
    layer->addChild(character);
    character->setPosition(Vector2(-300.0f, 0.0f));
    character->animate(make_shared<scene::Move>(4.0f, Vector2(300.0f, 0.0f)));

    scene::SpritePtr fireSprite = make_shared<scene::Sprite>("fire.json");
    fireSprite->setOffset(Vector2(0.0f, 20.0f));
    fireSprite->play(true);

    scene::NodePtr fireNode = make_shared<scene::Node>();
    fireNode->addComponent(fireSprite);
    fireNode->setPosition(Vector2(-100.0f, -140.0f));
    layer->addChild(fireNode);

    ouzel::scene::LayerPtr guiLayer = make_shared<scene::Layer>();
    addLayer(guiLayer);
    guiLayer->setCamera(make_shared<scene::Camera>());

    button = make_shared<gui::Button>("button.png", "button.png", "button_down.png", "", "Show/hide", graphics::Color(0, 0, 0, 255), "arial.fnt");
    button->setPosition(Vector2(-200.0f, 200.0f));
    guiLayer->addChild(button);

    backButton = make_shared<gui::Button>("button.png", "button.png", "button_down.png", "", "Back", graphics::Color(0, 0, 0, 255), "arial.fnt");
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    guiLayer->addChild(backButton);
}

SpritesSample::~SpritesSample()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
}

bool SpritesSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == backButton)
        {
            samples.back();
        }
        else if (event.node == button)
        {
            character->setVisible(!character->isVisible());
        }
    }
    
    return true;
}
