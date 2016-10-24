// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RTSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

RTSample::RTSample(Samples& aSamples):
    samples(aSamples),
    characterSprite("run.json"),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", graphics::Color::BLACK, "arial.fnt")
{
    eventHandler.uiHandler = bind(&RTSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&RTSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    addLayer(&rtLayer);

    ouzel::graphics::RenderTargetPtr renderTarget = sharedEngine->getRenderer()->createRenderTarget();
    renderTarget->init(Size2(256.0f, 256.0f), false);
    renderTarget->setClearColor(graphics::Color(0, 64, 0));

    rtCamera.setRenderTarget(renderTarget);
    rtLayer.addCamera(&rtCamera);

    camera1.setViewport(Rectangle(0.0f, 0.0f, 0.5f, 1.0f));
    camera2.setViewport(Rectangle(0.5f, 0.0f, 0.5f, 1.0f));

    layer.addCamera(&camera1);
    layer.addCamera(&camera2);
    addLayer(&layer);

    characterSprite.play(true);

    rtCharacter.addComponent(&characterSprite);
    rtLayer.addChild(&rtCharacter);

    scene::SpriteFrame rtFrame(renderTarget->getTexture(), Rectangle(0.0f, 0.0f, 256.0f, 256.0f), false, renderTarget->getTexture()->getSize(), Vector2(), Vector2(0.5f, 0.5f));

    std::vector<scene::SpriteFrame> spriteFrames = { rtFrame };
    rtSprite.reset(new scene::Sprite(spriteFrames));
    rtNode.addComponent(rtSprite.get());
    layer.addChild(&rtNode);

    guiLayer.addCamera(&guiCamera);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    backButton.setPosition(Vector2(-200.0f, -200.0f));
    menu.addWidget(&backButton);
}

bool RTSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE && event.node == &backButton)
    {
        samples.setSample("");
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
                samples.setSample("");
                break;
            default:
                break;
        }
    }
    
    return true;
}
