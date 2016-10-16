// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SpritesSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

SpritesSample::SpritesSample()
{
    eventHandler.uiHandler = bind(&SpritesSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&SpritesSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    layer = make_shared<scene::Layer>();
    addLayer(layer);
    layer->addCamera(make_shared<scene::Camera>());

    // character
    scene::SpritePtr characterSprite = make_shared<scene::Sprite>("run.json");
    characterSprite->play(true);

    character = make_shared<scene::Node>();
    character->addComponent(characterSprite);
    layer->addChild(character);
    character->setPosition(Vector2(-300.0f, 0.0f));
    character->animate(make_shared<scene::Move>(4.0f, Vector2(300.0f, 0.0f)));

    // fire
    scene::SpritePtr fireSprite = make_shared<scene::Sprite>("fire.json");
    fireSprite->setOffset(Vector2(0.0f, 20.0f));
    fireSprite->play(true);

    scene::NodePtr fireNode = make_shared<scene::Node>();
    fireNode->addComponent(fireSprite);
    fireNode->setPosition(Vector2(-100.0f, -140.0f));
    layer->addChild(fireNode);

    // triangle
    scene::SpritePtr triangleSprite = make_shared<scene::Sprite>("triangle.json");
    scene::NodePtr triangleNode = make_shared<scene::Node>();
    triangleNode->addComponent(triangleSprite);
    triangleNode->setPosition(Vector2(100.0f, -140.0f));
    layer->addChild(triangleNode);

    ouzel::scene::LayerPtr guiLayer = make_shared<scene::Layer>();
    guiLayer->addCamera(make_shared<scene::Camera>());
    addLayer(guiLayer);

    gui::MenuPtr menu = std::make_shared<gui::Menu>();
    guiLayer->addChild(menu);

    hideButton = make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Show/hide", graphics::Color::BLACK, "arial.fnt");
    hideButton->setPosition(Vector2(-200.0f, 200.0f));
    menu->addWidget(hideButton);

    wireframeButton = make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Wireframe", graphics::Color::BLACK, "arial.fnt");
    wireframeButton->setPosition(Vector2(-200.0f, 160.0f));
    menu->addWidget(wireframeButton);

    backButton = make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Back", graphics::Color::BLACK, "arial.fnt");
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton);
}

SpritesSample::~SpritesSample()
{
}

bool SpritesSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == backButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::make_shared<MainMenu>());
        }
        else if (event.node == hideButton.get())
        {
            character->setHidden(!character->isHidden());
        }
        else if (event.node == wireframeButton.get())
        {
            layer->setWireframe(!layer->getWireframe());
        }
    }

    return true;
}

bool SpritesSample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
{
    if (type == Event::Type::KEY_DOWN)
    {
        switch (event.key)
        {
            case input::KeyboardKey::ESCAPE:
                sharedEngine->getSceneManager()->setScene(std::make_shared<MainMenu>());
                break;
            default:
                break;
        }
    }

    return true;
}
