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

MainMenu::MainMenu():
    layer(new ouzel::scene::Layer()),
    camera(new ouzel::scene::Camera()),
    menu(new ouzel::gui::Menu()),
    gitHubButton(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "GitHub", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    spritesButton(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Sprites", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    guiButton(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "GUI", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    renderTargetButton(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Render target", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    animationsButton(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Animations", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    inputButton(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Input", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    soundButton(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Sound", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK)),
    perspectiveButton(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Perspective", "arial.fnt", Color(20, 0, 0, 255), Color::BLACK, Color::BLACK))
{
    eventHandler.uiHandler = bind(&MainMenu::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&MainMenu::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    addLayer(layer.get());

    camera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera->setTargetContentSize(Size2(400.0f, 600.0f));
    layer->addChild(camera.get());

    menu.reset(new gui::Menu());
    layer->addChild(menu.get());

    gitHubButton->setPosition(Vector2(0.0f, 120.0f));
    menu->addWidget(gitHubButton.get());

    spritesButton->setPosition(Vector2(0.0f, 80.0f));
    menu->addWidget(spritesButton.get());

    guiButton->setPosition(Vector2(0.0f, 40.0f));
    menu->addWidget(guiButton.get());

    renderTargetButton->setPosition(Vector2(0.0f, 0.0f));
    menu->addWidget(renderTargetButton.get());

    animationsButton->setPosition(Vector2(0.0f, -40.0f));
    menu->addWidget(animationsButton.get());

    inputButton->setPosition(Vector2(0.0f, -80.0f));
    menu->addWidget(inputButton.get());

    soundButton->setPosition(Vector2(0.0f, -120.0f));
    menu->addWidget(soundButton.get());

    perspectiveButton->setPosition(Vector2(0.0f, -160.0f));
    menu->addWidget(perspectiveButton.get());
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
        if (event.node == gitHubButton.get())
        {
            sharedApplication->openURL("https://github.com/elnormous/ouzel");
        }
        else if (event.node == spritesButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new SpritesSample()));
        }
        else if (event.node == guiButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new GUISample()));
        }
        else if (event.node == renderTargetButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new RTSample()));
        }
        else if (event.node == animationsButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new AnimationsSample()));
        }
        else if (event.node == inputButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new InputSample()));
        }
        else if (event.node == soundButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new SoundSample()));
        }
        else if (event.node == perspectiveButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new PerspectiveSample()));
        }
    }

    return true;
}
