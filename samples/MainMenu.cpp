// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MainMenu.h"

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

    button = gui::Button::create("button.png", "button.png", "button_down.png", "", "", graphics::Color(), "");

    layer->addChild(button);
}

MainMenu::~MainMenu()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
}

bool MainMenu::handleUI(Event::Type type, const UIEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(event);

    if (type == Event::Type::UI_CLICK_NODE && sender == button)
    {
        // change scene
    }

    return true;
}
