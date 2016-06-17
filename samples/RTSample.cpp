// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RTSample.h"

using namespace std;
using namespace ouzel;

RTSample::RTSample(Application& app):
    application(app)
{
    eventHandler = make_shared<EventHandler>();
    eventHandler->uiHandler = bind(&RTSample::handleUI, this, placeholders::_1, placeholders::_2, placeholders::_3);
    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);
    
    ouzel::scene::LayerPtr rtLayer = make_shared<scene::Layer>();
    addLayer(rtLayer);

    ouzel::graphics::RenderTargetPtr renderTarget = sharedEngine->getRenderer()->createRenderTarget(Size2(256.0f, 256.0f), false);
    renderTarget->setClearColor(graphics::Color(0, 64, 0));
    rtLayer->setCamera(make_shared<scene::Camera>());
    rtLayer->setRenderTarget(renderTarget);
    
    ouzel::scene::LayerPtr layer = make_shared<scene::Layer>();
    addLayer(layer);
    layer->setCamera(make_shared<scene::Camera>());

    scene::SpritePtr characterSprite = make_shared<scene::Sprite>("run.json");
    characterSprite->play(true);

    scene::NodePtr rtCharacter = make_shared<scene::Node>();
    rtCharacter->addDrawable(characterSprite);
    rtLayer->addChild(rtCharacter);

    scene::SpriteFramePtr rtFrame = scene::SpriteFrame::create(Rectangle(0.0f, 0.0f, 256.0f, 256.0f), renderTarget->getTexture(), false, renderTarget->getTexture()->getSize(), Vector2(), Vector2(0.5f, 0.5f));

    const std::vector<scene::SpriteFramePtr>& spriteFrames = { rtFrame };
    scene::SpritePtr rtSprite = make_shared<scene::Sprite>(spriteFrames);
    scene::NodePtr rtNode = make_shared<scene::Node>();
    rtNode->addDrawable(rtSprite);
    layer->addChild(rtNode);

    scene::LayerPtr guiLayer = make_shared<scene::Layer>();
    guiLayer->setCamera(make_shared<scene::Camera>());
    addLayer(guiLayer);

    backButton = make_shared<gui::Button>("button.png", "button.png", "button_down.png", "", "Back", graphics::Color(0, 0, 0, 255), "arial.fnt");
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    guiLayer->addChild(backButton);
}

RTSample::~RTSample()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
}

bool RTSample::handleUI(Event::Type type, const UIEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(event);

    if (type == Event::Type::UI_CLICK_NODE && sender == backButton)
    {
        application.back();
    }

    return true;
}
