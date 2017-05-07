// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MainMenu.h"
#include "SpritesSample.h"
#include "GUISample.h"
#include "RTSample.h"
#include "AnimationsSample.h"
#include "InputSample.h"
#include "SoundSample.h"
#include "PerspectiveSample.h"

using namespace std;
using namespace ouzel;

MainMenu::MainMenu(Samples& aSamples):
    samples(aSamples),
    layer(std::make_shared<ouzel::scene::Layer>()),
    camera(std::make_shared<ouzel::scene::Camera>()),
    menu(std::make_shared<ouzel::gui::Menu>()),
    gitHubButton(std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "GitHub", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    spritesButton(std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Sprites", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    guiButton(std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "GUI", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    renderTargetButton(std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Render target", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    animationsButton(std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Animations", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    inputButton(std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Input", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    soundButton(std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Sound", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    perspectiveButton(std::make_shared<gui::Button>("button.png", "button_selected.png", "button_down.png", "", "Perspective", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK))
{
    eventHandler.uiHandler = bind(&MainMenu::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&MainMenu::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    addLayer(layer);

    camera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera->setTargetContentSize(Size2(400.0f, 600.0f));
    layer->addChild(camera);

    menu = std::make_shared<gui::Menu>();
    layer->addChild(menu);

    gitHubButton->setPosition(Vector2(0.0f, 120.0f));
    menu->addWidget(gitHubButton);

    spritesButton->setPosition(Vector2(0.0f, 80.0f));
    menu->addWidget(spritesButton);

    guiButton->setPosition(Vector2(0.0f, 40.0f));
    menu->addWidget(guiButton);

    renderTargetButton->setPosition(Vector2(0.0f, 0.0f));
    menu->addWidget(renderTargetButton);

    animationsButton->setPosition(Vector2(0.0f, -40.0f));
    menu->addWidget(animationsButton);

    inputButton->setPosition(Vector2(0.0f, -80.0f));
    menu->addWidget(inputButton);

    soundButton->setPosition(Vector2(0.0f, -120.0f));
    menu->addWidget(soundButton);

    perspectiveButton->setPosition(Vector2(0.0f, -160.0f));
    menu->addWidget(perspectiveButton);
}

bool MainMenu::handleKeyboard(Event::Type type, const KeyboardEvent& event)
{
    if (event.key == ouzel::input::KeyboardKey::ESCAPE)
    {
        if (type == Event::Type::KEY_DOWN)
        {
            sharedApplication->exit();
        }

        return false;
    }

    return true;
}

bool MainMenu::handleUI(Event::Type type, const UIEvent& event)
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == gitHubButton)
        {
            sharedApplication->openURL("https://github.com/elnormous/ouzel");
        }
        else if (event.node == spritesButton)
        {
            samples.setScene(std::shared_ptr<scene::Scene>(new SpritesSample(samples)));
        }
        else if (event.node == guiButton)
        {
            samples.setScene(std::shared_ptr<scene::Scene>(new GUISample(samples)));
        }
        else if (event.node == renderTargetButton)
        {
            samples.setScene(std::shared_ptr<scene::Scene>(new RTSample(samples)));
        }
        else if (event.node == animationsButton)
        {
            samples.setScene(std::shared_ptr<scene::Scene>(new AnimationsSample(samples)));
        }
        else if (event.node == inputButton)
        {
            samples.setScene(std::shared_ptr<scene::Scene>(new InputSample(samples)));
        }
        else if (event.node == soundButton)
        {
            samples.setScene(std::shared_ptr<scene::Scene>(new SoundSample(samples)));
        }
        else if (event.node == perspectiveButton)
        {
            samples.setScene(std::shared_ptr<scene::Scene>(new PerspectiveSample(samples)));
        }
    }

    return true;
}
