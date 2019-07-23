// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "InputSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;

InputSample::InputSample():
    hideButton("button.png", "button_selected.png", "button_down.png", "", "Show/hide", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
    discoverButton("button.png", "button_selected.png", "button_down.png", "", "Discover gamepads", "Arial", 0.8F, Color::black(), Color::black(), Color::black()),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
    cursor(*engine->getInputManager())
{
    cursor.init("cursor.png", Vector2F(0.0F, 63.0F));

    handler.keyboardHandler = [this](const KeyboardEvent& event) {
        if (event.type == Event::Type::KeyboardKeyPress)
        {
            Vector2F flamePosition = camera.convertWorldToNormalized(flame.getPosition());

            switch (event.key)
            {
                case Keyboard::Key::Up:
                    flamePosition.y() += 0.01F;
                    break;
                case Keyboard::Key::Down:
                    flamePosition.y() -= 0.01F;
                    break;
                case Keyboard::Key::Left:
                    flamePosition.x() -= 0.01F;
                    break;
                case Keyboard::Key::Right:
                    flamePosition.x() += 0.01F;
                    break;
                case Keyboard::Key::R:
                    engine->getWindow()->setSize(Size2U(640, 480));
                    break;
                case Keyboard::Key::Tab:
                    hideButton.setEnabled(!hideButton.isEnabled());
                    break;
                case Keyboard::Key::Escape:
                case Keyboard::Key::Menu:
                case Keyboard::Key::Back:
                    if (Mouse* mouse = engine->getInputManager()->getMouse())
                        mouse->setCursorVisible(true);
                    engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                    return true;
                default:
                    break;
            }

            auto worldLocation = Vector2F(camera.convertNormalizedToWorld(flamePosition));

            flame.setPosition(worldLocation);
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
    };

    handler.mouseHandler = [this](const MouseEvent& event) {
        switch (event.type)
        {
            case Event::Type::MouseMove:
            {
                auto worldLocation = Vector2F(camera.convertNormalizedToWorld(event.position));
                flame.setPosition(worldLocation);
                break;
            }
            default:
                break;
        }

        return false;
    };

    handler.touchHandler = [this](const TouchEvent& event) {
        if (event.touchpad->isScreen())
        {
            auto worldLocation = Vector2F(camera.convertNormalizedToWorld(event.position));
            flame.setPosition(worldLocation);
        }

        return false;
    };

    handler.gamepadHandler = [this](const GamepadEvent& event) {
        if (event.type == Event::Type::GamepadButtonChange)
        {
            Vector2F flamePosition = camera.convertWorldToNormalized(flame.getPosition());

            switch (event.button)
            {
                case Gamepad::Button::FaceRight:
                    if (event.pressed) engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                    return false;
                case Gamepad::Button::DpadUp:
                case Gamepad::Button::LeftThumbUp:
                case Gamepad::Button::RightThumbUp:
                    flamePosition.y() = event.value / 2.0F + 0.5F;
                    break;
                case Gamepad::Button::DpadDown:
                case Gamepad::Button::LeftThumbDown:
                case Gamepad::Button::RightThumbDown:
                    flamePosition.y() = -event.value / 2.0F + 0.5F;
                    break;
                case Gamepad::Button::DpadLeft:
                case Gamepad::Button::LeftThumbLeft:
                case Gamepad::Button::RightThumbLeft:
                    flamePosition.x() = -event.value / 2.0F + 0.5F;
                    break;
                case Gamepad::Button::DpadRight:
                case Gamepad::Button::LeftThumbRight:
                case Gamepad::Button::RightThumbRight:
                    flamePosition.x() = event.value / 2.0F + 0.5F;
                    break;
                default:
                    break;
            }

            auto worldLocation = Vector2F(camera.convertNormalizedToWorld(flamePosition));
            flame.setPosition(worldLocation);
        }

        return false;
    };

    handler.uiHandler = [this](const UIEvent& event) {
        if (event.type == Event::Type::ActorClick)
        {
            if (event.actor == &backButton)
            {
                if (Mouse* mouse = engine->getInputManager()->getMouse())
                    mouse->setCursorVisible(true);
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());
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
    };

    engine->getEventDispatcher().addEventHandler(handler);

    camera.setClearColorBuffer(true);
    camera.setClearColor(ouzel::Color(64, 0, 0));
    camera.setScaleMode(scene::Camera::ScaleMode::ShowAll);
    camera.setTargetContentSize(Size2F(800.0F, 600.0F));
    cameraActor.addComponent(&camera);

    mover = std::make_unique<Mover>();
    cameraActor.addComponent(mover.get());

    layer.addChild(&cameraActor);
    addLayer(&layer);

    flameParticleSystem.init(*engine->getCache().getParticleSystemData("flame.json"));

    flame.addComponent(&flameParticleSystem);
    flame.setPickable(false);
    layer.addChild(&flame);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::ShowAll);
    guiCamera.setTargetContentSize(Size2F(800.0F, 600.0F));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    hideButton.setPosition(Vector2F(-200.0F, 200.0F));
    menu.addWidget(&hideButton);

    discoverButton.setPosition(Vector2F(-200.0F, 140.0F));
    menu.addWidget(&discoverButton);

    backButton.setPosition(Vector2F(-200.0F, -200.0F));
    menu.addWidget(&backButton);

    if (Mouse* mouse = engine->getInputManager()->getMouse())
    {
        mouse->setCursor(&cursor);

        auto worldLocation = Vector2F(camera.convertNormalizedToWorld(mouse->getPosition()));
        flame.setPosition(worldLocation);
    }
}
