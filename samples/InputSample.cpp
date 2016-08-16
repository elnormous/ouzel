// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "InputSample.h"

using namespace std;
using namespace ouzel;

InputSample::InputSample(Samples& pSamples):
    samples(pSamples)
{
    eventHandler.keyboardHandler = bind(&InputSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    eventHandler.mouseHandler = bind(&InputSample::handleMouse, this, placeholders::_1, placeholders::_2);
    eventHandler.touchHandler = bind(&InputSample::handleTouch, this, placeholders::_1, placeholders::_2);
    eventHandler.gamepadHandler = bind(&InputSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&InputSample::handleUI, this, placeholders::_1, placeholders::_2);

    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);
    
    scene::LayerPtr layer = make_shared<scene::Layer>();
    addLayer(layer);

    camera = make_shared<scene::Camera>();
    layer->setCamera(camera);

    button = make_shared<gui::Button>("button.png", "button.png", "button_down.png", "", "Show/hide", graphics::Color(0, 0, 0, 255), "arial.fnt");
    button->setPosition(Vector2(-200.0f, 200.0f));
    layer->addChild(button);

    scene::ParticleSystemPtr flameParticleSystem = make_shared<scene::ParticleSystem>("flame.json");
    
    flame = make_shared<scene::Node>();
    flame->addComponent(flameParticleSystem);
    flame->setPickable(false);
    layer->addChild(flame);

    scene::LayerPtr guiLayer = make_shared<scene::Layer>();
    guiLayer->setCamera(make_shared<scene::Camera>());
    addLayer(guiLayer);

    backButton = make_shared<gui::Button>("button.png", "button.png", "button_down.png", "", "Back", graphics::Color(0, 0, 0, 255), "arial.fnt");
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    guiLayer->addChild(backButton);
}

InputSample::~InputSample()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
}

bool InputSample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
{
    if (type == Event::Type::KEY_DOWN)
    {
        Vector2 position = camera->getPosition();

        switch (event.key)
        {
            case input::KeyboardKey::UP:
                position.y += 10.0f;
                break;
            case input::KeyboardKey::DOWN:
                position.y -= 10.0f;
                break;
            case input::KeyboardKey::LEFT:
                position.x -= 10.0f;
                break;
            case input::KeyboardKey::RIGHT:
                position.x += 10.0f;
                break;
            case input::KeyboardKey::RETURN:
                sharedEngine->getWindow()->setSize(Size2(640.0f, 480.0f));
                break;
            case input::KeyboardKey::TAB:
                button->setEnabled(!button->isEnabled());
                break;
            default:
                break;
        }

        camera->setPosition(position);
    }

    return true;
}

bool InputSample::handleMouse(Event::Type type, const MouseEvent& event) const
{
    switch (type)
    {
        case Event::Type::MOUSE_MOVE:
        {
            Vector2 worldLocation = camera->convertScreenToWorld(event.position);
            flame->setPosition(worldLocation);
            break;
        }
        default:
            break;
    }

    return true;
}

bool InputSample::handleTouch(Event::Type, const TouchEvent& event) const
{
    Vector2 worldLocation = camera->convertScreenToWorld(event.position);
    flame->setPosition(worldLocation);

    return true;
}

bool InputSample::handleGamepad(Event::Type type, const GamepadEvent& event) const
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        Vector2 position = camera->convertWorldToScreen(flame->getPosition());

        switch (event.button)
        {
            case input::GamepadButton::DPAD_UP:
            case input::GamepadButton::LEFT_THUMB_UP:
            case input::GamepadButton::RIGHT_THUMB_UP:
                position.y = event.value;
                break;
            case input::GamepadButton::DPAD_DOWN:
            case input::GamepadButton::LEFT_THUMB_DOWN:
            case input::GamepadButton::RIGHT_THUMB_DOWN:
                position.y = -event.value;
                break;
            case input::GamepadButton::DPAD_LEFT:
            case input::GamepadButton::LEFT_THUMB_LEFT:
            case input::GamepadButton::RIGHT_THUMB_LEFT:
                position.x = -event.value;
                break;
            case input::GamepadButton::DPAD_RIGHT:
            case input::GamepadButton::LEFT_THUMB_RIGHT:
            case input::GamepadButton::RIGHT_THUMB_RIGHT:
                position.x = event.value;
                break;
            default:
                break;
        }

        Vector2 worldLocation = camera->convertScreenToWorld(position);
        flame->setPosition(worldLocation);
    }

    return true;
}

bool InputSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == backButton)
        {
            samples.back();
        }
        else if (event.node == button)
        {
            sharedEngine->getInput()->setCursorVisible(!sharedEngine->getInput()->isCursorVisible());
        }
    }
    
    return true;
}
