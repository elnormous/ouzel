// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GUISample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

GUISample::GUISample(Samples& aSamples):
    samples(aSamples),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", graphics::Color::BLACK, "arial.fnt")
{
    eventHandler.uiHandler = bind(&GUISample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&GUISample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    guiLayer.addCamera(&guiCamera);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    button.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Button", graphics::Color::RED, "arial.fnt"));
    button->setPosition(Vector2(-200.0f, 100.0f));
    menu.addWidget(button.get());

    checkBox.reset(new gui::CheckBox("checkbox.png", "", "", "", "tick.png"));
    checkBox->setPosition(Vector2(-100.0f, 100.0f));
    guiLayer.addChild(checkBox.get());

    fullscreenButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Fullscreen", graphics::Color::BLACK, "arial.fnt"));
    fullscreenButton->setPosition(Vector2(-200.0f, 40.0f));
    menu.addWidget(fullscreenButton.get());

    label1.reset(new gui::Label("arial.fnt", "checkbox"));
    label1->setColor(graphics::Color::CYAN);
    label1->setPosition(Vector2(-60.0f, 100.0f));
    guiLayer.addChild(label1.get());

    sharedEngine->getLocalization()->addLanguage("latvian", "lv.mo");
    sharedEngine->getLocalization()->setLanguage("latvian");

    label2.reset(new gui::Label("ArialBlack.fnt", sharedEngine->getLocalization()->getString("Ouzel")));
    label2->setPosition(Vector2(10.0f, 0.0f));
    guiLayer.addChild(label2.get());

    label3.reset(new gui::Label("ArialBlack.fnt", "UTF-8 ĀāČč\nNew line", Vector2(0.0f, 0.5f)));
    label3->setColor(graphics::Color::BLUE);
    label3->setPosition(Vector2(-100.0f, -100.0f));
    label3->setScale(Vector2(0.5f, 0.5f));
    guiLayer.addChild(label3.get());

    backButton.setPosition(Vector2(-200.0f, -200.0f));
    menu.addWidget(&backButton);
}

bool GUISample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == &backButton)
        {
            samples.setSample("");
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
                samples.setSample("");
                break;
            default:
                break;
        }
    }

    return true;
}
