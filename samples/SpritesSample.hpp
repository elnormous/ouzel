#ifndef SPRITESSAMPLE_HPP
#define SPRITESSAMPLE_HPP

#include "gui/Widgets.hpp"
#include "scene/Animators.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class SpritesSample: public fluxion::scene::Scene
    {
    public:
        SpritesSample();

    private:
        fluxion::scene::Layer layer;
        fluxion::scene::Camera camera;
        fluxion::scene::Actor cameraActor;

        fluxion::scene::SpriteRenderer characterSprite;
        fluxion::scene::Actor character;
        fluxion::scene::SpriteRenderer fireSprite;
        fluxion::scene::Actor fireActor;
        fluxion::scene::SpriteRenderer triangleSprite;
        fluxion::scene::Actor triangleActor;

        fluxion::scene::Move move;

        fluxion::EventHandler handler;

        fluxion::gui::Button hideButton;
        fluxion::gui::Button wireframeButton;

        fluxion::scene::Layer guiLayer;
        fluxion::scene::Camera guiCamera;
        fluxion::scene::Actor guiCameraActor;
        fluxion::gui::Menu menu;
        fluxion::gui::Button backButton;
    };
}

#endif // SPRITESSAMPLE_HPP
