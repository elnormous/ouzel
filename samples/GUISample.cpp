// Ouzel by Elviss Strazdins

#include "core/Engine.hpp"
#include "GUISample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;
using namespace math;

namespace samples
{
    GUISample::GUISample():
        button{"button.png", "button_selected.png", "button_down.png", "", "Button", "Arial", 1.0F, redColor(), redColor(), blackColor()},
        fullscreenButton{"button.png", "button_selected.png", "button_down.png", "", "Fullscreen", "Arial", 1.0F, blackColor(), blackColor(), blackColor()},
        minimizeButton{"button.png", "button_selected.png", "button_down.png", "", "Minimize", "Arial", 1.0F, blackColor(), blackColor(), blackColor()},
        maximizeButton{"button.png", "button_selected.png", "button_down.png", "", "Maximize", "Arial", 1.0F, blackColor(), blackColor(), blackColor()},
        checkBox{"checkbox.png", "", "", "", "tick.png"},
        label1{"checkbox test", "AmosisTechnik", 24.0F, whiteColor(), Vector<float, 2>{0.0F, 0.5F}},
        label2{"", "ArialBlack"},
        label3{"UTF-8 ĀāČč\nNew line", "ArialBlack", 1.0F, whiteColor(), Vector<float, 2>{0.0F, 0.5F}},
        backButton{"button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, blackColor(), blackColor(), blackColor()}
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
            if (event.type == Event::Type::actorClick)
            {
                if (event.actor == &backButton)
                    engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                else if (event.actor == &button)
                    checkBox.setChecked(!checkBox.isChecked());
                else if (event.actor == &fullscreenButton)
                {
                    bool fullscreen = engine->getWindow()->isFullscreen();
                    engine->getWindow()->setFullscreen(!fullscreen);
                }
                else if (event.actor == &minimizeButton)
                {
                    engine->getWindow()->minimize();
                }
                else if (event.actor == &maximizeButton)
                {
                    engine->getWindow()->maximize();
                }
            }

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

        camera.setClearColorBuffer(true);
        camera.setClearColor(math::Color{64, 0, 0});
        camera.setScaleMode(scene::Camera::ScaleMode::showAll);
        camera.setTargetContentSize(math::Size<float, 2>{800.0F, 600.0F});
        cameraActor.addComponent(camera);
        layer.addChild(cameraActor);
        addLayer(layer);

        layer.addChild(menu);

        button.setPosition(Vector<float, 2>{-200.0F, 100.0F});
        menu.addWidget(button);

        checkBox.setPosition(Vector<float, 2>{-100.0F, 100.0F});
        layer.addChild(checkBox);

        fullscreenButton.setPosition(Vector<float, 2>{-200.0F, 40.0F});
        menu.addWidget(fullscreenButton);

        minimizeButton.setPosition(Vector<float, 2>{-200.0F, -0.0F});
        menu.addWidget(minimizeButton);

        maximizeButton.setPosition(Vector<float, 2>{-200.0F, -40.0F});
        menu.addWidget(maximizeButton);

        label1.getLabelDrawable().setColor(cyanColor());
        label1.setPosition(Vector<float, 2>{-88.0F, 108.0F});
        layer.addChild(label1);

        engine->getLocalization().addLanguage("latvian", engine->getFileSystem().readFile("lv.mo"));
        engine->getLocalization().setLanguage("latvian");

        label2.setText(engine->getLocalization().getString("Ouzel"));

        label2.setPosition(Vector<float, 2>{10.0F, 0.0F});
        layer.addChild(label2);

        label3.getLabelDrawable().setColor(blueColor());
        label3.setPosition(Vector<float, 2>{-100.0F, -100.0F});
        label3.setScale(Vector<float, 3>{0.5F, 0.5F, 1.0F});
        layer.addChild(label3);

        backButton.setPosition(Vector<float, 2>{-200.0F, -200.0F});
        menu.addWidget(backButton);
    }
}

