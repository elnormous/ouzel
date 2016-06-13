// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "InputSample.h"

using namespace std;
using namespace ouzel;

InputSample::InputSample()
{
    eventHandler = make_shared<EventHandler>();

    eventHandler->keyboardHandler = bind(&InputSample::handleKeyboard, this, placeholders::_1, placeholders::_2, placeholders::_3);
    eventHandler->mouseHandler = bind(&InputSample::handleMouse, this, placeholders::_1, placeholders::_2, placeholders::_3);
    eventHandler->touchHandler = bind(&InputSample::handleTouch, this, placeholders::_1, placeholders::_2, placeholders::_3);
    eventHandler->gamepadHandler = bind(&InputSample::handleGamepad, this, placeholders::_1, placeholders::_2, placeholders::_3);
    eventHandler->uiHandler = bind(&InputSample::handleUI, this, placeholders::_1, placeholders::_2, placeholders::_3);

    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);
    
    scene::LayerPtr layer = make_shared<scene::Layer>();
    addLayer(layer);

    camera = make_shared<scene::Camera>();
    layer->setCamera(camera);

    button = gui::Button::create("button.png", "button.png", "button_down.png", "", "Show/hide", graphics::Color(0, 0, 0, 255), "arial.fnt");
    button->setPosition(Vector2(-200.0f, 200.0f));
    layer->addChild(button);

    scene::ParticleSystemPtr flameParticleSystem = scene::ParticleSystem::createFromFile("flame.json");
    
    flame = make_shared<scene::Node>();
    flame->addDrawable(flameParticleSystem);
    flame->setPickable(false);
    layer->addChild(flame);
}

InputSample::~InputSample()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
}

bool InputSample::handleKeyboard(Event::Type type, const KeyboardEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

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

bool InputSample::handleMouse(Event::Type type, const MouseEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

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

bool InputSample::handleTouch(Event::Type type, const TouchEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(type);
    OUZEL_UNUSED(sender);

    Vector2 worldLocation = camera->convertScreenToWorld(event.position);
    flame->setPosition(worldLocation);

    return true;
}

bool InputSample::handleGamepad(Event::Type type, const GamepadEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

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

bool InputSample::handleUI(Event::Type type, const UIEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(event);

    if (type == Event::Type::UI_CLICK_NODE && sender == button)
    {
        sharedEngine->getInput()->setCursorVisible(!sharedEngine->getInput()->isCursorVisible());
    }
    
    return true;
}
