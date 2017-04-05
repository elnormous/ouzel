// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "InputSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

InputSample::InputSample(Samples& aSamples):
    samples(aSamples)
{
    eventHandler.keyboardHandler = bind(&InputSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    eventHandler.mouseHandler = bind(&InputSample::handleMouse, this, placeholders::_1, placeholders::_2);
    eventHandler.touchHandler = bind(&InputSample::handleTouch, this, placeholders::_1, placeholders::_2);
    eventHandler.gamepadHandler = bind(&InputSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&InputSample::handleUI, this, placeholders::_1, placeholders::_2);

    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    camera = std::make_shared<scene::Camera>();
    camera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera->setTargetContentSize(Size2(800.0f, 600.0f));

    layer = std::make_shared<scene::Layer>();
    layer->addCamera(camera);
    addLayer(layer);

    flameParticleSystem = std::make_shared<scene::ParticleSystem>();
    flameParticleSystem->initFromFile("flame.json");

    flame = std::make_shared<scene::Node>();
    flame->addComponent(flameParticleSystem);
    flame->setPickable(false);
    layer->addChild(flame);

    guiCamera = std::make_shared<scene::Camera>();
    guiCamera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera->setTargetContentSize(Size2(800.0f, 600.0f));
    guiLayer = std::make_shared<scene::Layer>();
    guiLayer->addCamera(guiCamera);
    addLayer(guiLayer);

    guiLayer->addChild(menu);

    button = std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Show/hide", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK);
    button->setPosition(Vector2(-200.0f, 200.0f));

    menu = std::make_shared<gui::Menu>();
    menu->addWidget(button);

    backButton = std::make_shared<ouzel::gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK);
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton);
}

bool InputSample::handleKeyboard(Event::Type type, const KeyboardEvent& event)
{
    if (type == Event::Type::KEY_DOWN)
    {
        Vector2 position = camera->getPosition();
        Vector2 flamePosition = camera->convertWorldToNormalized(flame->getPosition());

        switch (event.key)
        {
            case input::KeyboardKey::UP:
                flamePosition.y() += 0.01f;
                break;
            case input::KeyboardKey::DOWN:
                flamePosition.y() -= 0.01f;
                break;
            case input::KeyboardKey::LEFT:
                flamePosition.x() -= 0.01f;
                break;
            case input::KeyboardKey::RIGHT:
                flamePosition.x() += 0.01f;
                break;
            case input::KeyboardKey::KEY_W:
                position.y() += 10.0f;
                break;
            case input::KeyboardKey::KEY_S:
                position.y() -= 10.0f;
                break;
            case input::KeyboardKey::KEY_A:
                position.x() -= 10.0f;
                break;
            case input::KeyboardKey::KEY_D:
                position.x() += 10.0f;
                break;
            case input::KeyboardKey::KEY_R:
                sharedEngine->getWindow()->setSize(Size2(640.0f, 480.0f));
                break;
            case input::KeyboardKey::TAB:
                button->setEnabled(!button->isEnabled());
                break;
            case input::KeyboardKey::ESCAPE:
                sharedEngine->getInput()->setCursorVisible(true);
                samples.setScene(std::shared_ptr<scene::Scene>(new MainMenu(samples)));
                return true;
            default:
                break;
        }

        camera->setPosition(position);

        Vector2 worldLocation = camera->convertNormalizedToWorld(flamePosition);

        flame->setPosition(worldLocation);
    }

    return true;
}

bool InputSample::handleMouse(Event::Type type, const MouseEvent& event)
{
    switch (type)
    {
        case Event::Type::MOUSE_MOVE:
        {
            Vector2 worldLocation = camera->convertNormalizedToWorld(event.position);
            flame->setPosition(worldLocation);
            break;
        }
        default:
            break;
    }

    return true;
}

bool InputSample::handleTouch(Event::Type, const TouchEvent& event)
{
    Vector2 worldLocation = camera->convertNormalizedToWorld(event.position);
    flame->setPosition(worldLocation);

    return true;
}

bool InputSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        Vector2 flamePosition = camera->convertWorldToNormalized(flame->getPosition());

        switch (event.button)
        {
            case input::GamepadButton::B:
                if (event.pressed) samples.setScene(std::shared_ptr<scene::Scene>(new MainMenu(samples)));
                return true;
            case input::GamepadButton::DPAD_UP:
            case input::GamepadButton::LEFT_THUMB_UP:
            case input::GamepadButton::RIGHT_THUMB_UP:
                flamePosition.y() = event.value / 2.0f + 0.5f;
                break;
            case input::GamepadButton::DPAD_DOWN:
            case input::GamepadButton::LEFT_THUMB_DOWN:
            case input::GamepadButton::RIGHT_THUMB_DOWN:
                flamePosition.y() = -event.value / 2.0f + 0.5f;
                break;
            case input::GamepadButton::DPAD_LEFT:
            case input::GamepadButton::LEFT_THUMB_LEFT:
            case input::GamepadButton::RIGHT_THUMB_LEFT:
                flamePosition.x() = -event.value / 2.0f + 0.5f;
                break;
            case input::GamepadButton::DPAD_RIGHT:
            case input::GamepadButton::LEFT_THUMB_RIGHT:
            case input::GamepadButton::RIGHT_THUMB_RIGHT:
                flamePosition.x() = event.value / 2.0f + 0.5f;
                break;
            default:
                break;
        }

        Vector2 worldLocation = camera->convertNormalizedToWorld(flamePosition);
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
            sharedEngine->getInput()->setCursorVisible(true);
            samples.setScene(std::shared_ptr<scene::Scene>(new MainMenu(samples)));
        }
        else if (event.node == button)
        {
            sharedEngine->getInput()->setCursorVisible(!sharedEngine->getInput()->isCursorVisible());
        }
    }

    return true;
}
