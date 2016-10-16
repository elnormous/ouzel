// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GUISample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

GUISample::GUISample()
{
    eventHandler.uiHandler = bind(&GUISample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&GUISample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    scene::LayerPtr guiLayer = make_shared<scene::Layer>();
    guiLayer->addCamera(make_shared<scene::Camera>());
    addLayer(guiLayer);

    gui::MenuPtr menu = std::make_shared<gui::Menu>();
    guiLayer->addChild(menu);

    button = make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Button", graphics::Color::RED, "arial.fnt");
    button->setPosition(Vector2(-200.0f, 100.0f));
    menu->addWidget(button);

    checkBox = make_shared<gui::CheckBox>("checkbox.png", "", "", "", "tick.png");
    checkBox->setPosition(Vector2(-100.0f, 100.0f));
    guiLayer->addChild(checkBox);

    fullscreenButton = make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Fullscreen", graphics::Color::BLACK, "arial.fnt");
    fullscreenButton->setPosition(Vector2(-200.0f, 40.0f));
    menu->addWidget(fullscreenButton);

    gui::LabelPtr label = make_shared<gui::Label>("arial.fnt", "checkbox");
    label->setColor(graphics::Color::CYAN);
    label->setPosition(Vector2(-60.0f, 100.0f));
    guiLayer->addChild(label);

    sharedEngine->getLocalization()->addLanguage("latvian", "lv.mo");
    sharedEngine->getLocalization()->setLanguage("latvian");

    gui::LabelPtr label2 = make_shared<gui::Label>("ArialBlack.fnt", sharedEngine->getLocalization()->getString("Ouzel"));
    label2->setPosition(Vector2(10.0f, 0.0f));
    guiLayer->addChild(label2);

    gui::LabelPtr label3 = make_shared<gui::Label>("ArialBlack.fnt", "UTF-8 ĀāČč\nNew line", Vector2(0.0f, 0.5f));
    label3->setColor(graphics::Color::BLUE);
    label3->setPosition(Vector2(-100.0f, -100.0f));
    label3->setScale(Vector2(0.5f, 0.5f));
    guiLayer->addChild(label3);

    backButton = make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Back", graphics::Color::BLACK, "arial.fnt");
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton);
}

GUISample::~GUISample()
{
}

bool GUISample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == backButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::make_shared<MainMenu>());
        }
        else if (event.node == button.get())
        {
            checkBox->setChecked(!checkBox->isChecked());
        }
        else if (event.node == fullscreenButton.get())
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
                sharedEngine->getSceneManager()->setScene(std::make_shared<MainMenu>());
                break;
            default:
                break;
        }
    }

    return true;
}
