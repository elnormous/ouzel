// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GameSample.hpp"
#include "MainMenu.hpp"

using namespace std;
using namespace ouzel;

GameSample::GameSample()
{
    handler.keyboardHandler = bind(&GameSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    handler.mouseHandler = bind(&GameSample::handleMouse, this, placeholders::_1, placeholders::_2);
    handler.touchHandler = bind(&GameSample::handleTouch, this, placeholders::_1, placeholders::_2);
    handler.gamepadHandler = bind(&GameSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    handler.uiHandler = bind(&GameSample::handleUI, this, placeholders::_1, placeholders::_2);

    engine->getEventDispatcher().addEventHandler(&handler);

    addLayer(&layer);
    cameraActor.addComponent(&camera);
    layer.addChild(&cameraActor);
}

bool GameSample::handleKeyboard(Event::Type type, const KeyboardEvent& event)
{
    if (type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case input::KeyboardKey::ESCAPE:
            case input::KeyboardKey::MENU:
                engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                break;
            default:
                break;
        }
    }
    return false;
}

bool GameSample::handleMouse(Event::Type type, const MouseEvent&)
{
    if (type == Event::Type::MOUSE_PRESS)
    {

    }
    return false;
}

bool GameSample::handleTouch(Event::Type type, const TouchEvent&)
{
    if (type == Event::Type::TOUCH_BEGIN)
    {

    }
    return false;
}

bool GameSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == input::GamepadButton::FACE_RIGHT)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return false;
}

bool GameSample::handleUI(Event::Type, const UIEvent&) const
{
    return false;
}
