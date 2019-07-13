// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "RTSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;

RTSample::RTSample():
    characterSprite("run.json"),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, Color::black(), Color::black(), Color::black())
{
    handler.gamepadHandler = std::bind(&RTSample::handleGamepad, this, std::placeholders::_1);
    handler.uiHandler = std::bind(&RTSample::handleUI, this, std::placeholders::_1);
    handler.keyboardHandler = std::bind(&RTSample::handleKeyboard, this, std::placeholders::_1);
    engine->getEventDispatcher().addEventHandler(handler);

    addLayer(&rtLayer);

    auto renderTexture = std::make_shared<graphics::Texture>(*engine->getRenderer(),
                                                             Size2U(256, 256),
                                                             graphics::Flags::BindRenderTarget |
                                                             graphics::Flags::BindShader, 1, 1);

    auto depthTexture = std::make_shared<graphics::Texture>(*engine->getRenderer(),
                                                            Size2U(256, 256),
                                                            graphics::Flags::BindRenderTarget |
                                                            graphics::Flags::BindShader, 1, 1,
                                                            graphics::PixelFormat::Depth);

    auto renderTarget = std::make_shared<graphics::RenderTarget>(*engine->getRenderer(),
                                                                 std::vector<std::shared_ptr<graphics::Texture>>{renderTexture},
                                                                 depthTexture);

    rtCamera.setRenderTarget(renderTarget);
    rtCamera.setClearColorBuffer(true);
    rtCamera.setClearColor(Color(0, 64, 0));
    rtCameraActor.addComponent(&rtCamera);
    rtLayer.addChild(&rtCameraActor);

    camera1.setClearColorBuffer(true);
    camera1.setScaleMode(scene::Camera::ScaleMode::ShowAll);
    camera1.setTargetContentSize(Size2F(400.0F, 600.0F));
    camera1.setViewport(RectF(0.0F, 0.0F, 0.5F, 1.0F));
    camera1Actor.addComponent(&camera1);

    camera2.setScaleMode(scene::Camera::ScaleMode::ShowAll);
    camera2.setTargetContentSize(Size2F(400.0F, 600.0F));
    camera2.setViewport(RectF(0.5F, 0.0F, 0.5F, 1.0F));
    camera2Actor.addComponent(&camera2);

    layer.addChild(&camera1Actor);
    layer.addChild(&camera2Actor);
    addLayer(&layer);

    characterSprite.setAnimation("", true);
    characterSprite.play();
    rtCharacter.addComponent(&characterSprite);
    rtLayer.addChild(&rtCharacter);

    rtSprite.init(renderTexture);
    rtActor.addComponent(&rtSprite);
    layer.addChild(&rtActor);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::ShowAll);
    guiCamera.setTargetContentSize(Size2F(800.0F, 600.0F));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    backButton.setPosition(Vector2F(-200.0F, -200.0F));
    menu.addWidget(&backButton);
}

bool RTSample::handleGamepad(const GamepadEvent& event)
{
    if (event.type == Event::Type::GamepadButtonChange)
    {
        if (event.pressed &&
            event.button == Gamepad::Button::FaceRight)
            engine->getSceneManager().setScene(std::make_unique<MainMenu>());
    }

    return false;
}

bool RTSample::handleUI(const UIEvent& event) const
{
    if (event.type == Event::Type::ActorClick && event.actor == &backButton)
        engine->getSceneManager().setScene(std::make_unique<MainMenu>());

    return false;
}

bool RTSample::handleKeyboard(const KeyboardEvent& event) const
{
    if (event.type == Event::Type::KeyboardKeyPress)
    {
        switch (event.key)
        {
            case Keyboard::Key::Escape:
            case Keyboard::Key::Menu:
            case Keyboard::Key::Back:
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                return true;
            default:
                break;
        }
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
}
