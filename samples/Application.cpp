// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Application.h"
#include <cmath>

using namespace std;
using namespace ouzel;

Application::~Application()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
}

void Application::begin()
{
#if defined(OUZEL_PLATFORM_WINDOWS) || defined(OUZEL_PLATFORM_LINUX)
    sharedEngine->getFileSystem()->addResourcePath("Resources");
#endif

    sharedEngine->getLocalization()->addLanguage("latvian", "lv.mo");
    sharedEngine->getLocalization()->setLanguage("latvian");

    eventHandler = make_shared<EventHandler>();

    eventHandler->keyboardHandler = bind(&Application::handleKeyboard, this, placeholders::_1, placeholders::_2, placeholders::_3);
    eventHandler->mouseHandler = bind(&Application::handleMouse, this, placeholders::_1, placeholders::_2, placeholders::_3);
    eventHandler->touchHandler = bind(&Application::handleTouch, this, placeholders::_1, placeholders::_2, placeholders::_3);
    eventHandler->gamepadHandler = bind(&Application::handleGamepad, this, placeholders::_1, placeholders::_2, placeholders::_3);
    eventHandler->uiHandler = bind(&Application::handleUI, this, placeholders::_1, placeholders::_2, placeholders::_3);

    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    sharedEngine->getRenderer()->setClearColor(graphics::Color(64, 0, 0));
    sharedEngine->getWindow()->setTitle("Sample");

    scene::ScenePtr scene = make_shared<scene::Scene>();
    sharedEngine->getSceneManager()->setScene(scene);

    rtLayer = make_shared<scene::Layer>();
    scene->addLayer(rtLayer);
    renderTarget = sharedEngine->getRenderer()->createRenderTarget(Size2(256.0f, 256.0f), false);
    renderTarget->setClearColor(graphics::Color(0, 64, 0));
    rtLayer->setCamera(make_shared<scene::Camera>());
    rtLayer->setRenderTarget(renderTarget);

    layer = make_shared<scene::Layer>();
    scene->addLayer(layer);
    layer->setCamera(make_shared<scene::Camera>());

    uiLayer = make_shared<scene::Layer>();
    scene->addLayer(uiLayer);
    uiLayer->setCamera(make_shared<scene::Camera>());

    scene::DebugDrawablePtr debugDrawable = make_shared<scene::DebugDrawable>();
    debugDrawable->rectangle(Rectangle(100.0f, 100.0f), graphics::Color(0, 128, 128, 255), true);
    debugDrawable->rectangle(Rectangle(100.0f, 100.0f), graphics::Color(255, 255, 255, 255), false);
    debugDrawable->line(Vector2(0.0f, 0.0f), Vector2(50.0f, 50.0f), graphics::Color(0, 255, 255, 255));
    debugDrawable->point(Vector2(75.0f, 75.0f), graphics::Color(255, 0, 0, 255));

    debugDrawable->circle(Vector2(75.0f, 75.0f), 20.0f, graphics::Color(0, 0, 255, 255));
    debugDrawable->circle(Vector2(25.0f, 75.0f), 20.0f, graphics::Color(0, 0, 255, 255), true);

    scene::NodePtr drawNode = make_shared<scene::Node>();
    drawNode->addDrawable(debugDrawable);
    drawNode->setPosition(Vector2(-300, 0.0f));
    layer->addChild(drawNode);

    drawNode->animate(make_shared<scene::Shake>(10.0f, Vector2(10.0f, 20.0f), 20.0f));

    scene::SpritePtr characterSprite = scene::Sprite::createFromFile("run.json");
    characterSprite->play(true);

    character = make_shared<scene::Node>();
    character->addDrawable(characterSprite);
    layer->addChild(character);
    character->setPosition(Vector2(-300.0f, 0.0f));

    vector<scene::AnimatorPtr> sequence = {
        make_shared<scene::Move>(4.0f, Vector2(300.0f, 0.0f)),
        make_shared<scene::Fade>(2.0f, 0.4f)
    };

    character->animate(make_shared<scene::Sequence>(sequence));

    scene::SpritePtr fireSprite = scene::Sprite::createFromFile("fire.json");
    fireSprite->play(true);

    scene::NodePtr fireNode = make_shared<scene::Node>();
    fireNode->addDrawable(fireSprite);
    fireNode->setPosition(Vector2(-100.0f, -100.0f));
    layer->addChild(fireNode);
    fireNode->animate(make_shared<scene::Fade>(5.0f, 0.5f));

    scene::ParticleSystemPtr flameParticleSystem = scene::ParticleSystem::createFromFile("flame.json");

    flame = make_shared<scene::Node>();
    flame->addDrawable(flameParticleSystem);
    layer->addChild(flame);

    scene::SpritePtr witchSprite = scene::Sprite::createFromFile("witch.png");
    //witchSprite->setColor(graphics::Color(128, 0, 255, 255));

    witch = make_shared<scene::Node>();
    witch->addDrawable(witchSprite);
    witch->setPosition(Vector2(-250.0f, -200.0f));
    layer->addChild(witch);
    witch->animate(make_shared<scene::Repeat>(make_shared<scene::Rotate>(1.0f, TAU, false), 3));

    gui::LabelPtr label = gui::Label::create("font.fnt", sharedEngine->getLocalization()->getString("Test"));
    uiLayer->addChild(label);

    vector<scene::AnimatorPtr> sequence2 = {
        make_shared<scene::Animator>(1.0f), // delay
        make_shared<scene::Ease>(make_shared<scene::Move>(2.0f, Vector2(0.0f, -240.0f), false), scene::Ease::Type::OUT, scene::Ease::Func::BOUNCE)
    };

    label->animate(make_shared<scene::Sequence>(sequence2));

    button = gui::Button::create("button.png", "button.png", "button_down.png", "", "", graphics::Color(), "");
    button->setPosition(Vector2(-200.0f, 200.0f));
    uiLayer->addChild(button);

    checkBox = gui::CheckBox::create("checkbox.png", "", "", "", "tick.png");
    checkBox->setPosition(Vector2(-100.0f, 200.0f));
    uiLayer->addChild(checkBox);

    // Render target

    scene::NodePtr rtCharacter = make_shared<scene::Node>();
    rtCharacter->addDrawable(characterSprite);
    rtLayer->addChild(rtCharacter);

    scene::SpriteFramePtr rtFrame = scene::SpriteFrame::create(Rectangle(0.0f, 0.0f, 256.0f, 256.0f), renderTarget->getTexture(), false, renderTarget->getTexture()->getSize(), Vector2(), Vector2(0.5f, 0.5f));

    scene::SpritePtr rtSprite = scene::Sprite::createFromSpriteFrames({ rtFrame });
    scene::NodePtr rtNode = make_shared<scene::Node>();
    rtNode->addDrawable(rtSprite);
    rtNode->setPosition(Vector2(200.0f, 200.0f));
    layer->addChild(rtNode);

    sharedEngine->getInput()->startGamepadDiscovery();
}

