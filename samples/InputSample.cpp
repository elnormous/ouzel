// Ouzel by Elviss Strazdins

#include "core/Engine.hpp"
#include "InputSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;

namespace samples
{
    InputSample::InputSample():
        hideButton("button.png", "button_selected.png", "button_down.png", "", "Show/hide", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
        discoverButton("button.png", "button_selected.png", "button_down.png", "", "Discover gamepads", "Arial", 0.8F, Color::black(), Color::black(), Color::black()),
        backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
        cursor(*engine->getInputManager())
    {
        cursor.init("cursor.png", Vector<float, 2>(0.0F, 63.0F));

        handler.keyboardHandler = [this](const KeyboardEvent& event) {
            if (event.type == Event::Type::keyboardKeyPress)
            {
                Vector<float, 2> flamePosition = camera.convertWorldToNormalized(flame.getPosition());

                switch (event.key)
                {
                    case Keyboard::Key::up:
                        flamePosition.y() += 0.01F;
                        break;
                    case Keyboard::Key::down:
                        flamePosition.y() -= 0.01F;
                        break;
                    case Keyboard::Key::left:
                        flamePosition.x() -= 0.01F;
                        break;
                    case Keyboard::Key::right:
                        flamePosition.x() += 0.01F;
                        break;
                    case Keyboard::Key::r:
                        engine->getWindow()->setSize(Size<std::uint32_t, 2>(640, 480));
                        break;
                    case Keyboard::Key::tab:
                        hideButton.setEnabled(!hideButton.isEnabled());
                        break;
                    case Keyboard::Key::escape:
                    case Keyboard::Key::menu:
                    case Keyboard::Key::back:
                        if (Mouse* mouse = engine->getInputManager()->getMouse())
                            mouse->setCursorVisible(true);
                        engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                        return true;
                    default:
                        break;
                }

                auto worldLocation = Vector<float, 2>(camera.convertNormalizedToWorld(flamePosition));

                flame.setPosition(worldLocation);
            }
            else if (event.type == Event::Type::keyboardKeyRelease)
            {
                switch (event.key)
                {
                    case Keyboard::Key::escape:
                    case Keyboard::Key::menu:
                    case Keyboard::Key::back:
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
                case Event::Type::mouseMove:
                {
                    auto worldLocation = Vector<float, 2>(camera.convertNormalizedToWorld(event.position));
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
                auto worldLocation = Vector<float, 2>(camera.convertNormalizedToWorld(event.position));
                flame.setPosition(worldLocation);
            }

            return false;
        };

        handler.gamepadHandler = [this](const GamepadEvent& event) {
            if (event.type == Event::Type::gamepadButtonChange)
            {
                Vector<float, 2> flamePosition = camera.convertWorldToNormalized(flame.getPosition());

                switch (event.button)
                {
                    case Gamepad::Button::faceRight:
                        if (event.pressed) engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                        return false;
                    case Gamepad::Button::dPadUp:
                    case Gamepad::Button::leftThumbUp:
                    case Gamepad::Button::rightThumbUp:
                        flamePosition.y() = event.value / 2.0F + 0.5F;
                        break;
                    case Gamepad::Button::dPadDown:
                    case Gamepad::Button::leftThumbDown:
                    case Gamepad::Button::rightThumbDown:
                        flamePosition.y() = -event.value / 2.0F + 0.5F;
                        break;
                    case Gamepad::Button::dPadLeft:
                    case Gamepad::Button::leftThumbLeft:
                    case Gamepad::Button::rightThumbLeft:
                        flamePosition.x() = -event.value / 2.0F + 0.5F;
                        break;
                    case Gamepad::Button::dPadRight:
                    case Gamepad::Button::leftThumbRight:
                    case Gamepad::Button::rightThumbRight:
                        flamePosition.x() = event.value / 2.0F + 0.5F;
                        break;
                    default:
                        break;
                }

                auto worldLocation = Vector<float, 2>(camera.convertNormalizedToWorld(flamePosition));
                flame.setPosition(worldLocation);
            }

            return false;
        };

        handler.uiHandler = [this](const UIEvent& event) {
            if (event.type == Event::Type::actorClick)
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
        camera.setScaleMode(scene::Camera::ScaleMode::showAll);
        camera.setTargetContentSize(Size<float, 2>(800.0F, 600.0F));
        cameraActor.addComponent(camera);

        mover = std::make_unique<Mover>();
        cameraActor.addComponent(*mover);

        layer.addChild(cameraActor);
        addLayer(layer);

        flameParticleSystem.init(*engine->getCache().getParticleSystemData("flame.json"));

        flame.addComponent(flameParticleSystem);
        flame.setPickable(false);
        layer.addChild(flame);

        guiCamera.setScaleMode(scene::Camera::ScaleMode::showAll);
        guiCamera.setTargetContentSize(Size<float, 2>(800.0F, 600.0F));
        guiCameraActor.addComponent(guiCamera);
        guiLayer.addChild(guiCameraActor);
        addLayer(guiLayer);

        guiLayer.addChild(menu);

        hideButton.setPosition(Vector<float, 2>(-200.0F, 200.0F));
        menu.addWidget(hideButton);

        discoverButton.setPosition(Vector<float, 2>(-200.0F, 140.0F));
        menu.addWidget(discoverButton);

        backButton.setPosition(Vector<float, 2>(-200.0F, -200.0F));
        menu.addWidget(backButton);

        if (Mouse* mouse = engine->getInputManager()->getMouse())
        {
            mouse->setCursor(&cursor);

            auto worldLocation = Vector<float, 2>(camera.convertNormalizedToWorld(mouse->getPosition()));
            flame.setPosition(worldLocation);
        }
    }
}
