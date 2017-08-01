// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GameSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

GameSample::GameSample()
{
    eventHandler.keyboardHandler = bind(&GameSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    eventHandler.mouseHandler = bind(&GameSample::handleMouse, this, placeholders::_1, placeholders::_2);
    eventHandler.touchHandler = bind(&GameSample::handleTouch, this, placeholders::_1, placeholders::_2);
    eventHandler.gamepadHandler = bind(&GameSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&GameSample::handleUI, this, placeholders::_1, placeholders::_2);

    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    addLayer(layer.get());
    layer->addChild(camera.get());
}

bool GameSample::handleKeyboard(Event::Type type, const KeyboardEvent& event)
{
    if (type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case input::KeyboardKey::ESCAPE:
                sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                break;
            default:
                break;
        }
    }
    return true;
}

bool GameSample::handleMouse(Event::Type type, const MouseEvent&)
{
    if (type == Event::Type::MOUSE_PRESS)
    {

    }
    return true;
}

bool GameSample::handleTouch(Event::Type type, const TouchEvent&)
{
    if (type == Event::Type::TOUCH_BEGIN)
    {

    }
    return true;
}

bool GameSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == input::GamepadButton::FACE_RIGHT)
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        }
    }

    return true;
}

bool GameSample::handleUI(Event::Type, const UIEvent&) const
{
    return true;
}
