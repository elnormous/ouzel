// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SpritesSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

SpritesSample::SpritesSample(Samples& aSamples):
    samples(aSamples),
    backButton("button.png", "button_selected.png", "button_down.png", "", "Back", Color::BLACK, "arial.fnt")
{
    eventHandler.uiHandler = bind(&SpritesSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&SpritesSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    camera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera.setTargetContentSize(Size2(800.0f, 600.0f));
    layer.addCamera(&camera);
    addLayer(&layer);

    // character
    characterSprite.initFromFile("run.json");
    characterSprite.play(true);

    character.addComponent(&characterSprite);
    layer.addChild(&character);
    character.setPosition(Vector2(-300.0f, 0.0f));

    move.reset(new scene::Move(4.0f, Vector2(300.0f, 0.0f)));
    character.animate(move.get());

    // fire
    fireSprite.initFromFile("fire.json");
    fireSprite.setOffset(Vector2(0.0f, 20.0f));
    fireSprite.play(true);

    fireNode.addComponent(&fireSprite);
    fireNode.setPosition(Vector2(-100.0f, -140.0f));
    layer.addChild(&fireNode);

    // triangle
    triangleSprite.initFromFile("triangle.json");
    triangleNode.addComponent(&triangleSprite);
    triangleNode.setPosition(Vector2(100.0f, -140.0f));
    layer.addChild(&triangleNode);

    guiCamera.setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera.setTargetContentSize(Size2(800.0f, 600.0f));
    guiLayer.addCamera(&guiCamera);
    addLayer(&guiLayer);

    guiLayer.addChild(&menu);

    hideButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Show/hide", Color::BLACK, "arial.fnt"));
    hideButton->setPosition(Vector2(-200.0f, 200.0f));
    menu.addWidget(hideButton.get());

    wireframeButton.reset(new gui::Button("button.png", "button_selected.png", "button_down.png", "", "Wireframe", Color::BLACK, "arial.fnt"));
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
            samples.setScene(std::unique_ptr<scene::Scene>(new MainMenu(samples)));
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
                samples.setScene(std::unique_ptr<scene::Scene>(new MainMenu(samples)));
                break;
            default:
                break;
        }
    }

    return true;
}
