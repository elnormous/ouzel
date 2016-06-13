//
//  SpritesSample.cpp
//  samples
//
//  Created by Elviss Strazdins on 13/06/16.
//  Copyright (c) 2016 Bool Games. All rights reserved.
//

#include "SpritesSample.h"

using namespace std;
using namespace ouzel;

SpritesSample::SpritesSample()
{
    eventHandler = make_shared<EventHandler>();

    eventHandler->uiHandler = bind(&SpritesSample::handleUI, this, placeholders::_1, placeholders::_2, placeholders::_3);

    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    layer = make_shared<scene::Layer>();
    addLayer(layer);
    layer->setCamera(make_shared<scene::Camera>());

    uiLayer = make_shared<scene::Layer>();
    addLayer(uiLayer);
    uiLayer->setCamera(make_shared<scene::Camera>());

    scene::SpritePtr characterSprite = scene::Sprite::createFromFile("run.json");
    characterSprite->play(true);

    character = make_shared<scene::Node>();
    character->addDrawable(characterSprite);
    layer->addChild(character);
    character->setPosition(Vector2(-300.0f, 0.0f));
    character->animate(make_shared<scene::Move>(4.0f, Vector2(300.0f, 0.0f)));

    scene::SpritePtr fireSprite = scene::Sprite::createFromFile("fire.json");
    fireSprite->play(true);

    scene::NodePtr fireNode = make_shared<scene::Node>();
    fireNode->addDrawable(fireSprite);
    fireNode->setPosition(Vector2(-100.0f, -100.0f));
    layer->addChild(fireNode);

    button = gui::Button::create("button.png", "button.png", "button_down.png", "", "Show/hide", graphics::Color(0, 0, 0, 255), "arial.fnt");
    button->setPosition(Vector2(-200.0f, 200.0f));
    uiLayer->addChild(button);
}

bool SpritesSample::handleKeyboard(Event::Type type, const KeyboardEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

    if (type == Event::Type::KEY_DOWN)
    {
        Vector2 position = layer->getCamera()->getPosition();

        switch (event.key)
        {
            case input::KeyboardKey::UP:
                position.y += 10.0f;
                break;
            case input::KeyboardKey::DOWN:
                position.y -= 10.0f;
                break;
            case input::KeyboardKey::LEFT:
                position.x -= 10.0f;
                break;
            case input::KeyboardKey::RIGHT:
                position.x += 10.0f;
                break;
            case input::KeyboardKey::RETURN:
                sharedEngine->getWindow()->setSize(Size2(640.0f, 480.0f));
                break;
            case input::KeyboardKey::TAB:
                button->setEnabled(!button->isEnabled());
                break;
            default:
                break;
        }

        layer->getCamera()->setPosition(position);
    }

    return true;
}

SpritesSample::~SpritesSample()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
}

bool SpritesSample::handleUI(Event::Type type, const UIEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(event);

    if (type == Event::Type::UI_CLICK_NODE && sender == button)
    {
        character->setVisible(!character->isVisible());
    }
    
    return true;
}
