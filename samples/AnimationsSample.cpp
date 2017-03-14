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

    shapeDrawable.setNode(&drawNode);
    drawNode.setPosition(Vector2(-300, 0.0f));
    drawNode.setParent(&layer);

    shake.reset(new scene::Shake(10.0f, Vector2(10.0f, 20.0f), 20.0f));
    shake->setNode(&drawNode);
    shake->start();

    witchSprite.initFromFile("witch.png");

    witch.setPosition(Vector2(200, 0.0f));
    witchSprite.setNode(&witch);
    witch.setParent(&layer);

    witchScale.reset(new scene::Scale(2.0f, Vector2(0.1f, 0.1f), false));
    witchFade.reset(new scene::Fade(2.0f, 0.4f));

    witchRotate.reset(new scene::Rotate(1.0f, Vector3(0.0f, 0.0f, TAU), false));

    witchRepeat.reset(new scene::Repeat(3));
    witchRotate->setParent(witchRepeat.get());
    witchParallel.reset(new scene::Parallel());

    witchScale->setParent(witchParallel.get());
    witchFade->setParent(witchParallel.get());

    witchSequence.reset(new scene::Sequence());
    witchRepeat->setParent(witchSequence.get());
    witchParallel->setParent(witchSequence.get());

    witchSequence->setNode(&witch);
    witchSequence->start();

    ballSprite.initFromFile("ball.png");
    ballSprite.setNode(&ball);
    ball.setParent(&layer);

    ballDelay.reset(new scene::Animator(0.5f));
    ballMove.reset(new scene::Move(2.0f, Vector2(0.0f, -240.0f), false));
    ballEase.reset(new scene::Ease(scene::Ease::Type::OUT, scene::Ease::Func::BOUNCE));
    ballMove->setParent(ballEase.get());
     
    ballSequence.reset(new scene::Sequence());
    ballDelay->setParent(ballSequence.get());
    ballEase->setParent(ballSequence.get());

    ballSequence->setNode(&ball);
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