bool Application::handleKeyboard(Event::Type type, const KeyboardEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

    if (type == Event::Type::KEY_DOWN)
    {
        Vector2 position = layer->getCamera()->getPosition();

        switch (event.key)
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
                witch->setVisible(!witch->isVisible());
                break;
            case input::KeyboardKey::RETURN:
                sharedEngine->getWindow()->setSize(Size2(640.0f, 480.0f));
                break;
            case input::KeyboardKey::TAB:
                button->setEnabled(!button->isEnabled());
                break;
            default:
                break;
        }

        layer->getCamera()->setPosition(position);
    }

    return true;
}

bool Application::handleMouse(Event::Type type, const MouseEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

    switch (type)
    {
        case Event::Type::MOUSE_DOWN:
        {
            sharedEngine->getInput()->setCursorVisible(!sharedEngine->getInput()->isCursorVisible());
            break;
        }
        case Event::Type::MOUSE_MOVE:
        {
            Vector2 worldLocation = layer->getCamera()->convertScreenToWorld(event.position);
            flame->setPosition(worldLocation);
            break;
        }
        default:
            break;
    }

    return true;
}

bool Application::handleTouch(Event::Type type, const TouchEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(type);
    OUZEL_UNUSED(sender);

    Vector2 worldLocation = layer->getCamera()->convertScreenToWorld(event.position);
    flame->setPosition(worldLocation);

    return true;
}

bool Application::handleGamepad(Event::Type type, const GamepadEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        Vector2 position = layer->getCamera()->convertWorldToScreen(flame->getPosition());

        switch (event.button)
        {
            case input::GamepadButton::DPAD_UP:
            case input::GamepadButton::LEFT_THUMB_UP:
            case input::GamepadButton::RIGHT_THUMB_UP:
                position.y = event.value;
                break;
            case input::GamepadButton::DPAD_DOWN:
            case input::GamepadButton::LEFT_THUMB_DOWN:
            case input::GamepadButton::RIGHT_THUMB_DOWN:
                position.y = -event.value;
                break;
            case input::GamepadButton::DPAD_LEFT:
            case input::GamepadButton::LEFT_THUMB_LEFT:
            case input::GamepadButton::RIGHT_THUMB_LEFT:
                position.x = -event.value;
                break;
            case input::GamepadButton::DPAD_RIGHT:
            case input::GamepadButton::LEFT_THUMB_RIGHT:
            case input::GamepadButton::RIGHT_THUMB_RIGHT:
                position.x = event.value;
                break;
            case input::GamepadButton::A:
                witch->setVisible(!event.pressed);
                break;
            default:
                break;
        }

        Vector2 worldLocation = layer->getCamera()->convertScreenToWorld(position);
        flame->setPosition(worldLocation);
    }

    return true;
}

bool Application::handleUI(Event::Type type, const UIEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(event);

    if (type == Event::Type::UI_CLICK_NODE && sender == button)
    {
        character->setVisible(!character->isVisible());
    }

    return true;
}
