// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef RTSAMPLE_HPP
#define RTSAMPLE_HPP

#include "gui/Widgets.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class RTSample: public ouzel::scene::Scene
    {
    public:
        RTSample();

    private:
        ouzel::scene::Layer layer;
        ouzel::scene::Camera camera;
        ouzel::scene::Actor cameraActor;

        ouzel::scene::Layer rtLayer;

        ouzel::scene::Camera rtCamera;
        ouzel::scene::Actor rtCameraActor;
        ouzel::scene::Camera camera1;
        ouzel::scene::Actor camera1Actor;
        ouzel::scene::Camera camera2;
        ouzel::scene::Actor camera2Actor;

        ouzel::scene::SpriteRenderer characterSprite;
        ouzel::scene::Actor rtCharacter;

        ouzel::scene::SpriteRenderer rtSprite;
        ouzel::scene::Actor rtActor;

        ouzel::EventHandler handler;

        ouzel::scene::Layer guiLayer;
        ouzel::scene::Camera guiCamera;
        ouzel::scene::Actor guiCameraActor;
        ouzel::gui::Menu menu;
        ouzel::gui::Button backButton;

        std::shared_ptr<ouzel::graphics::Texture> renderTexture;
        ouzel::graphics::Texture depthTexture;
        ouzel::graphics::RenderTarget renderTarget;
    };
}

#endif // RTSAMPLE_HPP
