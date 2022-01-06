// Ouzel by Elviss Strazdins

#include "core/Engine.hpp"
#include "scene/Animators.hpp"
#include "PerspectiveSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace audio;
using namespace graphics;
using namespace input;
using namespace math;

namespace samples
{
    PerspectiveSample::PerspectiveSample():
        submix{engine->getAudio()},
        listener{engine->getAudio()},
        jumpSubmix{engine->getAudio()},
        jumpVoice{engine->getAudio(), engine->getCache().getSound("jump.wav")},
        jumpPanner{engine->getAudio()},
        backButton{"button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, blackColor, blackColor, blackColor},
        cursor{engine->getInputManager()}
    {
        cursor.init(SystemCursor::cross);

        if (Mouse* mouse = engine->getInputManager().getMouse())
            mouse->setCursor(&cursor);

        handler.keyboardHandler = [this](const KeyboardEvent& event) {
            if (event.type == Event::Type::keyboardKeyPress)
            {
                switch (event.key)
                {
                    case Keyboard::Key::up:
                        cameraRotation.x() += tau<float> / 100.0F;
                        break;
                    case Keyboard::Key::down:
                        cameraRotation.x() -= tau<float> / 100.0F;
                        break;
                    case Keyboard::Key::left:
                        cameraRotation.y() += tau<float> / 100.0F;
                        break;
                    case Keyboard::Key::right:
                        cameraRotation.y() -= tau<float> / 100.0F;
                        break;
                    case Keyboard::Key::escape:
                    case Keyboard::Key::menu:
                    case Keyboard::Key::back:
                        engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                        return true;
                    case Keyboard::Key::tab:
                        jumpVoice.play();
                        break;
                    case Keyboard::Key::s:
                        engine->getGraphics().saveScreenshot("test.png");
                        break;
                    default:
                        break;
                }

                if (cameraRotation.x() < -tau<float> / 6.0F) cameraRotation.x() = -tau<float> / 6.0F;
                if (cameraRotation.x() > tau<float> / 6.0F) cameraRotation.x() = tau<float> / 6.0F;

                cameraActor.setRotation(Vector<float, 3>{cameraRotation.x(), cameraRotation.y(), 0.0F});
                updateArrowPosition();
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

        handler.mouseHandler = [this](const ouzel::MouseEvent& event) {
            if (event.type == Event::Type::mouseMove &&
                event.mouse->isButtonDown(Mouse::Button::left))
            {
                cameraRotation.x() += event.difference.y();
                cameraRotation.y() += event.difference.x();

                if (cameraRotation.x() < -tau<float> / 6.0F) cameraRotation.x() = -tau<float> / 6.0F;
                if (cameraRotation.x() > tau<float> / 6.0F) cameraRotation.x() = tau<float> / 6.0F;

                cameraActor.setRotation(Vector<float, 3>{cameraRotation.x(), cameraRotation.y(), 0.0F});
                updateArrowPosition();
            }

            return false;
        };

        handler.touchHandler = [this](const ouzel::TouchEvent& event) {
            if (event.touchpad->isScreen() &&
                event.type == Event::Type::touchMove)
            {
                cameraRotation.x() += event.difference.y();
                cameraRotation.y() += event.difference.x();

                if (cameraRotation.x() < -tau<float> / 6.0F) cameraRotation.x() = -tau<float> / 6.0F;
                if (cameraRotation.x() > tau<float> / 6.0F) cameraRotation.x() = tau<float> / 6.0F;

                cameraActor.setRotation(Vector<float, 3>{cameraRotation.x(), cameraRotation.y(), 0.0F});
                updateArrowPosition();
            }

            return false;
        };

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
            }

            return false;
        };

        handler.windowHandler = [this](const WindowEvent& event) {
            if (event.type == Event::Type::resolutionChange)
                updateArrowPosition();

            return false;
        };

        engine->getEventDispatcher().addEventHandler(handler);

        camera.setClearColorBuffer(true);
        camera.setClearColor(math::Color{0, 0, 128});
        camera.setClearDepthBuffer(true);
        camera.setDepthTest(true);

        camera.setProjectionMode(scene::Camera::ProjectionMode::perspective);
        camera.setNearPlane(1.0F);
        camera.setFarPlane(1000.0F);
        cameraActor.setPosition(Vector<float, 3>{0.0F, 0.0F, 400.0F});
        cameraActor.addComponent(camera);
        layer.addChild(cameraActor);
        addLayer(layer);

        // floor
        floorSprite.init("floor.jpg");
        floorSprite.getMaterial()->textures[0]->setMaxAnisotropy(4);

        floor.addComponent(floorSprite);
        layer.addChild(floor);
        floor.setPosition(Vector<float, 2>{0.0F, -50.0F});
        floor.setRotation(Vector<float, 3>{tau<float> / 4.0F, tau<float> / 8.0F, 0.0F});

        // character
        characterSprite.init("run.json");
        characterSprite.setAnimation("", true);
        characterSprite.play();
        characterSprite.getMaterial()->textures[0]->setMaxAnisotropy(4);

        character.addComponent(characterSprite);
        layer.addChild(character);
        character.setPosition(Vector<float, 2>{10.0F, 0.0F});

        // box
        boxModel.init(*engine->getCache().getStaticMeshData("cube.obj"));
        box.addComponent(boxModel);
        box.setPosition(Vector<float, 3>{-160.0F, 0.0F, -50.0F});
        layer.addChild(box);

        submix.setOutput(&engine->getAudio().getMasterMix());

        cameraActor.addComponent(listener);
        listener.setMix(&submix);
        submix.setOutput(&engine->getAudio().getMasterMix());

        jumpSubmix.setOutput(&submix);
        jumpVoice.setOutput(&jumpSubmix);
        jumpSubmix.addEffect(&jumpPanner);
        jumpPanner.setRolloffFactor(0.01F);
        character.addComponent(jumpPanner);

        rotate = std::make_unique<scene::Rotate>(10.0F, Vector<float, 3>{0.0F, tau<float>, 0.0F});
        character.addComponent(*rotate);
        rotate->start();

        guiCamera.setScaleMode(scene::Camera::ScaleMode::showAll);
        guiCamera.setTargetContentSize(math::Size<float, 2>{800.0F, 600.0F});
        guiCameraActor.addComponent(guiCamera);
        guiLayer.addChild(guiCameraActor);
        addLayer(guiLayer);

        // arrow
        arrowSprite.init("arrow");
        arrowSprite.setOffset(Vector<float, 2>{0.0F, -arrowSprite.getBoundingBox().min[1] + 20.0F});
        arrow.addComponent(arrowSprite);
        updateArrowPosition();
        guiLayer.addChild(arrow);

        guiLayer.addChild(menu);

        backButton.setPosition(Vector<float, 2>{-200.0F, -200.0F});
        menu.addWidget(backButton);
    }

    void PerspectiveSample::updateArrowPosition()
    {
        const auto worldPosition = box.convertLocalToWorld(Vector<float, 3>{0.0F, boxModel.getBoundingBox().max[1], 0.0F});
        const auto normalizedPosition = camera.convertWorldToNormalized(worldPosition);
        const auto guiPosition = guiCamera.convertNormalizedToWorld(normalizedPosition);
        arrow.setPosition(guiPosition);
    }
}
