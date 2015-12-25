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
    Application::Application(Engine* engine):
        _engine(engine)
    {
        _engine->addEventHandler(this);
        
        AutoPtr<Sprite> sprite = new Sprite("tim-from-braid.png", _engine->getScene());
        _engine->getScene()->getRootNode()->addChild(sprite);
        
        AutoPtr<Sprite> witch = new Sprite("witch.png", _engine->getScene());
        witch->setPosition(Vector2(100.0f, 100.0f));
        _engine->getScene()->getRootNode()->addChild(witch);
    }
    
    Application::~Application()
    {
        _engine->removeEventHandler(this);
    }
    
    bool Application::handleEvent(const Event& event)
    {
        return true;
    }
    
    void Application::update(float delta)
    {
        
    }
}
