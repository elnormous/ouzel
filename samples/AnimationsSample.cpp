// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AnimationsSample.hpp"
#include "MainMenu.hpp"

using namespace std;
using namespace ouzel;
using namespace input;

AnimationsSample::AnimationsSample():
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK)
{
    handler.gamepadHandler = bind(&AnimationsSample::handleGamepad, this, placeholders::_1);
    handler.uiHandler = bind(&AnimationsSample::handleUI, this, placeholders::_1);
    handler.keyboardHandler = bind(&AnimationsSample::handleKeyboard, this, placeholders::_1);
    engine->getEventDispatcher().addEventHandler(&handler);

    camera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera.setTargetContentSize(Size2(800.0F, 600.0F));
    cameraActor.addComponent(&camera);
    layer.addChild(&cameraActor);
    addLayer(&layer);

    shapeDrawable.rectangle(ouzel::Rect(100.0F, 100.0F), Color(0, 128, 128, 255), true);
    shapeDrawable.rectangle(ouzel::Rect(100.0F, 100.0F), Color::WHITE, false, 2.0F);
    shapeDrawable.line(Vector2(0.0F, 0.0F), Vector2(50.0F, 50.0F), Color::CYAN, 2.0F);

    shapeDrawable.curve({Vector2(50.0F, 50.0F),
                         Vector2(100.0F, 50.0F),
                         Vector2(50.0F, 0.0F),
                         Vector2(100.0F, 0.0F)},
                        Color::YELLOW);

    shapeDrawable.circle(Vector2(25.0F, 75.0F), 20.0F, Color::BLUE, true);
    shapeDrawable.circle(Vector2(25.0F, 75.0F), 20.0F, Color::WHITE, false);
    shapeDrawable.circle(Vector2(75.0F, 75.0F), 20.0F, Color::BLUE, false, 16, 4.0F);

    shapeDrawable.polygon({Vector2(15.0F, 75.0F),
                           Vector2(25.0F, 75.0F),
                           Vector2(25.0F, 55.0F)},
                          Color("#ff0000"), false);

    drawActor.addComponent(&shapeDrawable);
    drawActor.setPosition(Vector2(-300, 0.0F));
    layer.addChild(&drawActor);

    shake.reset(new scene::Shake(10.0F, Vector3(10.0F, 20.0F, 0.0F), 20.0F));
    drawActor.addComponent(shake);
    shake->start();

    witchSprite.init("witch.png");

    witch.setPosition(Vector2(200, 0.0F));
    witch.addComponent(&witchSprite);
    layer.addChild(&witch);

    witchScale.reset(new scene::Scale(2.0F, Vector3(0.1F, 0.1F, 0.0F), false));
    witchFade.reset(new scene::Fade(2.0F, 0.4F));

    vector<scene::Animator*> parallel = {
        witchScale.get(),
        witchFade.get()
    };

    witchRotate.reset(new scene::Rotate(1.0F, Vector3(0.0F, 0.0F, TAU), false));

    witchRepeat.reset(new scene::Repeat(witchRotate, 3));
    witchParallel.reset(new scene::Parallel(parallel));

    vector<scene::Animator*> sequence = {
        witchRepeat.get(),
        witchParallel.get()
    };

    witchSequence.reset(new scene::Sequence(sequence));

    witch.addComponent(witchSequence);
    witchSequence->start();

    ballSprite.init("ball.png");

    ball.addComponent(&ballSprite);
    layer.addChild(&ball);

    ballDelay.reset(new scene::Animator(1.0F));
    ballMove.reset(new scene::Move(2.0F, Vector3(0.0F, -240.0F, 0.0F), false));
    ballEase.reset(new scene::Ease(ballMove, scene::Ease::Mode::EASE_OUT, scene::Ease::Func::BOUNCE));

    vector<scene::Animator*> sequence2 = {
        ballDelay.get(),
        ballEase.get()
    };

    ballSequence.reset(new scene::Sequence(sequence2));

    ball.addComponent(ballSequence);
    ballSequence->start();

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2(800.0F, 600.0F));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    backButton.setPosition(Vector2(-200.0F, -200.0F));
    menu.addWidget(&backButton);
}

bool AnimationsSample::handleGamepad(const GamepadEvent& event)
{
    if (event.type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == Gamepad::Button::FACE_RIGHT)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return false;
}

bool AnimationsSample::handleUI(const UIEvent& event) const
{
    if (event.type == Event::Type::ACTOR_CLICK && event.actor == &backButton)
        engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));

    return false;
}

bool AnimationsSample::handleKeyboard(const KeyboardEvent& event) const
{
    if (event.type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
                engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                return true;
            default:
                break;
        }
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
