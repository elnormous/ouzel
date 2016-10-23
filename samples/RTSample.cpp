// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RTSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

RTSample::RTSample(Samples& aSamples):
    samples(aSamples),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", graphics::Color::BLACK, "arial.fnt")
{
    eventHandler.uiHandler = bind(&RTSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&RTSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    rtLayer.reset(new scene::Layer());
    addLayer(rtLayer.get());

    ouzel::graphics::RenderTargetPtr renderTarget = sharedEngine->getRenderer()->createRenderTarget();
    renderTarget->init(Size2(256.0f, 256.0f), false);
    renderTarget->setClearColor(graphics::Color(0, 64, 0));

    rtCamera.reset(new scene::Camera());
    rtCamera->setRenderTarget(renderTarget);
    rtLayer->addCamera(rtCamera.get());

    camera1.reset(new scene::Camera());
    camera1->setViewport(Rectangle(0.0f, 0.0f, 0.5f, 1.0f));
    camera2.reset(new scene::Camera());
    camera2->setViewport(Rectangle(0.5f, 0.0f, 0.5f, 1.0f));

    layer.reset(new scene::Layer());
    layer->addCamera(camera1.get());
    layer->addCamera(camera2.get());
    addLayer(layer.get());

    characterSprite.reset(new scene::Sprite("run.json"));
    characterSprite->play(true);

    rtCharacter.reset(new scene::Node());
    rtCharacter->addComponent(characterSprite.get());
    rtLayer->addChild(rtCharacter.get());

    scene::SpriteFramePtr rtFrame = std::make_shared<scene::SpriteFrame>(renderTarget->getTexture(), Rectangle(0.0f, 0.0f, 256.0f, 256.0f), false, renderTarget->getTexture()->getSize(), Vector2(), Vector2(0.5f, 0.5f));

    const std::vector<scene::SpriteFramePtr>& spriteFrames = { rtFrame };
    rtSprite.reset(new scene::Sprite(spriteFrames));
    rtNode.reset(new scene::Node());
    rtNode->addComponent(rtSprite.get());
    layer->addChild(rtNode.get());

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
