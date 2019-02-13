// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RTSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;

RTSample::RTSample():
    characterSprite("run.json"),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", 1.0F, Color::BLACK, Color::BLACK, Color::BLACK)
{
    handler.gamepadHandler = std::bind(&RTSample::handleGamepad, this, std::placeholders::_1);
    handler.uiHandler = std::bind(&RTSample::handleUI, this, std::placeholders::_1);
    handler.keyboardHandler = std::bind(&RTSample::handleKeyboard, this, std::placeholders::_1);
    engine->getEventDispatcher().addEventHandler(&handler);

    addLayer(&rtLayer);

    std::shared_ptr<graphics::Texture> renderTexture = std::make_shared<graphics::Texture>(*engine->getRenderer(),
                                                                                           Size2<uint32_t>(256, 256),
                                                                                           graphics::Texture::BIND_RENDER_TARGET |
                                                                                           graphics::Texture::BIND_SHADER, 1, 1);

    std::shared_ptr<graphics::Texture> depthTexture = std::make_shared<graphics::Texture>(*engine->getRenderer(),
                                                                                          Size2<uint32_t>(256, 256),
                                                                                          graphics::Texture::BIND_RENDER_TARGET |
                                                                                          graphics::Texture::BIND_SHADER, 1, 1,
                                                                                          graphics::PixelFormat::DEPTH);

    std::shared_ptr<graphics::RenderTarget> renderTarget = std::make_shared<graphics::RenderTarget>(*engine->getRenderer(),
                                                                                                    std::vector<std::shared_ptr<graphics::Texture>>{renderTexture},
                                                                                                    depthTexture);

    rtCamera.setRenderTarget(renderTarget);
    rtCamera.setClearColorBuffer(true);
    rtCamera.setClearColor(Color(0, 64, 0));
    rtCameraActor.addComponent(&rtCamera);
    rtLayer.addChild(&rtCameraActor);

    camera1.setClearColorBuffer(true);
    camera1.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera1.setTargetContentSize(Size2<float>(400.0F, 600.0F));
    camera1.setViewport(Rect<float>(0.0F, 0.0F, 0.5F, 1.0F));
    camera1Actor.addComponent(&camera1);

    camera2.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera2.setTargetContentSize(Size2<float>(400.0F, 600.0F));
    camera2.setViewport(Rect<float>(0.5F, 0.0F, 0.5F, 1.0F));
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

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2<float>(800.0F, 600.0F));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    backButton.setPosition(Vector2<float>(-200.0F, -200.0F));
    menu.addWidget(&backButton);
}

bool RTSample::handleGamepad(const GamepadEvent& event)
{
    if (event.type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == Gamepad::Button::FACE_RIGHT)
            engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return false;
}

bool RTSample::handleUI(const UIEvent& event) const
{
    if (event.type == Event::Type::ACTOR_CLICK && event.actor == &backButton)
        engine->getSceneManager().setScene(std::unique_ptr<scene::Scene>(new MainMenu()));

    return false;
}

bool RTSample::handleKeyboard(const KeyboardEvent& event) const
{
    if (event.type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
            case Keyboard::Key::BACK:
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
            case Keyboard::Key::BACK:
                return true;
            default:
                break;
        }
    }

    return false;
}
