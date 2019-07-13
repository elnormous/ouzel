// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "GameSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;

GameSample::GameSample()
{
    handler.keyboardHandler = std::bind(&GameSample::handleKeyboard, this, std::placeholders::_1);
    handler.mouseHandler = std::bind(&GameSample::handleMouse, this, std::placeholders::_1);
    handler.touchHandler = std::bind(&GameSample::handleTouch, this, std::placeholders::_1);
    handler.gamepadHandler = std::bind(&GameSample::handleGamepad, this, std::placeholders::_1);
    handler.uiHandler = std::bind(&GameSample::handleUI, this, std::placeholders::_1);

    camera.setClearColorBuffer(true);
    camera.setClearColor(ouzel::Color(64, 0, 0));
    
    engine->getEventDispatcher().addEventHandler(handler);

    addLayer(&layer);
    cameraActor.addComponent(&camera);
    layer.addChild(&cameraActor);
}

bool GameSample::handleKeyboard(const KeyboardEvent& event)
{
    if (event.type == Event::Type::KeyboardKeyPress)
    {
        switch (event.key)
        {
            case Keyboard::Key::Escape:
            case Keyboard::Key::Menu:
            case Keyboard::Key::Back:
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                return true;
            default:
                break;
        }
    }
    else if (event.type == Event::Type::KeyboardKeyRelease)
    {
        switch (event.key)
        {
            case Keyboard::Key::Escape:
            case Keyboard::Key::Menu:
            case Keyboard::Key::Back:
                return true;
            default:
                break;
        }
    }

    return false;
}

bool GameSample::handleMouse(const MouseEvent& event)
{
    if (event.type == Event::Type::MousePress)
    {

    }
    return false;
}

bool GameSample::handleTouch(const TouchEvent& event)
{
    if (event.type == Event::Type::TouchBegin)
    {

    }
    return false;
}

bool GameSample::handleGamepad(const GamepadEvent& event)
{
    if (event.type == Event::Type::GamepadButtonChange)
    {
        if (event.pressed &&
            event.button == Gamepad::Button::FaceRight)
            engine->getSceneManager().setScene(std::make_unique<MainMenu>());
    }

    return false;
}

bool GameSample::handleUI(const UIEvent&) const
{
    return false;
}
