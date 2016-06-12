// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MainMenu.h"
#include "SpritesSample.h"
#include "GUISample.h"
#include "RTSample.h"

using namespace std;
using namespace ouzel;

MainMenu::MainMenu()
{
    eventHandler = make_shared<EventHandler>();

    eventHandler->uiHandler = bind(&MainMenu::handleUI, this, placeholders::_1, placeholders::_2, placeholders::_3);

    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);
    
    scene::LayerPtr layer = make_shared<scene::Layer>();
    addLayer(layer);

    layer->setCamera(make_shared<scene::Camera>());

    spritesButton = gui::Button::create("button.png", "button.png", "button_down.png", "", "Sprites", graphics::Color(20, 0, 0, 255), "arial.fnt");
    spritesButton->setPosition(Vector2(0.0f, 40.0f));
    layer->addChild(spritesButton);

    GUIButton = gui::Button::create("button.png", "button.png", "button_down.png", "", "GUI", graphics::Color(20, 0, 0, 255), "arial.fnt");
    GUIButton->setPosition(Vector2(0.0f, 0.0f));
    layer->addChild(GUIButton);

    renderTargetButton = gui::Button::create("button.png", "button.png", "button_down.png", "", "Render target", graphics::Color(20, 0, 0, 255), "arial.fnt");
    renderTargetButton->setPosition(Vector2(0.0f, -40.0f));
    layer->addChild(renderTargetButton);
}

MainMenu::~MainMenu()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
}

bool MainMenu::handleUI(Event::Type type, const UIEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(event);

    if (type == Event::Type::UI_CLICK_NODE)
    {
        scene::ScenePtr newScene;

        if (sender == spritesButton)
        {
            newScene = make_shared<SpritesSample>();
        }
        else if (sender == GUIButton)
        {
            newScene = make_shared<GUISample>();
        }
        else if (sender == renderTargetButton)
        {
            newScene = make_shared<RTSample>();
        }

        if (newScene)
        {
            sharedEngine->getSceneManager()->setScene(newScene);
        }
    }

    return true;
}
