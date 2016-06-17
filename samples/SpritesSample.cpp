// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SpritesSample.h"

using namespace std;
using namespace ouzel;

SpritesSample::SpritesSample(Application& app):
    application(app)
{
    eventHandler = make_shared<EventHandler>();
    eventHandler->uiHandler = bind(&SpritesSample::handleUI, this, placeholders::_1, placeholders::_2, placeholders::_3);
    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    ouzel::scene::LayerPtr layer = make_shared<scene::Layer>();
    addLayer(layer);
    layer->setCamera(make_shared<scene::Camera>());

    scene::SpritePtr characterSprite = make_shared<scene::Sprite>("run.json");
    characterSprite->play(true);

    character = make_shared<scene::Node>();
    character->addDrawable(characterSprite);
    layer->addChild(character);
    character->setPosition(Vector2(-300.0f, 0.0f));
    character->animate(make_shared<scene::Move>(4.0f, Vector2(300.0f, 0.0f)));

    scene::SpritePtr fireSprite = make_shared<scene::Sprite>("fire.json");
    fireSprite->play(true);

    scene::NodePtr fireNode = make_shared<scene::Node>();
    fireNode->addDrawable(fireSprite);
    fireNode->setPosition(Vector2(-100.0f, -100.0f));
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

bool SpritesSample::handleUI(Event::Type type, const UIEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(event);

    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (sender == backButton)
        {
            application.back();
        }
        else if (sender == button)
        {
            character->setVisible(!character->isVisible());
        }
    }
    
    return true;
}
