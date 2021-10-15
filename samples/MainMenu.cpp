// Ouzel by Elviss Strazdins

#include "core/Engine.hpp"
#include "MainMenu.hpp"
#include "SpritesSample.hpp"
#include "GUISample.hpp"
#include "RTSample.hpp"
#include "AnimationsSample.hpp"
#include "InputSample.hpp"
#include "SoundSample.hpp"
#include "PerspectiveSample.hpp"

using namespace ouzel;
using namespace input;
using namespace math;

namespace samples
{
    MainMenu::MainMenu():
        gitHubButton{"button.png", "button_selected.png", "button_down.png", "", "GitHub", "Arial", 1.0F, Color{20, 0, 0, 255}, blackColor(), blackColor()},
        spritesButton{"button.png", "button_selected.png", "button_down.png", "", "Sprites", "Arial", 1.0F,Color{20, 0, 0, 255}, blackColor(), blackColor()},
        guiButton{"button.png", "button_selected.png", "button_down.png", "", "GUI", "Arial", 1.0F, Color{20, 0, 0, 255}, blackColor(), blackColor()},
        renderTargetButton{"button.png", "button_selected.png", "button_down.png", "", "Render target", "Arial", 1.0F, Color{20, 0, 0, 255}, blackColor(), blackColor()},
        animationsButton{"button.png", "button_selected.png", "button_down.png", "", "Animations", "Arial", 1.0F, Color{20, 0, 0, 255}, blackColor(), blackColor()},
        inputButton{"button.png", "button_selected.png", "button_down.png", "", "Input", "Arial", 1.0F, Color{20, 0, 0, 255}, blackColor(), blackColor()},
        soundButton{"button.png", "button_selected.png", "button_down.png", "", "Sound", "Arial", 1.0F, Color{20, 0, 0, 255}, blackColor(), blackColor()},
        perspectiveButton{"button.png", "button_selected.png", "button_down.png", "", "Perspective", "Arial", 1.0F, Color{20, 0, 0, 255}, blackColor(), blackColor()}
    {
        handler.uiHandler = [this](const UIEvent& event) {
            if (event.type == Event::Type::actorClick)
            {
                if (event.actor == &gitHubButton)
                    engine->openUrl("https://github.com/elnormous/ouzel");
                else if (event.actor == &spritesButton)
                    engine->getSceneManager().setScene(std::make_unique<SpritesSample>());
                else if (event.actor == &guiButton)
                    engine->getSceneManager().setScene(std::make_unique<GUISample>());
                else if (event.actor == &renderTargetButton)
                    engine->getSceneManager().setScene(std::make_unique<RTSample>());
                else if (event.actor == &animationsButton)
                    engine->getSceneManager().setScene(std::make_unique<AnimationsSample>());
                else if (event.actor == &inputButton)
                    engine->getSceneManager().setScene(std::make_unique<InputSample>());
                else if (event.actor == &soundButton)
                    engine->getSceneManager().setScene(std::make_unique<SoundSample>());
                else if (event.actor == &perspectiveButton)
                    engine->getSceneManager().setScene(std::make_unique<PerspectiveSample>());
            }

            return false;
        };

        handler.keyboardHandler = [](const KeyboardEvent& event) {
            if (event.type == Event::Type::keyboardKeyPress)
            {
                switch (event.key)
                {
                    case Keyboard::Key::escape:
                        engine->exit();
                        break;
                    case Keyboard::Key::menu:
                    case Keyboard::Key::back:
                        return false;
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
                        return false;
                    default:
                        break;
                }
            }

            return true;
        };

        engine->getEventDispatcher().addEventHandler(handler);

        addLayer(layer);

        camera.setClearColorBuffer(true);
        camera.setClearColor(math::Color{64, 0, 0});
        camera.setScaleMode(scene::Camera::ScaleMode::showAll);
        camera.setTargetContentSize(math::Size<float, 2>{400.0F, 600.0F});
        cameraActor.addComponent(camera);
        layer.addChild(cameraActor);

        layer.addChild(menu);

        gitHubButton.setPosition(Vector<float, 2>{0.0F, 120.0F});
        menu.addWidget(gitHubButton);

        spritesButton.setPosition(Vector<float, 2>{0.0F, 80.0F});
        menu.addWidget(spritesButton);

        guiButton.setPosition(Vector<float, 2>{0.0F, 40.0F});
        menu.addWidget(guiButton);

        renderTargetButton.setPosition(Vector<float, 2>{0.0F, 0.0F});
        menu.addWidget(renderTargetButton);

        animationsButton.setPosition(Vector<float, 2>{0.0F, -40.0F});
        menu.addWidget(animationsButton);

        inputButton.setPosition(Vector<float, 2>{0.0F, -80.0F});
        menu.addWidget(inputButton);

        soundButton.setPosition(Vector<float, 2>{0.0F, -120.0F});
        menu.addWidget(soundButton);

        perspectiveButton.setPosition(Vector<float, 2>{0.0F, -160.0F});
        menu.addWidget(perspectiveButton);
    }
}
