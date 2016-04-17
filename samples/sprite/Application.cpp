// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Application.h"
#include <cmath>

using namespace std;
using namespace ouzel;

Application::~Application()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(_eventHandler);
}

void Application::begin()
{
    sharedEngine->getLocalization()->addLanguage("latvian", "lv.mo");
    sharedEngine->getLocalization()->setLanguage("latvian");

    _eventHandler = make_shared<EventHandler>();

    _eventHandler->keyboardHandler = std::bind(&Application::handleKeyboard, this, std::placeholders::_1, std::placeholders::_2);
    _eventHandler->mouseHandler = std::bind(&Application::handleMouse, this, std::placeholders::_1, std::placeholders::_2);
    _eventHandler->touchHandler = std::bind(&Application::handleTouch, this, std::placeholders::_1, std::placeholders::_2);
    _eventHandler->gamepadHandler = std::bind(&Application::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
    _eventHandler->uiHandler = std::bind(&Application::handleUI, this, std::placeholders::_1, std::placeholders::_2);

    sharedEngine->getEventDispatcher()->addEventHandler(_eventHandler);

    sharedEngine->getRenderer()->setClearColor(video::Color(64, 0, 0));
    sharedEngine->getWindow()->setTitle("Sample");

    scene::ScenePtr scene = make_shared<scene::Scene>();
    sharedEngine->getSceneManager()->setScene(scene);

    _layer = std::make_shared<scene::Layer>();
    scene->addLayer(_layer);
    _layer->setCamera(std::make_shared<scene::Camera>());

    _uiLayer = std::make_shared<scene::Layer>();
    scene->addLayer(_uiLayer);
    _uiLayer->setCamera(std::make_shared<scene::Camera>());

    scene::DrawNodePtr drawNode = std::make_shared<scene::DrawNode>();
    drawNode->rectangle(Rectangle(100.0f, 100.0f), video::Color(0, 128, 128, 255), true);
    drawNode->rectangle(Rectangle(100.0f, 100.0f), video::Color(255, 255, 255, 255), false);
    drawNode->line(Vector2(0.0f, 0.0f), Vector2(50.0f, 50.0f), video::Color(0, 255, 255, 255));
    drawNode->point(Vector2(75.0f, 75.0f), video::Color(255, 0, 0, 255));

    drawNode->circle(Vector2(75.0f, 75.0f), 20.0f, video::Color(0, 0, 255, 255));
    drawNode->circle(Vector2(25.0f, 75.0f), 20.0f, video::Color(0, 0, 255, 255), true);

    drawNode->setPosition(Vector2(-300, 0.0f));
    _layer->addChild(drawNode);

    _sprite = scene::Sprite::createFromFile("run.json");
    _sprite->play(true);
    _layer->addChild(_sprite);
    _sprite->setPosition(Vector2(-300.0f, 0.0f));

    std::vector<scene::AnimatorPtr> sequence = {
        make_shared<scene::Move>(4.0f, Vector2(300.0f, 0.0f)),
        make_shared<scene::Fade>(2.0f, 0.4f)
    };

    _sprite->animate(make_shared<scene::Sequence>(sequence));

    scene::SpritePtr fire = scene::Sprite::createFromFile("fire.json");
    fire->play(true);
    fire->setPosition(Vector2(-100.0f, -100.0f));
    _layer->addChild(fire);
    fire->animate(make_shared<scene::Fade>(5.0f, 0.5f));

    _flame = scene::ParticleSystem::createFromFile("flame.json");
    _layer->addChild(_flame);

    _witch = scene::Sprite::createFromFile("witch.png");
    _witch->setPosition(Vector2(100.0f, 100.0f));
    _witch->setColor(video::Color(128, 0, 255, 255));
    _layer->addChild(_witch);
    _witch->animate(make_shared<scene::Repeat>(make_shared<scene::Rotate>(1.0f, TAU, false), 3));

    gui::LabelPtr label = gui::Label::create("font.fnt", sharedEngine->getLocalization()->getString("Test"));
    _uiLayer->addChild(label);

    std::vector<scene::AnimatorPtr> sequence2 = {
        make_shared<scene::Animator>(1.0f), // delay
        make_shared<scene::Ease>(make_shared<scene::Move>(2.0f, Vector2(0.0f, -240.0f), false), scene::Ease::Type::OUT, scene::Ease::Func::BOUNCE)
    };

    label->animate(make_shared<scene::Sequence>(sequence2));

    _button = gui::Button::create("button.png", "button.png", "button_down.png", "", "", video::Color(), "");
    _button->setPosition(Vector2(-200.0f, 200.0f));
    _uiLayer->addChild(_button);

    sharedEngine->getInput()->startGamepadDiscovery();
}

bool Application::handleKeyboard(const KeyboardEventPtr& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

    if (event->type == Event::Type::KEY_DOWN)
    {
        Vector2 position = _layer->getCamera()->getPosition();

        switch (event->key)
        {
            case input::KeyboardKey::UP:
                position.y += 10.0f;
                break;
            case input::KeyboardKey::DOWN:
                position.y -= 10.0f;
                break;
            case input::KeyboardKey::LEFT:
                position.x -= 10.0f;
                break;
            case input::KeyboardKey::RIGHT:
                position.x += 10.0f;
                break;
            case input::KeyboardKey::SPACE:
                _witch->setVisible(!_witch->isVisible());
                break;
            case input::KeyboardKey::RETURN:
                sharedEngine->getWindow()->setSize(Size2(640.0f, 480.0f));
                break;
            case input::KeyboardKey::TAB:
                _button->setEnabled(!_button->isEnabled());
                break;
            default:
                break;
        }

        _layer->getCamera()->setPosition(position);
    }

    return true;
}

bool Application::handleMouse(const MouseEventPtr& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

    switch (event->type)
    {
        case Event::Type::MOUSE_DOWN:
        {
            sharedEngine->getInput()->setCursorVisible(!sharedEngine->getInput()->isCursorVisible());
            break;
        }
        case Event::Type::MOUSE_MOVE:
        {
            Vector2 worldLocation = _layer->getCamera()->convertScreenToWorld(event->position);
            _flame->setPosition(worldLocation);
            break;
        }
        default:
            break;
    }

    return true;
}

bool Application::handleTouch(const TouchEventPtr& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

    Vector2 worldLocation = _layer->getCamera()->convertScreenToWorld(event->position);
    _flame->setPosition(worldLocation);

    return true;
}

bool Application::handleGamepad(const GamepadEventPtr& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

    if (event->type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        Vector2 position = _layer->getCamera()->convertWorldToScreen(_flame->getPosition());

        switch (event->button)
        {
            case input::GamepadButton::DPAD_UP:
            case input::GamepadButton::LEFT_THUMB_UP:
            case input::GamepadButton::RIGHT_THUMB_UP:
                position.y = event->value;
                break;
            case input::GamepadButton::DPAD_DOWN:
            case input::GamepadButton::LEFT_THUMB_DOWN:
            case input::GamepadButton::RIGHT_THUMB_DOWN:
                position.y = -event->value;
                break;
            case input::GamepadButton::DPAD_LEFT:
            case input::GamepadButton::LEFT_THUMB_LEFT:
            case input::GamepadButton::RIGHT_THUMB_LEFT:
                position.x = -event->value;
                break;
            case input::GamepadButton::DPAD_RIGHT:
            case input::GamepadButton::LEFT_THUMB_RIGHT:
            case input::GamepadButton::RIGHT_THUMB_RIGHT:
                position.x = event->value;
                break;
            case input::GamepadButton::A:
                _witch->setVisible(!event->pressed);
                break;
            default:
                break;
        }

        Vector2 worldLocation = _layer->getCamera()->convertScreenToWorld(position);
        _flame->setPosition(worldLocation);
    }

    return true;
}

bool Application::handleUI(const UIEventPtr& event, const VoidPtr& sender) const
{
    if (event->type == Event::Type::UI_CLICK_NODE && sender == _button)
    {
        _sprite->setVisible(!_sprite->isVisible());
    }

    return true;
}
