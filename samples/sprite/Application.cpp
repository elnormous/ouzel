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
        
        _engine->getRenderer()->setClearColor(Color(64, 0, 0));

        _sprite = new Sprite("run.json", _engine->getScene());
        _sprite->play(true);
        _engine->getScene()->getRootNode()->addChild(_sprite);
        
        _witch = new Sprite("witch.png", _engine->getScene());
        _witch->setPosition(Vector2(100.0f, 100.0f));
        _witch->setColor(Color(128, 0, 255, 255));
        _engine->getScene()->getRootNode()->addChild(_witch);
        
        Sprite* fire = new Sprite("fire.json", _engine->getScene());
        fire->play(true);
        fire->setPosition(Vector2(-100.0f, -100.0f));
        _engine->getScene()->getRootNode()->addChild(fire);
    }
    
    Application::~Application()
    {
        _engine->removeEventHandler(this);
    }
    
    bool Application::handleEvent(const Event& event)
    {
        switch (event.type)
        {
            case Event::Type::KEY_DOWN:
            {
                Vector2 position = _sprite->getPosition();
                
                switch (event.keyboardEvent.key)
                {
                    case KeyboardKey::UP:
                        position.y += 10.0f;
                        break;
                    case KeyboardKey::DOWN:
                        position.y -= 10.0f;
                        break;
                    case KeyboardKey::LEFT:
                        position.x -= 10.0f;
                        break;
                    case KeyboardKey::RIGHT:
                        position.x += 10.0f;
                        break;
                    default:
                        break;
                }
                
                _sprite->setPosition(position);
                break;
            }
            case Event::Type::MOUSE_MOVE:
            {
                _witch->setPosition(event.mouseEvent.position);
                break;
            }
            default:
                break;
        }
        
        return true;
    }
    
    void Application::update(float delta)
    {
        
    }
}
