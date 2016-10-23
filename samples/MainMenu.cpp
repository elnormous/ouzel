// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MainMenu.h"

using namespace std;
using namespace ouzel;

MainMenu::MainMenu(Samples& aSamples):
    samples(aSamples)
{
    eventHandler.uiHandler = bind(&MainMenu::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&MainMenu::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    layer.reset(new scene::Layer());
    addLayer(layer.get());

    camera.reset(new scene::Camera());
    layer->addCamera(camera.get());

    menu.reset(new gui::Menu());
    layer->addChild(menu.get());

    spritesButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Sprites", graphics::Color(20, 0, 0, 255), "arial.fnt"));
    spritesButton->setPosition(Vector2(0.0f, 80.0f));
    menu->addWidget(spritesButton.get());

    GUIButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "GUI", graphics::Color(20, 0, 0, 255), "arial.fnt"));
    GUIButton->setPosition(Vector2(0.0f, 40.0f));
    menu->addWidget(GUIButton.get());

    renderTargetButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Render target", graphics::Color(20, 0, 0, 255), "arial.fnt"));
    renderTargetButton->setPosition(Vector2(0.0f, 0.0f));
    menu->addWidget(renderTargetButton.get());

    animationsButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Animations", graphics::Color(20, 0, 0, 255), "arial.fnt"));
    animationsButton->setPosition(Vector2(0.0f, -40.0f));
    menu->addWidget(animationsButton.get());

    inputButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Input", graphics::Color(20, 0, 0, 255), "arial.fnt"));
    inputButton->setPosition(Vector2(0.0f, -80.0f));
    menu->addWidget(inputButton.get());

    soundButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Sound", graphics::Color(20, 0, 0, 255), "arial.fnt"));
    soundButton->setPosition(Vector2(0.0f, -120.0f));
    menu->addWidget(soundButton.get());
}

bool MainMenu::handleKeyboard(Event::Type type, const KeyboardEvent& event)
{
    if (event.key == ouzel::input::KeyboardKey::ESCAPE)
    {
        if (type == Event::Type::KEY_DOWN)
        {
            sharedEngine->exit();
        }

        return false;
    }

    return true;
}

bool MainMenu::handleUI(Event::Type type, const UIEvent& event)
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == spritesButton.get())
        {
            samples.setSample("sprites");
        }
        else if (event.node == GUIButton.get())
        {
            samples.setSample("gui");
        }
        else if (event.node == renderTargetButton.get())
        {
            samples.setSample("render_target");
        }
        else if (event.node == animationsButton.get())
        {
            samples.setSample("animations");
        }
        else if (event.node == inputButton.get())
        {
            samples.setSample("input");
        }
        else if (event.node == soundButton.get())
        {
            samples.setSample("sound");
        }
    }

    return true;
}
