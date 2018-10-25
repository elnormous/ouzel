// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GameSample.hpp"
#include "MainMenu.hpp"

using namespace std;
using namespace ouzel;
using namespace input;

GameSample::GameSample()
{
    handler.keyboardHandler = bind(&GameSample::handleKeyboard, this, placeholders::_1);
    handler.mouseHandler = bind(&GameSample::handleMouse, this, placeholders::_1);
    handler.touchHandler = bind(&GameSample::handleTouch, this, placeholders::_1);
    handler.gamepadHandler = bind(&GameSample::handleGamepad, this, placeholders::_1);
    handler.uiHandler = bind(&GameSample::handleUI, this, placeholders::_1);

    engine->getEventDispatcher().addEventHandler(&handler);

    addLayer(&layer);
    cameraActor.addComponent(&camera);
    layer.addChild(&cameraActor);
}

bool GameSample::handleKeyboard(const KeyboardEvent& event)
{
    if (event.type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
                engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                return true;
            default:
                break;
        }
    }
    else if (event.type == Event::Type::KEY_RELEASE)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
                return true;
            default:
                break;
        }
    }

    return false;
}

bool GameSample::handleMouse(const MouseEvent& event)
{
    if (event.type == Event::Type::MOUSE_PRESS)
    {

    }
    return false;
}

bool GameSample::handleTouch(const TouchEvent& event)
{
    if (event.type == Event::Type::TOUCH_BEGIN)
    {

    }
    return false;
}

bool GameSample::handleGamepad(const GamepadEvent& event)
{
    if (event.type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == Gamepad::Button::FACE_RIGHT)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return false;
}

bool GameSample::handleUI(const UIEvent&) const
{
    return false;
}
