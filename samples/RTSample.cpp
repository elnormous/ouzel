// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RTSample.h"

using namespace std;
using namespace ouzel;

RTSample::RTSample()
{
    ouzel::scene::LayerPtr rtLayer = make_shared<scene::Layer>();
    addLayer(rtLayer);

    ouzel::graphics::RenderTargetPtr renderTarget = sharedEngine->getRenderer()->createRenderTarget(Size2(256.0f, 256.0f), false);
    renderTarget->setClearColor(graphics::Color(0, 64, 0));
    rtLayer->setCamera(make_shared<scene::Camera>());
    rtLayer->setRenderTarget(renderTarget);
    
    ouzel::scene::LayerPtr layer = make_shared<scene::Layer>();
    addLayer(layer);
    layer->setCamera(make_shared<scene::Camera>());

    scene::SpritePtr characterSprite = scene::Sprite::createFromFile("run.json");
    characterSprite->play(true);

    scene::NodePtr rtCharacter = make_shared<scene::Node>();
    rtCharacter->addDrawable(characterSprite);
    rtLayer->addChild(rtCharacter);

    scene::SpriteFramePtr rtFrame = scene::SpriteFrame::create(Rectangle(0.0f, 0.0f, 256.0f, 256.0f), renderTarget->getTexture(), false, renderTarget->getTexture()->getSize(), Vector2(), Vector2(0.5f, 0.5f));

    scene::SpritePtr rtSprite = scene::Sprite::createFromSpriteFrames({ rtFrame });
    scene::NodePtr rtNode = make_shared<scene::Node>();
    rtNode->addDrawable(rtSprite);
    layer->addChild(rtNode);
}
