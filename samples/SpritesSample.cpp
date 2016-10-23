// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SpritesSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

SpritesSample::SpritesSample(Samples& aSamples):
    samples(aSamples)
{
    eventHandler.uiHandler = bind(&SpritesSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&SpritesSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    layer.reset(new scene::Layer());
    addLayer(layer.get());

    camera.reset(new scene::Camera());
    layer->addCamera(camera.get());

    // character
    characterSprite.reset(new scene::Sprite("run.json"));
    characterSprite->play(true);

    character.reset(new scene::Node());
    character->addComponent(characterSprite.get());
    layer->addChild(character.get());
    character->setPosition(Vector2(-300.0f, 0.0f));

    move.reset(new scene::Move(4.0f, Vector2(300.0f, 0.0f)));
    character->animate(move.get());

    // fire
    fireSprite.reset(new scene::Sprite("fire.json"));
    fireSprite->setOffset(Vector2(0.0f, 20.0f));
    fireSprite->play(true);

    fireNode.reset(new scene::Node());
    fireNode->addComponent(fireSprite.get());
    fireNode->setPosition(Vector2(-100.0f, -140.0f));
    layer->addChild(fireNode.get());

    // triangle
    triangleSprite.reset(new scene::Sprite("triangle.json"));
    triangleNode.reset(new scene::Node());
    triangleNode->addComponent(triangleSprite.get());
    triangleNode->setPosition(Vector2(100.0f, -140.0f));
    layer->addChild(triangleNode.get());

    guiLayer.reset(new scene::Layer());
    guiCamera.reset(new scene::Camera());
    guiLayer->addCamera(guiCamera.get());
    addLayer(guiLayer.get());

    menu.reset(new gui::Menu());
    guiLayer->addChild(menu.get());

    hideButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Show/hide", graphics::Color::BLACK, "arial.fnt"));
    hideButton->setPosition(Vector2(-200.0f, 200.0f));
    menu->addWidget(hideButton.get());

    wireframeButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Wireframe", graphics::Color::BLACK, "arial.fnt"));
    wireframeButton->setPosition(Vector2(-200.0f, 160.0f));
    menu->addWidget(wireframeButton.get());

    backButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Back", graphics::Color::BLACK, "arial.fnt"));
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton.get());
}

bool SpritesSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == backButton.get())
        {
            samples.setSample("");
        }
        else if (event.node == hideButton.get())
        {
            character->setHidden(!character->isHidden());
        }
        else if (event.node == wireframeButton.get())
        {
            camera->setWireframe(!camera->getWireframe());
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
                samples.setSample("");
                break;
            default:
                break;
        }
    }

    return true;
}
