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
        Engine::getInstance()->addEventHandler(this);
        
        Renderer::getInstance()->setClearColor(Color(64, 0, 0));
        
        Scene* scene = new Scene();
        SceneManager::getInstance()->setScene(scene);
        
        _layer = new Layer();
        scene->addLayer(_layer);

        _sprite = new Sprite();
        _sprite->initFromFile("run.json");
        _sprite->play(true);
        _layer->addChild(_sprite);
        
        _witch = new Sprite();
        _witch->initFromFile("witch.png");
        _witch->setPosition(Vector2(100.0f, 100.0f));
        _witch->setColor(Color(128, 0, 255, 255));
        _layer->addChild(_witch);
        
        Sprite* fire = new Sprite();
        fire->initFromFile("fire.json");
        fire->play(true);
        fire->setPosition(Vector2(-100.0f, -100.0f));
        _layer->addChild(fire);
        
        ParticleSystem* flame = new ParticleSystem();
        flame->initFromFile("flame.json");
        _layer->addChild(flame);
    }
    
    Application::~Application()
    {
        Engine::getInstance()->removeEventHandler(this);
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
                Vector2 worldLocation = _layer->viewToWorldLocation(event.mouseEvent.position);
                _witch->setPosition(worldLocation);
                break;
            }
            case Event::Type::TOUCH_BEGIN:
            case Event::Type::TOUCH_MOVE:
            case Event::Type::TOUCH_END:
            {
                Vector2 worldLocation = _layer->viewToWorldLocation(event.touchEvent.position);
                _witch->setPosition(worldLocation);
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
