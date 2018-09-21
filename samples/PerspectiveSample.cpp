// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "PerspectiveSample.hpp"
#include "MainMenu.hpp"

using namespace std;
using namespace ouzel;
using namespace audio;
using namespace graphics;

PerspectiveSample::PerspectiveSample():
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK)
{
    cursor.init(input::SystemCursor::CROSS);
    engine->getInputManager()->setCursor(&cursor);

    handler.keyboardHandler = bind(&PerspectiveSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    handler.mouseHandler = bind(&PerspectiveSample::handleMouse, this, placeholders::_1, placeholders::_2);
    handler.touchHandler = bind(&PerspectiveSample::handleTouch, this, placeholders::_1, placeholders::_2);
    handler.gamepadHandler = bind(&PerspectiveSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    handler.uiHandler = bind(&PerspectiveSample::handleUI, this, placeholders::_1, placeholders::_2);

    engine->getEventDispatcher().addEventHandler(&handler);

    engine->getRenderer()->setClearDepthBuffer(true);

    camera.setDepthTest(true);
    camera.setDepthWrite(true);

    camera.setProjectionMode(scene::Camera::ProjectionMode::PERSPECTIVE);
    camera.setFarPlane(1000.0F);
    cameraActor.setPosition(Vector3(0.0F, 0.0F, -400.0F));
    cameraActor.addComponent(&camera);
    layer.addChild(&cameraActor);
    addLayer(&layer);

    // floor
    floorSprite.init("floor.jpg");
    floorSprite.getMaterial()->textures[0]->setMaxAnisotropy(4);

    floor.addComponent(&floorSprite);
    layer.addChild(&floor);
    floor.setPosition(Vector2(0.0F, -50.0F));
    floor.setRotation(Vector3(TAU / 4.04F, TAU / 8.0F, 0.0F));

    // character
    characterSprite.init("run.json");
    characterSprite.setAnimation("", true);
    characterSprite.play();
    characterSprite.getMaterial()->textures[0]->setMaxAnisotropy(4);
    characterSprite.getMaterial()->cullMode = graphics::Renderer::CullMode::NONE;

    character.addComponent(&characterSprite);
    layer.addChild(&character);
    character.setPosition(Vector2(10.0F, 0.0F));

    cameraActor.addComponent(&listener);
    engine->getAudio()->addListener(&listener);

    jumpSound = Sound(engine->getCache().getSoundData("jump.wav"));
    jumpSound.setOutput(&listener);
    jumpSound.setRolloffFactor(0.01F);
    character.addComponent(&jumpSound);

    rotate.reset(new scene::Rotate(10.0F, Vector3(0.0F, TAU, 0.0F)));
    character.addComponent(rotate);
    rotate->start();

    boxModel.init(*engine->getCache().getMeshData("cube.obj"));
    box.addComponent(&boxModel);
    box.setPosition(Vector3(-160.0F, 0.0F, -50.0F));
    layer.addChild(&box);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2(800.0F, 600.0F));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    backButton.setPosition(Vector2(-200.0F, -200.0F));
    menu.addWidget(&backButton);
}

bool PerspectiveSample::handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event)
{
    if (type == Event::Type::ACTOR_CLICK)
    {
        if (event.actor == &backButton)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return false;
}

bool PerspectiveSample::handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event)
{
    if (type == Event::Type::KEY_PRESS ||
        type == Event::Type::KEY_REPEAT)
    {
        switch (event.key)
        {
            case input::Keyboard::Key::UP:
                cameraRotation.x -= TAU / 100.0F;
                break;
            case input::Keyboard::Key::DOWN:
                cameraRotation.x += TAU / 100.0F;
                break;
            case input::Keyboard::Key::LEFT:
                cameraRotation.y -= TAU / 100.0F;
                break;
            case input::Keyboard::Key::RIGHT:
                cameraRotation.y += TAU / 100.0F;
                break;
            case input::Keyboard::Key::ESCAPE:
            case input::Keyboard::Key::MENU:
                engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                return false;
            case input::Keyboard::Key::TAB:
                jumpSound.play();
                break;
            case input::Keyboard::Key::S:
                engine->getRenderer()->saveScreenshot("test.png");
                break;
            default:
                break;
        }

        if (cameraRotation.x < -TAU / 6.0F) cameraRotation.x = -TAU / 6.0F;
        if (cameraRotation.x > TAU / 6.0F) cameraRotation.x = TAU / 6.0F;

        cameraActor.setRotation(Vector3(cameraRotation.x, cameraRotation.y, 0.0F));

        //engine->getAudio()->setListenerRotation(camera.getRotation());
    }

    return false;
}

bool PerspectiveSample::handleMouse(ouzel::Event::Type type, const ouzel::MouseEvent& event)
{
    if (event.modifiers & LEFT_MOUSE_DOWN)
    {
        if (type == Event::Type::MOUSE_MOVE)
        {
            cameraRotation.x -= event.difference.y;
            cameraRotation.y -= event.difference.x;

            if (cameraRotation.x < -TAU / 6.0F) cameraRotation.x = -TAU / 6.0F;
            if (cameraRotation.x > TAU / 6.0F) cameraRotation.x = TAU / 6.0F;

            cameraActor.setRotation(Vector3(cameraRotation.x, cameraRotation.y, 0.0F));
        }
    }

    return false;
}

bool PerspectiveSample::handleTouch(ouzel::Event::Type type, const ouzel::TouchEvent& event)
{
    if (type == Event::Type::TOUCH_MOVE)
    {
        cameraRotation.x -= event.difference.y;
        cameraRotation.y -= event.difference.x;

        if (cameraRotation.x < -TAU / 6.0F) cameraRotation.x = -TAU / 6.0F;
        if (cameraRotation.x > TAU / 6.0F) cameraRotation.x = TAU / 6.0F;

        cameraActor.setRotation(Vector3(cameraRotation.x, cameraRotation.y, 0.0F));
    }

    return false;
}

bool PerspectiveSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == input::Gamepad::Button::FACE_RIGHT)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return false;
}
