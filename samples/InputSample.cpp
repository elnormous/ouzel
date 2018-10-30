// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "InputSample.hpp"
#include "MainMenu.hpp"

using namespace std;
using namespace ouzel;
using namespace input;

class Mover: public scene::Component
{
public:
    Mover():
        scene::Component(10)
    {
        handler.keyboardHandler = bind(&Mover::handleKeyboard, this, placeholders::_1);

        engine->getEventDispatcher().addEventHandler(&handler);
    }

    bool handleKeyboard(const KeyboardEvent& event)
    {
        if (actor)
        {
            if (event.type == Event::Type::KEY_PRESS)
            {
                Vector2 position = Vector2(actor->getPosition());

                switch (event.key)
                {
                    case Keyboard::Key::W:
                        position.y += 10.0F;
                        break;
                    case Keyboard::Key::S:
                        position.y -= 10.0F;
                        break;
                    case Keyboard::Key::A:
                        position.x -= 10.0F;
                        break;
                    case Keyboard::Key::D:
                        position.x += 10.0F;
                        break;
                    default:
                        break;
                }

                actor->setPosition(position);
            }
        }

        return false;
    }

    ouzel::EventHandler handler;
};

InputSample::InputSample():
    cursor(*engine->getInputManager()),
    hideButton("button.png", "button_selected.png", "button_down.png", "", "Show/hide", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK),
    discoverButton("button.png", "button_selected.png", "button_down.png", "", "Discover gamepads", "arial.fnt", 0.8F, Color::BLACK, Color::BLACK, Color::BLACK),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK)
{
    cursor.init("cursor.png", Vector2(0.0F, 63.0F));

    handler.keyboardHandler = bind(&InputSample::handleKeyboard, this, placeholders::_1);
    handler.mouseHandler = bind(&InputSample::handleMouse, this, placeholders::_1);
    handler.touchHandler = bind(&InputSample::handleTouch, this, placeholders::_1);
    handler.gamepadHandler = bind(&InputSample::handleGamepad, this, placeholders::_1);
    handler.uiHandler = bind(&InputSample::handleUI, this, placeholders::_1);

    engine->getEventDispatcher().addEventHandler(&handler);

    camera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera.setTargetContentSize(Size2(800.0F, 600.0F));
    cameraActor.addComponent(&camera);

    std::unique_ptr<Mover> mover(new Mover());
    cameraActor.addComponent(std::move(mover));

    layer.addChild(&cameraActor);
    addLayer(&layer);

    flameParticleSystem.init("flame.json");

    flame.addComponent(&flameParticleSystem);
    flame.setPickable(false);
    layer.addChild(&flame);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2(800.0F, 600.0F));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    hideButton.setPosition(Vector2(-200.0F, 200.0F));
    menu.addWidget(&hideButton);

    discoverButton.setPosition(Vector2(-200.0F, 140.0F));
    menu.addWidget(&discoverButton);

    backButton.setPosition(Vector2(-200.0F, -200.0F));
    menu.addWidget(&backButton);

    if (Mouse* mouse = engine->getInputManager()->getMouse())
    {
        mouse->setCursor(&cursor);

        Vector2 worldLocation = Vector2(camera.convertNormalizedToWorld(mouse->getPosition()));
        flame.setPosition(worldLocation);
    }
}

bool InputSample::handleKeyboard(const KeyboardEvent& event)
{
    if (event.type == Event::Type::KEY_PRESS)
    {
        Vector2 flamePosition = camera.convertWorldToNormalized(flame.getPosition());

        switch (event.key)
        {
            case Keyboard::Key::UP:
                flamePosition.y += 0.01F;
                break;
            case Keyboard::Key::DOWN:
                flamePosition.y -= 0.01F;
                break;
            case Keyboard::Key::LEFT:
                flamePosition.x -= 0.01F;
                break;
            case Keyboard::Key::RIGHT:
                flamePosition.x += 0.01F;
                break;
            case Keyboard::Key::R:
                engine->getWindow()->setSize(Size2(640.0F, 480.0F));
                break;
            case Keyboard::Key::TAB:
                hideButton.setEnabled(!hideButton.isEnabled());
                break;
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
                if (Mouse* mouse = engine->getInputManager()->getMouse())
                    mouse->setCursorVisible(true);
                engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                return true;
            default:
                break;
        }

        Vector2 worldLocation = Vector2(camera.convertNormalizedToWorld(flamePosition));

        flame.setPosition(worldLocation);
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

bool InputSample::handleMouse(const MouseEvent& event)
{
    switch (event.type)
    {
        case Event::Type::MOUSE_MOVE:
        {
            Vector2 worldLocation = Vector2(camera.convertNormalizedToWorld(event.position));
            flame.setPosition(worldLocation);
            break;
        }
        default:
            break;
    }

    return false;
}

bool InputSample::handleTouch(const TouchEvent& event)
{
    Vector2 worldLocation = Vector2(camera.convertNormalizedToWorld(event.position));
    flame.setPosition(worldLocation);

    return false;
}

bool InputSample::handleGamepad(const GamepadEvent& event)
{
    if (event.type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        Vector2 flamePosition = camera.convertWorldToNormalized(flame.getPosition());

        switch (event.button)
        {
            case Gamepad::Button::FACE_RIGHT:
                if (event.pressed) engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                return false;
            case Gamepad::Button::DPAD_UP:
            case Gamepad::Button::LEFT_THUMB_UP:
            case Gamepad::Button::RIGHT_THUMB_UP:
                flamePosition.y = event.value / 2.0F + 0.5F;
                break;
            case Gamepad::Button::DPAD_DOWN:
            case Gamepad::Button::LEFT_THUMB_DOWN:
            case Gamepad::Button::RIGHT_THUMB_DOWN:
                flamePosition.y = -event.value / 2.0F + 0.5F;
                break;
            case Gamepad::Button::DPAD_LEFT:
            case Gamepad::Button::LEFT_THUMB_LEFT:
            case Gamepad::Button::RIGHT_THUMB_LEFT:
                flamePosition.x = -event.value / 2.0F + 0.5F;
                break;
            case Gamepad::Button::DPAD_RIGHT:
            case Gamepad::Button::LEFT_THUMB_RIGHT:
            case Gamepad::Button::RIGHT_THUMB_RIGHT:
                flamePosition.x = event.value / 2.0F + 0.5F;
                break;
            default:
                break;
        }

        Vector2 worldLocation = Vector2(camera.convertNormalizedToWorld(flamePosition));
        flame.setPosition(worldLocation);
    }

    return false;
}

bool InputSample::handleUI(const UIEvent& event) const
{
    if (event.type == Event::Type::ACTOR_CLICK)
    {
        if (event.actor == &backButton)
        {
            if (Mouse* mouse = engine->getInputManager()->getMouse())
                mouse->setCursorVisible(true);
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        }
        else if (event.actor == &hideButton)
        {
            if (Mouse* mouse = engine->getInputManager()->getMouse())
                mouse->setCursorVisible(!engine->getInputManager()->getMouse()->isCursorVisible());
        }
        else if (event.actor == &discoverButton)
            engine->getInputManager()->startDeviceDiscovery();
    }

    return false;
}
