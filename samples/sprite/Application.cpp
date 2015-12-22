//
//  Application.cpp
//  ouzel
//
//  Created by Elviss Strazdins on 20/12/15.
//  Copyright © 2015 Bool Games. All rights reserved.
//

#include "Application.h"

namespace ouzel
{
    Application::Application()
    {
        _engine = new Engine(Renderer::Driver::DIRECT3D11, Size2(640.0f, 480.0f), false);
        _engine->addEventHandler(this);
    }
    
    Application::~Application()
    {
        _engine->removeEventHandler(this);
    }
    
    void Application::begin()
    {
        Sprite* sprite = new Sprite("tim-from-braid.png", _engine->getScene());
        _engine->getScene()->getRootNode()->addChild(sprite);
        
        Sprite* witch = new Sprite("witch.png", _engine->getScene());
        witch->setPosition(Vector2(100.0f, 100.0f));
        _engine->getScene()->getRootNode()->addChild(witch);
        
        sprite->release();
    }
    
    bool Application::handleEvent(const Event& event)
    {
        return true;
    }
    
    void Application::update(float delta)
    {
        
    }
}
