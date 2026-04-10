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
    class AnimationsSample: public fluxion::scene::Scene
    {
    public:
        AnimationsSample();

    private:
        fluxion::scene::Layer layer;
        fluxion::scene::Camera camera;
        fluxion::scene::Actor cameraActor;

        fluxion::scene::ShapeRenderer shapeDrawable;
        fluxion::scene::Actor drawActor;

        std::unique_ptr<fluxion::scene::Shake> shake;

        fluxion::scene::SpriteRenderer witchSprite;
        fluxion::scene::Actor witch;

        std::unique_ptr<fluxion::scene::Animator> witchScale;
        std::unique_ptr<fluxion::scene::Animator> witchFade;
        std::unique_ptr<fluxion::scene::Animator> witchRotate;
        std::unique_ptr<fluxion::scene::Animator> witchRepeat;
        std::unique_ptr<fluxion::scene::Animator> witchParallel;
        std::unique_ptr<fluxion::scene::Animator> witchSequence;

        fluxion::scene::SpriteRenderer ballSprite;
        fluxion::scene::Actor ball;

        std::unique_ptr<fluxion::scene::Animator> ballDelay;
        std::unique_ptr<fluxion::scene::Animator> ballEase;
        std::unique_ptr<fluxion::scene::Animator> ballMove;
        std::unique_ptr<fluxion::scene::Animator> ballSequence;

        fluxion::EventHandler handler;

        fluxion::scene::Layer guiLayer;
        fluxion::scene::Camera guiCamera;
        fluxion::scene::Actor guiCameraActor;
        fluxion::gui::Menu menu;
        fluxion::gui::Button backButton;
    };
}

#endif // ANIMATIONSSAMPLE_HPP
