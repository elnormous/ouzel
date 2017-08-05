// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SpritesSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

SpritesSample::SpritesSample():
    backButton(new ouzel::gui::Button("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK))
{
    eventHandler.gamepadHandler = bind(&SpritesSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&SpritesSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&SpritesSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    camera.reset(new scene::Camera());
    camera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera->setTargetContentSize(Size2(800.0f, 600.0f));

    layer.reset(new scene::Layer());
    layer->addChild(camera.get());
    addLayer(layer.get());

    // character
    characterSprite.reset(new scene::Sprite());
    characterSprite->init("run.json");
    characterSprite->play(true);

    character.reset(new scene::Node());
    character->addComponent(characterSprite.get());
    layer->addChild(character.get());
    character->setPosition(Vector2(-300.0f, 0.0f));

    move.reset(new scene::Move(4.0f, Vector2(300.0f, 0.0f)));
    character->addComponent(move.get());
    move->start();

    // fire
    fireSprite.reset(new scene::Sprite());
    fireSprite->init("fire.json");
    fireSprite->setOffset(Vector2(0.0f, 20.0f));
    fireSprite->play(true);

    fireNode.reset(new scene::Node());
    fireNode->addComponent(fireSprite.get());
    fireNode->setPosition(Vector2(-100.0f, -140.0f));
    layer->addChild(fireNode.get());

    // triangle
    triangleSprite.reset(new scene::Sprite());
    triangleSprite->init("triangle.json");

    triangleNode.reset(new scene::Node());
    triangleNode->addComponent(triangleSprite.get());
    triangleNode->setPosition(Vector2(100.0f, -140.0f));
    layer->addChild(triangleNode.get());

    guiCamera.reset(new scene::Camera());
    guiCamera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera->setTargetContentSize(Size2(800.0f, 600.0f));

    guiLayer.reset(new scene::Layer());
    guiLayer->addChild(guiCamera.get());
    addLayer(guiLayer.get());

    menu.reset(new gui::Menu());
    guiLayer->addChild(menu.get());

    hideButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Show/hide", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK));
    hideButton->setPosition(Vector2(-200.0f, 200.0f));
    menu->addWidget(hideButton.get());

    wireframeButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Wireframe", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK));
    wireframeButton->setPosition(Vector2(-200.0f, 160.0f));
    menu->addWidget(wireframeButton.get());

    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton.get());
}

bool SpritesSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == input::GamepadButton::FACE_RIGHT)
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        }
    }

    return true;
}

bool SpritesSample::handleUI(Event::Type type, const UIEvent& event)
{
    if (type == Event::Type::CLICK_NODE)
    {
        if (event.node == backButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
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
    if (type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case input::KeyboardKey::ESCAPE:
            case input::KeyboardKey::MENU:
                sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                break;
            default:
                break;
        }
    }

    return true;
}
