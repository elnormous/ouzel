// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AnimationsSample.h"

using namespace std;
using namespace ouzel;

AnimationsSample::AnimationsSample(Samples& aSamples):
    samples(aSamples)
{
    eventHandler.uiHandler = bind(&AnimationsSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&AnimationsSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    layer.reset(new scene::Layer());
    camera.reset(new scene::Camera());
    layer->addCamera(camera.get());
    addLayer(layer.get());

    shapeDrawable.reset(new scene::ShapeDrawable());
    shapeDrawable->rectangle(ouzel::Rectangle(100.0f, 100.0f), graphics::Color(0, 128, 128, 255), true);
    shapeDrawable->rectangle(ouzel::Rectangle(100.0f, 100.0f), graphics::Color::WHITE, false);
    shapeDrawable->line(Vector2(0.0f, 0.0f), Vector2(50.0f, 50.0f), graphics::Color::CYAN);
    shapeDrawable->point(Vector2(75.0f, 75.0f), graphics::Color::RED);

    shapeDrawable->circle(Vector2(75.0f, 75.0f), 20.0f, graphics::Color::BLUE);
    shapeDrawable->circle(Vector2(25.0f, 75.0f), 20.0f, graphics::Color::BLUE, true);

    drawNode.reset(new scene::Node());
    drawNode->addComponent(shapeDrawable.get());
    drawNode->setPosition(Vector2(-300, 0.0f));
    layer->addChild(drawNode.get());

    shake.reset(new scene::Shake(10.0f, Vector2(10.0f, 20.0f), 20.0f));
    drawNode->animate(shake.get());

    witchSprite.reset(new scene::Sprite("witch.png"));

    witch.reset(new scene::Node());
    witch->setPosition(Vector2(200, 0.0f));
    witch->addComponent(witchSprite.get());
    layer->addChild(witch.get());

    witchScale.reset(new scene::Scale(2.0f, Vector2(0.1f, 0.1f), false));
    witchFade.reset(new scene::Fade(2.0f, 0.4f));

    vector<scene::Animator*> parallel = {
        witchScale.get(),
        witchFade.get()
    };

    witchRotate.reset(new scene::Rotate(1.0f, TAU, false));

    witchRepeat.reset(new scene::Repeat(witchRotate.get(), 3));
    witchParallel.reset(new scene::Parallel(parallel));

    vector<scene::Animator*> sequence = {
        witchRepeat.get(),
        witchParallel.get()
    };

    witchSequence.reset(new scene::Sequence(sequence));

    witch->animate(witchSequence.get());

    ball.reset(new scene::Node());
    ballSprite.reset(new scene::Sprite("ball.png"));
    ball->addComponent(ballSprite.get());
    layer->addChild(ball.get());

    ballDelay.reset(new scene::Animator(1.0f));
    ballMove.reset(new scene::Move(2.0f, Vector2(0.0f, -240.0f), false));
    ballEase.reset(new scene::Ease(ballMove.get(), scene::Ease::Type::OUT, scene::Ease::Func::BOUNCE));

    vector<scene::Animator*> sequence2 = {
        ballDelay.get(),
        ballEase.get()
    };
     
    ballSequence.reset(new scene::Sequence(sequence2));

    ball->animate(ballSequence.get());

    guiLayer.reset(new scene::Layer());
    guiCamera.reset(new scene::Camera());
    guiLayer->addCamera(guiCamera.get());
    addLayer(guiLayer.get());

    menu.reset(new gui::Menu());
    guiLayer->addChild(menu.get());

    backButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Back", graphics::Color::BLACK, "arial.fnt"));
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton.get());
}

bool AnimationsSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE && event.node == backButton.get())
    {
        samples.setSample("");
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
                samples.setSample("");
                break;
            default:
                break;
        }
    }

    return true;
}
