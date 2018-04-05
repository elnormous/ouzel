// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class PerspectiveSample: public ouzel::scene::Scene
{
public:
    PerspectiveSample();

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event);
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event);
    bool handleMouse(ouzel::Event::Type type, const ouzel::MouseEvent& event);
    bool handleTouch(ouzel::Event::Type type, const ouzel::TouchEvent& event);
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);

    ouzel::audio::Listener listener;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;
    ouzel::scene::Actor cameraActor;

    ouzel::Vector3 cameraRotation;

    ouzel::scene::Sprite floorSprite;
    ouzel::scene::Actor floor;

    ouzel::scene::Sprite characterSprite;
    ouzel::scene::Actor character;
    ouzel::audio::Sound jumpSound;

    ouzel::scene::ModelRenderer boxModel;
    ouzel::scene::Actor box;

    std::unique_ptr<ouzel::scene::Animator> rotate;

    ouzel::EventHandler handler;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::scene::Actor guiCameraActor;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;

    ouzel::input::Cursor cursor;
};
