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
    cursor.init(SystemCursor::CROSS);

    if (Mouse* mouse = engine->getInputManager()->getMouse())
        mouse->setCursor(&cursor);

    handler.keyboardHandler = std::bind(&PerspectiveSample::handleKeyboard, this, std::placeholders::_1);
    handler.mouseHandler = std::bind(&PerspectiveSample::handleMouse, this, std::placeholders::_1);
    handler.touchHandler = std::bind(&PerspectiveSample::handleTouch, this, std::placeholders::_1);
    handler.gamepadHandler = std::bind(&PerspectiveSample::handleGamepad, this, std::placeholders::_1);
    handler.uiHandler = std::bind(&PerspectiveSample::handleUI, this, std::placeholders::_1);

    engine->getEventDispatcher().addEventHandler(handler);

    camera.setClearColorBuffer(true);
    camera.setClearColor(ouzel::Color(0, 0, 128));
    camera.setClearDepthBuffer(true);
    camera.setDepthTest(true);

    camera.setProjectionMode(scene::Camera::ProjectionMode::PERSPECTIVE);
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
    floor.setRotation(Vector3F(tau<float>() / 4.04F, tau<float>() / 8.0F, 0.0F));

    // character
    characterSprite.init("run.json");
    characterSprite.setAnimation("", true);
    characterSprite.play();
    characterSprite.getMaterial()->textures[0]->setMaxAnisotropy(4);
    characterSprite.getMaterial()->cullMode = graphics::CullMode::NONE;

    character.addComponent(&characterSprite);
    layer.addChild(&character);
    character.setPosition(Vector2F(10.0F, 0.0F));

    submix.setOutput(&engine->getAudio()->getMasterMix());
    
    cameraActor.addComponent(&listener);
    listener.setMix(&submix);
    submix.setOutput(&engine->getAudio()->getMasterMix());

    jumpSubmix.setOutput(&submix);
    jumpVoice.setOutput(&jumpSubmix);
    jumpSubmix.addEffect(&jumpPanner);
    jumpPanner.setRolloffFactor(0.01F);
    character.addComponent(&jumpPanner);

    rotate.reset(new scene::Rotate(10.0F, Vector3F(0.0F, tau<float>(), 0.0F)));
    character.addComponent(rotate.get());
    rotate->start();

    boxModel.init(*engine->getCache().getStaticMeshData("cube.obj"));
    box.addComponent(&boxModel);
    box.setPosition(Vector3F(-160.0F, 0.0F, -50.0F));
    layer.addChild(&box);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2F(800.0F, 600.0F));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    backButton.setPosition(Vector2F(-200.0F, -200.0F));
    menu.addWidget(&backButton);
}

bool PerspectiveSample::handleUI(const ouzel::UIEvent& event)
{
    if (event.type == Event::Type::ACTOR_CLICK)
    {
        if (event.actor == &backButton)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return false;
}

bool PerspectiveSample::handleKeyboard(const ouzel::KeyboardEvent& event)
{
    if (event.type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case Keyboard::Key::UP:
                cameraRotation.x() -= tau<float>() / 100.0F;
                break;
            case Keyboard::Key::DOWN:
                cameraRotation.x() += tau<float>() / 100.0F;
                break;
            case Keyboard::Key::LEFT:
                cameraRotation.y() -= tau<float>() / 100.0F;
                break;
            case Keyboard::Key::RIGHT:
                cameraRotation.y() += tau<float>() / 100.0F;
                break;
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
            case Keyboard::Key::BACK:
                engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                return true;
            case Keyboard::Key::TAB:
                jumpVoice.play();
                break;
            case Keyboard::Key::S:
                engine->getRenderer()->saveScreenshot("test.png");
                break;
            default:
                break;
        }

        if (cameraRotation.x() < -tau<float>() / 6.0F) cameraRotation.x() = -tau<float>() / 6.0F;
        if (cameraRotation.x() > tau<float>() / 6.0F) cameraRotation.x() = tau<float>() / 6.0F;

        cameraActor.setRotation(Vector3F(cameraRotation.x(), cameraRotation.y(), 0.0F));
    }
    else if (event.type == Event::Type::KEY_RELEASE)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
            case Keyboard::Key::BACK:
                return true;
            default:
                break;
        }
    }

    return false;
}

bool PerspectiveSample::handleMouse(const ouzel::MouseEvent& event)
{
    if (event.type == Event::Type::MOUSE_MOVE &&
        event.mouse->isButtonDown(Mouse::Button::LEFT))
    {
        cameraRotation.x() -= event.difference.y();
        cameraRotation.y() -= event.difference.x();

        if (cameraRotation.x() < -tau<float>() / 6.0F) cameraRotation.x() = -tau<float>() / 6.0F;
        if (cameraRotation.x() > tau<float>() / 6.0F) cameraRotation.x() = tau<float>() / 6.0F;

        cameraActor.setRotation(Vector3F(cameraRotation.x(), cameraRotation.y(), 0.0F));
    }

    return false;
}

bool PerspectiveSample::handleTouch(const ouzel::TouchEvent& event)
{
    if (event.touchpad->isScreen() &&
        event.type == Event::Type::TOUCH_MOVE)
    {
        cameraRotation.x() -= event.difference.y();
        cameraRotation.y() -= event.difference.x();

        if (cameraRotation.x() < -tau<float>() / 6.0F) cameraRotation.x() = -tau<float>() / 6.0F;
        if (cameraRotation.x() > tau<float>() / 6.0F) cameraRotation.x() = tau<float>() / 6.0F;

        cameraActor.setRotation(Vector3F(cameraRotation.x(), cameraRotation.y(), 0.0F));
    }

    return false;
}

bool PerspectiveSample::handleGamepad(const GamepadEvent& event)
{
    if (event.type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == Gamepad::Button::FACE_RIGHT)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return false;
}
