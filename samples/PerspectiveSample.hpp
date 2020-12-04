// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef PERSPECTIVESAMPLE_HPP
#define PERSPECTIVESAMPLE_HPP

#include "audio/Effects.hpp"
#include "audio/Listener.hpp"
#include "audio/Submix.hpp"
#include "gui/Widgets.hpp"
#include "input/Cursor.hpp"
#include "scene/Animator.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class PerspectiveSample: public ouzel::scene::Scene
    {
    public:
        PerspectiveSample();

    private:
        ouzel::audio::Submix submix;
        ouzel::audio::Listener listener;

        ouzel::scene::Layer layer;
        ouzel::scene::Camera camera;
        ouzel::scene::Actor cameraActor;

        ouzel::Vector3F cameraRotation;

        ouzel::scene::SpriteRenderer floorSprite;
        ouzel::scene::Actor floor;

        ouzel::scene::SpriteRenderer characterSprite;
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
}

#endif // PERSPECTIVESAMPLE_HPP
