// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "RTSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;

RTSample::RTSample():
    characterSprite("run.json"),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
    renderTexture(std::make_shared<graphics::Texture>(*engine->getRenderer(),
                                                      Size2U(256, 256),
                                                      graphics::Flags::BindRenderTarget |
                                                      graphics::Flags::BindShader, 1, 1)),
    depthTexture(*engine->getRenderer(),
                 Size2U(256, 256),
                 graphics::Flags::BindRenderTarget |
                 graphics::Flags::BindShader, 1, 1,
                 graphics::PixelFormat::Depth),
    renderTarget(*engine->getRenderer(),
                 {renderTexture.get()},
                 &depthTexture)
{
    handler.gamepadHandler = [](const GamepadEvent& event) {
        if (event.type == Event::Type::GamepadButtonChange)
        {
            if (event.pressed &&
                event.button == Gamepad::Button::FaceRight)
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());
        }

        return false;
    };

    handler.uiHandler = [this](const UIEvent& event) {
        if (event.type == Event::Type::ActorClick && event.actor == &backButton)
            engine->getSceneManager().setScene(std::make_unique<MainMenu>());

        return false;
    };

    handler.keyboardHandler = [](const KeyboardEvent& event) {
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
    };

    engine->getEventDispatcher().addEventHandler(handler);

    addLayer(&rtLayer);

    rtCamera.setRenderTarget(&renderTarget);
    rtCamera.setClearColorBuffer(true);
    rtCamera.setClearColor(Color(0, 64, 0));
    rtCameraActor.addComponent(&rtCamera);
    rtLayer.addChild(&rtCameraActor);

    camera1.setClearColorBuffer(true);
    camera1.setScaleMode(scene::Camera::ScaleMode::showAll);
    camera1.setTargetContentSize(Size2F(400.0F, 600.0F));
    camera1.setViewport(RectF(0.0F, 0.0F, 0.5F, 1.0F));
    camera1Actor.addComponent(&camera1);

    camera2.setScaleMode(scene::Camera::ScaleMode::showAll);
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

    guiCamera.setScaleMode(scene::Camera::ScaleMode::showAll);
    guiCamera.setTargetContentSize(Size2F(800.0F, 600.0F));
    guiCameraActor.addComponent(&guiCamera);
    guiLayer.addChild(&guiCameraActor);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    backButton.setPosition(Vector2F(-200.0F, -200.0F));
    menu.addWidget(&backButton);
}
