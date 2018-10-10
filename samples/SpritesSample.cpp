// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SpritesSample.hpp"
#include "MainMenu.hpp"

using namespace std;
using namespace ouzel;
using namespace input;

SpritesSample::SpritesSample():
    hideButton("button.png", "button_selected.png", "button_down.png", "", "Show/hide", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    wireframeButton("button.png", "button_selected.png", "button_down.png", "", "Wireframe", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK)
{
    handler.gamepadHandler = bind(&SpritesSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    handler.uiHandler = bind(&SpritesSample::handleUI, this, placeholders::_1, placeholders::_2);
    handler.keyboardHandler = bind(&SpritesSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    engine->getEventDispatcher().addEventHandler(&handler);

    camera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera.setTargetContentSize(Size2(800.0F, 600.0F));
    cameraActor.addComponent(&camera);
    layer.addChild(&cameraActor);
    addLayer(&layer);

    // character
    characterSprite.init("run.json");
    characterSprite.setAnimation("", true);
    characterSprite.play();

    character.addComponent(&characterSprite);
    layer.addChild(&character);
    character.setPosition(Vector2(-300.0F, 0.0F));

    move.reset(new scene::Move(4.0F, Vector2(300.0F, 0.0F)));
    character.addComponent(move);
    move->start();

    // fire
    fireSprite.init("fire.json");
    fireSprite.setOffset(Vector2(0.0F, 20.0F));
    fireSprite.setAnimation("", true);
    fireSprite.play();

    fireActor.addComponent(&fireSprite);
    fireActor.setPosition(Vector2(-100.0F, -140.0F));
    layer.addChild(&fireActor);

    // triangle
    triangleSprite.init("triangle.json");

    triangleActor.addComponent(&triangleSprite);
    triangleActor.setPosition(Vector2(100.0F, -140.0F));
    layer.addChild(&triangleActor);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2(800.0F, 600.0F));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    hideButton.setPosition(Vector2(-200.0F, 200.0F));
    menu.addWidget(&hideButton);

    wireframeButton.setPosition(Vector2(-200.0F, 160.0F));
    menu.addWidget(&wireframeButton);

    backButton.setPosition(Vector2(-200.0F, -200.0F));
    menu.addWidget(&backButton);
}

bool SpritesSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == Gamepad::Button::FACE_RIGHT)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return false;
}

bool SpritesSample::handleUI(Event::Type type, const UIEvent& event)
{
    if (type == Event::Type::ACTOR_CLICK)
    {
        if (event.actor == &backButton)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        else if (event.actor == &hideButton)
            character.setHidden(!character.isHidden());
        else if (event.actor == &wireframeButton)
            camera.setWireframe(!camera.getWireframe());
    }

    return false;
}

bool SpritesSample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
{
    if (type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
                engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                return true;
            default:
                break;
        }
    }
    else if (type == Event::Type::KEY_RELEASE)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
                return true;
            default:
                break;
        }
    }

    return false;
}
