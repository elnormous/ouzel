// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SpritesSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

SpritesSample::SpritesSample(Samples& aSamples):
    samples(aSamples),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", graphics::Color::BLACK, "arial.fnt")
{
    eventHandler.uiHandler = bind(&SpritesSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&SpritesSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    addLayer(&layer);
    layer.addCamera(&camera);

    // character
    characterSprite.reset(new scene::Sprite("run.json"));
    characterSprite->play(true);

    character.addComponent(characterSprite.get());
    layer.addChild(&character);
    character.setPosition(Vector2(-300.0f, 0.0f));

    move.reset(new scene::Move(4.0f, Vector2(300.0f, 0.0f)));
    character.animate(move.get());

    // fire
    fireSprite.reset(new scene::Sprite("fire.json"));
    fireSprite->setOffset(Vector2(0.0f, 20.0f));
    fireSprite->play(true);

    fireNode.addComponent(fireSprite.get());
    fireNode.setPosition(Vector2(-100.0f, -140.0f));
    layer.addChild(&fireNode);

    // triangle
    triangleSprite.reset(new scene::Sprite("triangle.json"));
    triangleNode.addComponent(triangleSprite.get());
    triangleNode.setPosition(Vector2(100.0f, -140.0f));
    layer.addChild(&triangleNode);


    guiLayer.addCamera(&guiCamera);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    hideButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Show/hide", graphics::Color::BLACK, "arial.fnt"));
    hideButton->setPosition(Vector2(-200.0f, 200.0f));
    menu.addWidget(hideButton.get());

    wireframeButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Wireframe", graphics::Color::BLACK, "arial.fnt"));
    wireframeButton->setPosition(Vector2(-200.0f, 160.0f));
    menu.addWidget(wireframeButton.get());

    backButton.setPosition(Vector2(-200.0f, -200.0f));
    menu.addWidget(&backButton);
}

bool SpritesSample::handleUI(Event::Type type, const UIEvent& event)
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == &backButton)
        {
            samples.setSample("");
        }
        else if (event.node == hideButton.get())
        {
            character.setHidden(!character.isHidden());
        }
        else if (event.node == wireframeButton.get())
        {
            camera.setWireframe(!camera.getWireframe());
        }
    }

    return true;
}

bool SpritesSample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
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
