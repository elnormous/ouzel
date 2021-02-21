// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef ANIMATIONSSAMPLE_HPP
#define ANIMATIONSSAMPLE_HPP

#include <memory>
#include "gui/Widgets.hpp"
#include "scene/Animators.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"
#include "scene/ShapeRenderer.hpp"
#include "scene/SpriteRenderer.hpp"

namespace samples
{
    class AnimationsSample: public ouzel::scene::Scene
    {
    public:
        AnimationsSample();

    private:
        ouzel::scene::Layer layer;
        ouzel::scene::Camera camera;
        ouzel::scene::Actor cameraActor;

        ouzel::scene::ShapeRenderer shapeDrawable;
        ouzel::scene::Actor drawActor;

        std::unique_ptr<ouzel::scene::Shake> shake;

        ouzel::scene::SpriteRenderer witchSprite;
        ouzel::scene::Actor witch;

        std::unique_ptr<ouzel::scene::Animator> witchScale;
        std::unique_ptr<ouzel::scene::Animator> witchFade;
        std::unique_ptr<ouzel::scene::Animator> witchRotate;
        std::unique_ptr<ouzel::scene::Animator> witchRepeat;
        std::unique_ptr<ouzel::scene::Animator> witchParallel;
        std::unique_ptr<ouzel::scene::Animator> witchSequence;

        ouzel::scene::SpriteRenderer ballSprite;
        ouzel::scene::Actor ball;

        std::unique_ptr<ouzel::scene::Animator> ballDelay;
        std::unique_ptr<ouzel::scene::Animator> ballEase;
        std::unique_ptr<ouzel::scene::Animator> ballMove;
        std::unique_ptr<ouzel::scene::Animator> ballSequence;

        ouzel::EventHandler handler;

        ouzel::scene::Layer guiLayer;
        ouzel::scene::Camera guiCamera;
        ouzel::scene::Actor guiCameraActor;
        ouzel::gui::Menu menu;
        ouzel::gui::Button backButton;
    };
}

#endif // ANIMATIONSSAMPLE_HPP
