// Ouzel by Elviss Strazdins

#include "core/Engine.hpp"
#include "scene/Animators.hpp"
#include "SpritesSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;
using namespace math;

namespace samples
{
    SpritesSample::SpritesSample():
        hideButton{"button.png", "button_selected.png", "button_down.png", "", "Show/hide", "Arial", 1.0F, blackColor(), blackColor(), blackColor()},
        wireframeButton{"button.png", "button_selected.png", "button_down.png", "", "Wireframe", "Arial", 1.0F, blackColor(), blackColor(), blackColor()},
        backButton{"button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, blackColor(), blackColor(), blackColor()}
    {
        handler.gamepadHandler = [](const GamepadEvent& event) {
            if (event.type == Event::Type::gamepadButtonChange)
            {
                if (event.pressed &&
                    event.button == Gamepad::Button::faceRight)
                    engine->getSceneManager().setScene(std::make_unique<MainMenu>());
            }

            return false;
        };

        handler.uiHandler = [this](const UIEvent& event) {
            if (event.type == Event::Type::actorClick)
            {
                if (event.actor == &backButton)
                    engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                else if (event.actor == &hideButton)
                    character.setHidden(!character.isHidden());
                else if (event.actor == &wireframeButton)
                    camera.setWireframe(!camera.getWireframe());
            }

            return false;
        };

        handler.keyboardHandler = [](const KeyboardEvent& event) {
            if (event.type == Event::Type::keyboardKeyPress)
            {
                switch (event.key)
                {
                    case Keyboard::Key::escape:
                    case Keyboard::Key::menu:
                    case Keyboard::Key::back:
                        engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                        return true;
                    default:
                        break;
                }
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

        engine->getEventDispatcher().addEventHandler(handler);

        camera.setClearColorBuffer(true);
        camera.setClearColor(math::Color{64, 64, 64});
        camera.setScaleMode(scene::Camera::ScaleMode::showAll);
        camera.setTargetContentSize(math::Size<float, 2>{800.0F, 600.0F});
        cameraActor.addComponent(camera);
        layer.addChild(cameraActor);
        addLayer(layer);

        // character
        characterSprite.init("run.json");
        characterSprite.setAnimation("", true);
        characterSprite.play();

        character.addComponent(characterSprite);
        layer.addChild(character);
        character.setPosition(Vector<float, 2>{-300.0F, 0.0F});

        move = std::make_unique<scene::Move>(4.0F, Vector<float, 3>{300.0F, 0.0F, 0.0F});
        character.addComponent(*move);
        move->start();

        // fire
        fireSprite.init("fire.json");
        fireSprite.setOffset(Vector<float, 2>{0.0F, 20.0F});
        fireSprite.setAnimation("", true);
        fireSprite.play();

        fireActor.addComponent(fireSprite);
        fireActor.setPosition(Vector<float, 2>{-100.0F, -140.0F});
        layer.addChild(fireActor);

        // triangle
        triangleSprite.init("triangle.json");

        triangleActor.addComponent(triangleSprite);
        triangleActor.setPosition(Vector<float, 2>{100.0F, -140.0F});
        layer.addChild(triangleActor);

        guiCamera.setScaleMode(scene::Camera::ScaleMode::showAll);
        guiCamera.setTargetContentSize(math::Size<float, 2>{800.0F, 600.0F});
        guiCameraActor.addComponent(guiCamera);
        guiLayer.addChild(guiCameraActor);
        addLayer(guiLayer);

        guiLayer.addChild(menu);

        hideButton.setPosition(Vector<float, 2>{-200.0F, 200.0F});
        menu.addWidget(hideButton);

        wireframeButton.setPosition(Vector<float, 2>{-200.0F, 160.0F});
        menu.addWidget(wireframeButton);

        backButton.setPosition(Vector<float, 2>{-200.0F, -200.0F});
        menu.addWidget(backButton);
    }
}
