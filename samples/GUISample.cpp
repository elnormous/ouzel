// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "GUISample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;

namespace samples
{
    GUISample::GUISample():
        button("button.png", "button_selected.png", "button_down.png", "", "Button", "Arial", 1.0F, Color::red(), Color::red(), Color::black()),
        fullscreenButton("button.png", "button_selected.png", "button_down.png", "", "Fullscreen", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
        minimizeButton("button.png", "button_selected.png", "button_down.png", "", "Minimize", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
        maximizeButton("button.png", "button_selected.png", "button_down.png", "", "Maximize", "Arial", 1.0F, Color::black(), Color::black(), Color::black()),
        checkBox("checkbox.png", "", "", "", "tick.png"),
        label1("checkbox test", "AmosisTechnik", 24.0F, Color::white(), Vector2F(0.0F, 0.5F)),
        label2("", "ArialBlack"),
        label3("UTF-8 ĀāČč\nNew line", "ArialBlack", 1.0F, Color::white(), Vector2F(0.0F, 0.5F)),
        backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, Color::black(), Color::black(), Color::black())
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
        camera.setClearColor(ouzel::Color(64, 0, 0));
        camera.setScaleMode(scene::Camera::ScaleMode::showAll);
        camera.setTargetContentSize(Size2F(800.0F, 600.0F));
        cameraActor.addComponent(&camera);
        layer.addChild(&cameraActor);
        addLayer(layer);

        layer.addChild(&menu);

        button.setPosition(Vector2F(-200.0F, 100.0F));
        menu.addWidget(&button);

        checkBox.setPosition(Vector2F(-100.0F, 100.0F));
        layer.addChild(&checkBox);

        fullscreenButton.setPosition(Vector2F(-200.0F, 40.0F));
        menu.addWidget(&fullscreenButton);

        minimizeButton.setPosition(Vector2F(-200.0F, -0.0F));
        menu.addWidget(&minimizeButton);

        maximizeButton.setPosition(Vector2F(-200.0F, -40.0F));
        menu.addWidget(&maximizeButton);

        label1.getLabelDrawable()->setColor(Color::cyan());
        label1.setPosition(Vector2F(-88.0F, 108.0F));
        layer.addChild(&label1);

        engine->getLocalization().addLanguage("latvian", engine->getFileSystem().readFile("lv.mo"));
        engine->getLocalization().setLanguage("latvian");

        label2.setText(engine->getLocalization().getString("Ouzel"));

        label2.setPosition(Vector2F(10.0F, 0.0F));
        layer.addChild(&label2);

        label3.getLabelDrawable()->setColor(Color::blue());
        label3.setPosition(Vector2F(-100.0F, -100.0F));
        label3.setScale(Vector3F(0.5F, 0.5F, 1.0F));
        layer.addChild(&label3);

        backButton.setPosition(Vector2F(-200.0F, -200.0F));
        menu.addWidget(&backButton);
    }
}

