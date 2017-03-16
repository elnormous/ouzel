// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RTSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

RTSample::RTSample(Samples& aSamples):
    samples(aSamples),
    characterSprite("run.json"),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK)
{
    eventHandler.gamepadHandler = bind(&RTSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&RTSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&RTSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    rtLayer.setScene(this);

    std::shared_ptr<graphics::Texture> renderTarget(new graphics::Texture());
    renderTarget->init(Size2(256.0f, 256.0f), true, false, true, 1, false);
    renderTarget->setClearColor(Color(0, 64, 0));

    rtCamera.setRenderTarget(renderTarget);
    rtCamera.setLayer(&rtLayer);

    camera1.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera1.setTargetContentSize(Size2(400.0f, 600.0f));
    camera1.setViewport(Rectangle(0.0f, 0.0f, 0.5f, 1.0f));
    camera1.setLayer(&layer);

    camera2.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera2.setTargetContentSize(Size2(400.0f, 600.0f));
    camera2.setViewport(Rectangle(0.5f, 0.0f, 0.5f, 1.0f));
    camera2.setLayer(&layer);

    layer.setScene(this);

    characterSprite.play(true);

    characterSprite.setNode(&rtCharacter);
    rtCharacter.setParent(&rtLayer);

    scene::SpriteFrame rtFrame(renderTarget, Rectangle(0.0f, 0.0f, 256.0f, 256.0f), false, renderTarget->getSize(), Vector2(), Vector2(0.5f, 0.5f));

    std::vector<scene::SpriteFrame> spriteFrames = {rtFrame};
    rtSprite.initFromSpriteFrames(spriteFrames);
    rtSprite.setNode(&rtNode);
    rtNode.setParent(&layer);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2(800.0f, 600.0f));
    guiCamera.setLayer(&guiLayer);
    guiLayer.setScene(this);

    menu.setParent(&guiLayer);

    backButton.setPosition(Vector2(-200.0f, -200.0f));
    backButton.setMenu(&menu);
    backButton.setParent(&menu);
}

bool RTSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == input::GamepadButton::B)
        {
            samples.setScene(std::unique_ptr<scene::Scene>(new MainMenu(samples)));
        }
    }

    return true;
}

bool RTSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE && event.node == &backButton)
    {
        samples.setScene(std::unique_ptr<scene::Scene>(new MainMenu(samples)));
    }

    return true;
}

bool RTSample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
{
    if (type == Event::Type::KEY_DOWN)
    {
        switch (event.key)
        {
            case input::KeyboardKey::ESCAPE:
                samples.setScene(std::unique_ptr<scene::Scene>(new MainMenu(samples)));
                break;
            default:
                break;
        }
    }
    
    return true;
}
