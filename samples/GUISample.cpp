// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GUISample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

GUISample::GUISample(Samples& aSamples):
    samples(aSamples),
    backButton(std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK))
{
    eventHandler.gamepadHandler = bind(&GUISample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&GUISample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&GUISample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    guiCamera = std::make_shared<scene::Camera>();
    guiCamera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera->setTargetContentSize(Size2(800.0f, 600.0f));

    guiLayer = std::make_shared<scene::Layer>();
    guiLayer->addChild(guiCamera);
    addLayer(guiLayer);

    menu = std::make_shared<gui::Menu>();
    guiLayer->addChild(menu);

    button = std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Button", "arial.fnt", Color::RED, Color::RED, Color::BLACK);
    button->setPosition(Vector2(-200.0f, 100.0f));
    menu->addWidget(button);

    checkBox = std::make_shared<gui::CheckBox>("checkbox.png", "", "", "", "tick.png");
    checkBox->setPosition(Vector2(-100.0f, 100.0f));
    guiLayer->addChild(checkBox);

    fullscreenButton = std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Fullscreen", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK);
    fullscreenButton->setPosition(Vector2(-200.0f, 40.0f));
    menu->addWidget(fullscreenButton);

    label1 = std::make_shared<gui::Label>("checkbox", "arial.fnt");
    label1->setColor(Color::CYAN);
    label1->setPosition(Vector2(-60.0f, 100.0f));
    guiLayer->addChild(label1);

    sharedEngine->getLocalization()->addLanguage("latvian", "lv.mo");
    sharedEngine->getLocalization()->setLanguage("latvian");

    label2 = std::make_shared<gui::Label>(sharedEngine->getLocalization()->getString("Ouzel"), "ArialBlack.fnt");
    label2->setPosition(Vector2(10.0f, 0.0f));
    guiLayer->addChild(label2);

    label3 = std::make_shared<gui::Label>("UTF-8 ĀāČč\nNew line", "ArialBlack.fnt", Color::WHITE, Vector2(0.0f, 0.5f));
    label3->setColor(Color::BLUE);
    label3->setPosition(Vector2(-100.0f, -100.0f));
    label3->setScale(Vector3(0.5f, 0.5f, 1.0f));
    guiLayer->addChild(label3);

    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton);
}

bool GUISample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == input::GamepadButton::FACE2)
        {
            samples.setScene(std::shared_ptr<scene::Scene>(new MainMenu(samples)));
        }
    }

    return true;
}

bool GUISample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == backButton)
        {
            samples.setScene(std::shared_ptr<scene::Scene>(new MainMenu(samples)));
        }
        else if (event.node == button)
        {
            checkBox->setChecked(!checkBox->isChecked());
        }
        else if (event.node == fullscreenButton)
        {
            bool fullscreen = ouzel::sharedEngine->getWindow()->isFullscreen();
            ouzel::sharedEngine->getWindow()->setFullscreen(!fullscreen);
        }
    }

    return true;
}

bool GUISample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
{
    if (type == Event::Type::KEY_DOWN)
    {
        switch (event.key)
        {
            case input::KeyboardKey::ESCAPE:
                samples.setScene(std::shared_ptr<scene::Scene>(new MainMenu(samples)));
                break;
            default:
                break;
        }
    }

    return true;
}
