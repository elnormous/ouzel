// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#ifndef PERSPECTIVESAMPLE_HPP
#define PERSPECTIVESAMPLE_HPP

#include "ouzel.hpp"

class PerspectiveSample: public ouzel::scene::Scene
{
public:
    PerspectiveSample();

private:
    bool handleUI(const ouzel::UIEvent& event);
    bool handleKeyboard(const ouzel::KeyboardEvent& event);
    bool handleMouse(const ouzel::MouseEvent& event);
    bool handleTouch(const ouzel::TouchEvent& event);
    bool handleGamepad(const ouzel::GamepadEvent& event);

    ouzel::audio::Submix submix;
    ouzel::audio::Listener listener;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;
    ouzel::scene::Actor cameraActor;

    ouzel::Vector3<float> cameraRotation;

    ouzel::scene::Sprite floorSprite;
    ouzel::scene::Actor floor;

    ouzel::scene::Sprite characterSprite;
    ouzel::scene::Actor character;
    ouzel::audio::Submix jumpSubmix;
    ouzel::audio::Voice jumpVoice;
    ouzel::audio::Panner jumpPanner;

    ouzel::scene::StaticMeshRenderer boxModel;
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

#endif // PERSPECTIVESAMPLE_HPP
