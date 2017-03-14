// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AnimationsSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

AnimationsSample::AnimationsSample(Samples& aSamples):
    samples(aSamples),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK)
{
    eventHandler.gamepadHandler = bind(&AnimationsSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&AnimationsSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&AnimationsSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    camera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera.setTargetContentSize(Size2(800.0f, 600.0f));
    layer.addCamera(&camera);
    addLayer(&layer);

    shapeDrawable.rectangle(ouzel::Rectangle(100.0f, 100.0f), Color(0, 128, 128, 255), true);
    shapeDrawable.rectangle(ouzel::Rectangle(100.0f, 100.0f), Color::WHITE, false);
    shapeDrawable.line(Vector2(0.0f, 0.0f), Vector2(50.0f, 50.0f), Color::CYAN);
    shapeDrawable.point(Vector2(75.0f, 75.0f), Color::RED);

    shapeDrawable.curve({Vector2(50.0f, 50.0f),
                         Vector2(100.0f, 50.0f),
                         Vector2(50.0f, 0.0f),
                         Vector2(100.0f, 0.0f)},
                        Color::YELLOW);

    shapeDrawable.circle(Vector2(75.0f, 75.0f), 20.0f, Color::BLUE);
    shapeDrawable.circle(Vector2(25.0f, 75.0f), 20.0f, Color::BLUE, true);

    drawNode.addComponent(&shapeDrawable);
    drawNode.setPosition(Vector2(-300, 0.0f));
    drawNode.setParent(&layer);

    shake.reset(new scene::Shake(10.0f, Vector2(10.0f, 20.0f), 20.0f));
    drawNode.addComponent(shake.get());
    shake->start();

    witchSprite.initFromFile("witch.png");

    witch.setPosition(Vector2(200, 0.0f));
    witch.addComponent(&witchSprite);
    witch.setParent(&layer);

    witchScale.reset(new scene::Scale(2.0f, Vector2(0.1f, 0.1f), false));
    witchFade.reset(new scene::Fade(2.0f, 0.4f));

    vector<scene::Animator*> parallel = {
        witchScale.get(),
        witchFade.get()
    };

    witchRotate.reset(new scene::Rotate(1.0f, Vector3(0.0f, 0.0f, TAU), false));

    witchRepeat.reset(new scene::Repeat(witchRotate.get(), 3));
    witchParallel.reset(new scene::Parallel(parallel));

    vector<scene::Animator*> sequence = {
        witchRepeat.get(),
        witchParallel.get()
    };

    witchSequence.reset(new scene::Sequence(sequence));

    witch.addComponent(witchSequence.get());
    witchSequence->start();

    ballSprite.initFromFile("ball.png");
    ball.addComponent(&ballSprite);
    ball.setParent(&layer);

    ballDelay.reset(new scene::Animator(1.0f));
    ballMove.reset(new scene::Move(2.0f, Vector2(0.0f, -240.0f), false));
    ballEase.reset(new scene::Ease(ballMove.get(), scene::Ease::Type::OUT, scene::Ease::Func::BOUNCE));

    vector<scene::Animator*> sequence2 = {
        ballDelay.get(),
        ballEase.get()
    };
     
    ballSequence.reset(new scene::Sequence(sequence2));

    ball.addComponent(ballSequence.get());
    ballSequence->start();

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2(800.0f, 600.0f));
    guiLayer.addCamera(&guiCamera);
    addLayer(&guiLayer);

    menu.setParent(&guiLayer);

    backButton.setPosition(Vector2(-200.0f, -200.0f));
    menu.addWidget(&backButton);
}

bool AnimationsSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == input::GamepadButton::B)
        {
            samples.setScene(std::unique_ptr<scene::Scene>(new MainMenu(samples)));
        }
    }

    return true;
}

bool AnimationsSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE && event.node == &backButton)
    {
        samples.setScene(std::unique_ptr<scene::Scene>(new MainMenu(samples)));
    }

    return true;
}

bool AnimationsSample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
{
    if (type == Event::Type::KEY_DOWN)
    {
        switch (event.key)
        {
            case input::KeyboardKey::ESCAPE:
                samples.setScene(std::unique_ptr<scene::Scene>(new MainMenu(samples)));
                break;
            default:
                break;
        }
    }

    return true;
}
