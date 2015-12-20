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
        _engine = new Engine(Renderer::Driver::OPENGL);
        
        _engine->addEventHandler(this);
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