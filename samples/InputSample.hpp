// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef INPUTSAMPLE_HPP
#define INPUTSAMPLE_HPP

#include "ouzel.hpp"

class Mover: public ouzel::scene::Component
{
public:
    Mover():
        ouzel::scene::Component(10)
    {
        handler.keyboardHandler = std::bind(&Mover::handleKeyboard, this, std::placeholders::_1);

        ouzel::engine->getEventDispatcher().addEventHandler(handler);
    }

    bool handleKeyboard(const ouzel::KeyboardEvent& event)
    {
        if (actor)
        {
            if (event.type == ouzel::Event::Type::KeyboardKeyPress)
            {
                ouzel::Vector2F position = ouzel::Vector2F(actor->getPosition());

                switch (event.key)
                {
                    case ouzel::input::Keyboard::Key::W:
                        position.y() += 10.0F;
                        break;
                    case ouzel::input::Keyboard::Key::S:
                        position.y() -= 10.0F;
                        break;
                    case ouzel::input::Keyboard::Key::A:
                        position.x() -= 10.0F;
                        break;
                    case ouzel::input::Keyboard::Key::D:
                        position.x() += 10.0F;
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

class InputSample: public ouzel::scene::Scene
{
public:
    InputSample();

private:
    bool handleKeyboard(const ouzel::KeyboardEvent& event);
    bool handleMouse(const ouzel::MouseEvent& event);
    bool handleTouch(const ouzel::TouchEvent& event);
    bool handleGamepad(const ouzel::GamepadEvent& event);
    bool handleUI(const ouzel::UIEvent& event) const;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;
    ouzel::scene::Actor cameraActor;

    ouzel::EventHandler handler;

    ouzel::gui::Button hideButton;
    ouzel::gui::Button discoverButton;
    ouzel::scene::ParticleSystem flameParticleSystem;
    ouzel::scene::Actor flame;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::scene::Actor guiCameraActor;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;

    ouzel::input::Cursor cursor;

    std::unique_ptr<Mover> mover;
};

#endif // INPUTSAMPLE_HPP
