// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "PerspectiveSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace audio;
using namespace graphics;
using namespace input;

PerspectiveSample::PerspectiveSample():
    submix(*engine->getAudio()),
    listener(*engine->getAudio()),
    jumpSubmix(*engine->getAudio()),
    jumpVoice(*engine->getAudio(), engine->getCache().getSound("jump.wav")),
    jumpPanner(*engine->getAudio()),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
    cursor(*engine->getInputManager())
{
    cursor.init(SystemCursor::Cross);

    if (Mouse* mouse = engine->getInputManager()->getMouse())
        mouse->setCursor(&cursor);

    handler.keyboardHandler = [this](const KeyboardEvent& event) {
        if (event.type == Event::Type::KeyboardKeyPress)
        {
            switch (event.key)
            {
                case Keyboard::Key::Up:
                    cameraRotation.x() -= tau<float> / 100.0F;
                    break;
                case Keyboard::Key::Down:
                    cameraRotation.x() += tau<float> / 100.0F;
                    break;
                case Keyboard::Key::Left:
                    cameraRotation.y() -= tau<float> / 100.0F;
                    break;
                case Keyboard::Key::Right:
                    cameraRotation.y() += tau<float> / 100.0F;
                    break;
                case Keyboard::Key::Escape:
                case Keyboard::Key::Menu:
                case Keyboard::Key::Back:
                    engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                    return true;
                case Keyboard::Key::Tab:
                    jumpVoice.play();
                    break;
                case Keyboard::Key::S:
                    engine->getRenderer()->saveScreenshot("test.png");
                    break;
                default:
                    break;
            }

            if (cameraRotation.x() < -tau<float> / 6.0F) cameraRotation.x() = -tau<float> / 6.0F;
            if (cameraRotation.x() > tau<float> / 6.0F) cameraRotation.x() = tau<float> / 6.0F;

            cameraActor.setRotation(Vector3F(cameraRotation.x(), cameraRotation.y(), 0.0F));
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

    handler.mouseHandler = [this](const ouzel::MouseEvent& event) {
        if (event.type == Event::Type::MouseMove &&
            event.mouse->isButtonDown(Mouse::Button::Left))
        {
            cameraRotation.x() -= event.difference.y();
            cameraRotation.y() -= event.difference.x();

            if (cameraRotation.x() < -tau<float> / 6.0F) cameraRotation.x() = -tau<float> / 6.0F;
            if (cameraRotation.x() > tau<float> / 6.0F) cameraRotation.x() = tau<float> / 6.0F;

            cameraActor.setRotation(Vector3F(cameraRotation.x(), cameraRotation.y(), 0.0F));
        }

        return false;
    };

    handler.touchHandler = [this](const ouzel::TouchEvent& event) {
        if (event.touchpad->isScreen() &&
            event.type == Event::Type::TouchMove)
        {
            cameraRotation.x() -= event.difference.y();
            cameraRotation.y() -= event.difference.x();

            if (cameraRotation.x() < -tau<float> / 6.0F) cameraRotation.x() = -tau<float> / 6.0F;
            if (cameraRotation.x() > tau<float> / 6.0F) cameraRotation.x() = tau<float> / 6.0F;

            cameraActor.setRotation(Vector3F(cameraRotation.x(), cameraRotation.y(), 0.0F));
        }

        return false;
    };

    handler.gamepadHandler = [](const GamepadEvent& event) {
        if (event.type == Event::Type::GamepadButtonChange)
        {
            if (event.pressed &&
                event.button == Gamepad::Button::FaceRight)
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());
        }

        return false;
    };

    handler.uiHandler = [this](const UIEvent& event) {
        if (event.type == Event::Type::ActorClick)
        {
            if (event.actor == &backButton)
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());
        }

        return false;
    };

    engine->getEventDispatcher().addEventHandler(handler);

    camera.setClearColorBuffer(true);
    camera.setClearColor(ouzel::Color(0, 0, 128));
    camera.setClearDepthBuffer(true);
    camera.setDepthTest(true);

    camera.setProjectionMode(scene::Camera::ProjectionMode::Perspective);
    camera.setFarPlane(1000.0F);
    cameraActor.setPosition(Vector3F(0.0F, 0.0F, -400.0F));
    cameraActor.addComponent(&camera);
    layer.addChild(&cameraActor);
    addLayer(&layer);

    // floor
    floorSprite.init("floor.jpg");
    floorSprite.getMaterial()->textures[0]->setMaxAnisotropy(4);

    floor.addComponent(&floorSprite);
    layer.addChild(&floor);
    floor.setPosition(Vector2F(0.0F, -50.0F));
    floor.setRotation(Vector3F(tau<float> / 4.04F, tau<float> / 8.0F, 0.0F));

    // character
    characterSprite.init("run.json");
    characterSprite.setAnimation("", true);
    characterSprite.play();
    characterSprite.getMaterial()->textures[0]->setMaxAnisotropy(4);
    characterSprite.getMaterial()->cullMode = graphics::CullMode::NoCull;

    character.addComponent(&characterSprite);
    layer.addChild(&character);
    character.setPosition(Vector2F(10.0F, 0.0F));

    submix.setOutput(&engine->getAudio()->getMasterMix());
    
    cameraActor.addComponent(&listener);
    listener.setMix(&submix);
    submix.setOutput(&engine->getAudio()->getMasterMix());

    jumpSubmix.setOutput(&submix);
    jumpVoice.setOutput(&jumpSubmix);
    //jumpSubmix.addEffect(&jumpPanner);
    jumpPanner.setRolloffFactor(0.01F);
    character.addComponent(&jumpPanner);

    rotate = std::make_unique<scene::Rotate>(10.0F, Vector3F(0.0F, tau<float>, 0.0F));
    character.addComponent(rotate.get());
    rotate->start();

    boxModel.init(*engine->getCache().getStaticMeshData("cube.obj"));
    box.addComponent(&boxModel);
    box.setPosition(Vector3F(-160.0F, 0.0F, -50.0F));
    layer.addChild(&box);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::ShowAll);
    guiCamera.setTargetContentSize(Size2F(800.0F, 600.0F));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    backButton.setPosition(Vector2F(-200.0F, -200.0F));
    menu.addWidget(&backButton);
}
