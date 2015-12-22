// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Application.h"
#include <memory>

class GameDelegate: public ouzel::AppDelegate
{
public:
    GameDelegate()
    {
    }
    
    virtual ~GameDelegate()
    {
        delete _application;
    }
    
    virtual void begin(ouzel::Engine* engine) override
    {
        _application = new ouzel::Application(engine);
    }
    
private:
    ouzel::Application* _application;
};

GameDelegate appDelegate;