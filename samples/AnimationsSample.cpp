// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AnimationsSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

AnimationsSample::AnimationsSample(Samples& aSamples):
    samples(aSamples),
    backButton(std::make_shared<ouzel::gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK))
{
    eventHandler.gamepadHandler = bind(&AnimationsSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&AnimationsSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&AnimationsSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    camera = std::make_shared<scene::Camera>();
    camera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera->setTargetContentSize(Size2(800.0f, 600.0f));

    layer = std::make_shared<scene::Layer>();
    layer->addChild(camera);
    addLayer(layer);

    shapeDrawable = std::make_shared<scene::ShapeDrawable>();
    shapeDrawable->rectangle(ouzel::Rectangle(100.0f, 100.0f), Color(0, 128, 128, 255), true);
    shapeDrawable->rectangle(ouzel::Rectangle(100.0f, 100.0f), Color::WHITE, false);
    shapeDrawable->line(Vector2(0.0f, 0.0f), Vector2(50.0f, 50.0f), Color::CYAN, 2.0f);
    shapeDrawable->point(Vector2(75.0f, 75.0f), Color::RED);

    shapeDrawable->curve({Vector2(50.0f, 50.0f),
                          Vector2(100.0f, 50.0f),
                          Vector2(50.0f, 0.0f),
                          Vector2(100.0f, 0.0f)},
                         Color::YELLOW);

    shapeDrawable->circle(Vector2(75.0f, 75.0f), 20.0f, Color::BLUE);
    shapeDrawable->circle(Vector2(25.0f, 75.0f), 20.0f, Color::BLUE, true);

    drawNode = std::make_shared<scene::Node>();
    drawNode->addComponent(shapeDrawable);
    drawNode->setPosition(Vector2(-300, 0.0f));
    layer->addChild(drawNode);

    shake = std::make_shared<scene::Shake>(10.0f, Vector2(10.0f, 20.0f), 20.0f);
    drawNode->addComponent(shake);
    shake->start();

    witchSprite = std::make_shared<scene::Sprite>();
    witchSprite->initFromFile("witch.png");

    witch = std::make_shared<scene::Node>();
    witch->setPosition(Vector2(200, 0.0f));
    witch->addComponent(witchSprite);
    layer->addChild(witch);

    witchScale = std::make_shared<scene::Scale>(2.0f, Vector2(0.1f, 0.1f), false);
    witchFade = std::make_shared<scene::Fade>(2.0f, 0.4f);

    vector<std::shared_ptr<scene::Animator>> parallel = {
        witchScale,
        witchFade
    };

    witchRotate = std::make_shared<scene::Rotate>(1.0f, Vector3(0.0f, 0.0f, TAU), false);

    witchRepeat = std::make_shared<scene::Repeat>(witchRotate, 3);
    witchParallel = std::make_shared<scene::Parallel>(parallel);

    vector<std::shared_ptr<scene::Animator>> sequence = {
        witchRepeat,
        witchParallel
    };

    witchSequence = std::make_shared<scene::Sequence>(sequence);

    witch->addComponent(witchSequence);
    witchSequence->start();

    ballSprite = std::make_shared<scene::Sprite>();
    ballSprite->initFromFile("ball.png");

    ball = std::make_shared<scene::Node>();
    ball->addComponent(ballSprite);
    layer->addChild(ball);

    ballDelay = std::make_shared<scene::Animator>(1.0f);
    ballMove = std::make_shared<scene::Move>(2.0f, Vector2(0.0f, -240.0f), false);
    ballEase = std::make_shared<scene::Ease>(ballMove, scene::Ease::Type::OUT, scene::Ease::Func::BOUNCE);

    vector<std::shared_ptr<scene::Animator>> sequence2 = {
        ballDelay,
        ballEase
    };
     
    ballSequence = std::make_shared<scene::Sequence>(sequence2);

    ball->addComponent(ballSequence);
    ballSequence->start();

    guiCamera = std::make_shared<scene::Camera>();
    guiCamera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera->setTargetContentSize(Size2(800.0f, 600.0f));

    guiLayer = std::make_shared<scene::Layer>();
    guiLayer->addChild(guiCamera);
    addLayer(guiLayer);

    menu = std::make_shared<gui::Menu>();
    guiLayer->addChild(menu);

    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton);
}

bool AnimationsSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == input::GamepadButton::FACE2)
        {
            samples.setScene(std::shared_ptr<scene::Scene>(new MainMenu(samples)));
        }
    }

    return true;
}

bool AnimationsSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE && event.node == backButton)
    {
        samples.setScene(std::shared_ptr<scene::Scene>(new MainMenu(samples)));
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
                samples.setScene(std::shared_ptr<scene::Scene>(new MainMenu(samples)));
                break;
            default:
                break;
        }
    }

    return true;
}
