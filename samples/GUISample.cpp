// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "GUISample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;

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
    handler.gamepadHandler = std::bind(&GUISample::handleGamepad, this, std::placeholders::_1);
    handler.uiHandler = std::bind(&GUISample::handleUI, this, std::placeholders::_1);
    handler.keyboardHandler = std::bind(&GUISample::handleKeyboard, this, std::placeholders::_1);
    engine->getEventDispatcher().addEventHandler(handler);

    camera.setClearColorBuffer(true);
    camera.setClearColor(ouzel::Color(64, 0, 0));
    camera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera.setTargetContentSize(Size2F(800.0F, 600.0F));
    cameraActor.addComponent(&camera);
    layer.addChild(&cameraActor);
    addLayer(&layer);

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

bool GUISample::handleGamepad(const GamepadEvent& event)
{
    if (event.type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == Gamepad::Button::FACE_RIGHT)
            engine->getSceneManager().setScene(std::make_unique<MainMenu>());
    }

    return false;
}

bool GUISample::handleUI(const UIEvent& event)
{
    if (event.type == Event::Type::ACTOR_CLICK)
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
}

bool GUISample::handleKeyboard(const KeyboardEvent& event) const
{
    if (event.type == Event::Type::KEY_PRESS)
    {
        switch (event.key)
        {
            case Keyboard::Key::ESCAPE:
            case Keyboard::Key::MENU:
            case Keyboard::Key::BACK:
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());
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
