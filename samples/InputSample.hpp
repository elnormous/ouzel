#ifndef INPUTSAMPLE_HPP
#define INPUTSAMPLE_HPP

#include "core/Engine.hpp"
#include "gui/Widgets.hpp"
#include "input/Cursor.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class Mover: public fluxion::scene::Component
    {
    public:
        Mover()
        {
            handler.keyboardHandler = [this](const fluxion::KeyboardEvent& event) {
                if (actor)
                {
                    if (event.type == fluxion::Event::Type::keyboardKeyPress)
                    {
                        fluxion::math::Vector<float, 2> position = fluxion::math::Vector<float, 2>(actor->getPosition());

                        switch (event.key)
                        {
                            case fluxion::input::Keyboard::Key::w:
                                position.y() += 10.0F;
                                break;
                            case fluxion::input::Keyboard::Key::s:
                                position.y() -= 10.0F;
                                break;
                            case fluxion::input::Keyboard::Key::a:
                                position.x() -= 10.0F;
                                break;
                            case fluxion::input::Keyboard::Key::d:
                                position.x() += 10.0F;
                                break;
                            default:
                                break;
                        }

                        actor->setPosition(position);
                    }
                }

                return false;
            };

            fluxion::engine->getEventDispatcher().addEventHandler(handler);
        }

        fluxion::EventHandler handler;
    };

    class InputSample: public fluxion::scene::Scene
    {
    public:
        InputSample();

    private:
        fluxion::scene::Layer layer;
        fluxion::scene::Camera camera;
        fluxion::scene::Actor cameraActor;

        fluxion::EventHandler handler;

        fluxion::gui::Button hideButton;
        fluxion::gui::Button discoverButton;
        fluxion::scene::ParticleSystem flameParticleSystem;
        fluxion::scene::Actor flame;

        fluxion::scene::Layer guiLayer;
        fluxion::scene::Camera guiCamera;
        fluxion::scene::Actor guiCameraActor;
        fluxion::gui::Menu menu;
        fluxion::gui::Button backButton;

        fluxion::input::Cursor cursor;

        Mover mover;
    };
}

#endif // INPUTSAMPLE_HPP
