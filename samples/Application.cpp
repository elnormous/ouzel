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
        _engine = new Engine(Renderer::Driver::OPENGL, Size2(640.0f, 480.0f), false);
        
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