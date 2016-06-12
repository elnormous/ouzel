// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GUISample.h"

using namespace std;
using namespace ouzel;

GUISample::GUISample()
{
    scene::LayerPtr layer = make_shared<scene::Layer>();
    addLayer(layer);
    layer->setCamera(make_shared<scene::Camera>());

    button = gui::Button::create("button.png", "button.png", "button_down.png", "", "", graphics::Color(), "");
    button->setPosition(Vector2(-200.0f, 200.0f));
    layer->addChild(button);
    
    checkBox = gui::CheckBox::create("checkbox.png", "", "", "", "tick.png");
    checkBox->setPosition(Vector2(-100.0f, 200.0f));
    layer->addChild(checkBox);
}
