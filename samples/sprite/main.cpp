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
    
    void begin(ouzel::Engine* engine)
    {
        _application = new ouzel::Application(engine);
    }
    
private:
    ouzel::Application* _application;
};

GameDelegate* appDelegate = new GameDelegate();
std::unique_ptr<GameDelegate> ptr(appDelegate);