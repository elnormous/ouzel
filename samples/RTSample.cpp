// Ouzel by Elviss Strazdins

#include "core/Engine.hpp"
#include "RTSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;

namespace samples
{
    RTSample::RTSample():
        characterSprite("run.json"),
        backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
        renderTexture(std::make_shared<graphics::Texture>(*engine->getGraphics(),
                                                          ouzel::Size<std::uint32_t, 2>(256, 256),
                                                          graphics::Flags::bindRenderTarget |
                                                          graphics::Flags::bindShader, 1, 1)),
        depthTexture(*engine->getGraphics(),
                     ouzel::Size<std::uint32_t, 2>(256, 256),
                     graphics::Flags::bindRenderTarget |
                     graphics::Flags::bindShader, 1, 1,
                     graphics::PixelFormat::depth),
        renderTarget(*engine->getGraphics(),
                     {renderTexture.get()},
                     &depthTexture)
    {
        handler.gamepadHandler = [](const GamepadEvent& event) {
            if (event.type == Event::Type::gamepadButtonChange)
            {
                if (event.pressed &&
                    event.button == Gamepad::Button::faceRight)
                    engine->getSceneManager().setScene(std::make_unique<MainMenu>());
            }

            return false;
        };

        handler.uiHandler = [this](const UIEvent& event) {
            if (event.type == Event::Type::actorClick && event.actor == &backButton)
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());

            return false;
        };

        handler.keyboardHandler = [](const KeyboardEvent& event) {
            if (event.type == Event::Type::keyboardKeyPress)
            {
                switch (event.key)
                {
                    case Keyboard::Key::escape:
                    case Keyboard::Key::menu:
                    case Keyboard::Key::back:
                        engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                        return true;
                    default:
                        break;
                }
            }
            else if (event.type == Event::Type::keyboardKeyRelease)
            {
                switch (event.key)
                {
                    case Keyboard::Key::escape:
                    case Keyboard::Key::menu:
                    case Keyboard::Key::back:
                        return true;
                    default:
                        break;
                }
            }

            return false;
        };

        engine->getEventDispatcher().addEventHandler(handler);

        addLayer(rtLayer);

        rtCamera.setRenderTarget(&renderTarget);
        rtCamera.setClearColorBuffer(true);
        rtCamera.setClearColor(Color(0, 64, 0));
        rtCameraActor.addComponent(rtCamera);
        rtLayer.addChild(rtCameraActor);

        camera1.setClearColorBuffer(true);
        camera1.setScaleMode(scene::Camera::ScaleMode::showAll);
        camera1.setTargetContentSize(ouzel::Size<float, 2>(400.0F, 600.0F));
        camera1.setViewport(ouzel::Rect<float>(0.0F, 0.0F, 0.5F, 1.0F));
        camera1Actor.addComponent(camera1);

        camera2.setScaleMode(scene::Camera::ScaleMode::showAll);
        camera2.setTargetContentSize(ouzel::Size<float, 2>(400.0F, 600.0F));
        camera2.setViewport(ouzel::Rect<float>(0.5F, 0.0F, 0.5F, 1.0F));
        camera2Actor.addComponent(camera2);

        layer.addChild(camera1Actor);
        layer.addChild(camera2Actor);
        addLayer(layer);

        characterSprite.setAnimation("", true);
        characterSprite.play();
        rtCharacter.addComponent(characterSprite);
        rtLayer.addChild(rtCharacter);

        rtSprite.init(renderTexture);
        rtActor.addComponent(rtSprite);
        layer.addChild(rtActor);

        guiCamera.setScaleMode(scene::Camera::ScaleMode::showAll);
        guiCamera.setTargetContentSize(ouzel::Size<float, 2>(800.0F, 600.0F));
        guiCameraActor.addComponent(guiCamera);
        guiLayer.addChild(guiCameraActor);
        addLayer(guiLayer);

        guiLayer.addChild(menu);

        backButton.setPosition(Vector<float, 2>(-200.0F, -200.0F));
        menu.addWidget(backButton);
    }
}
