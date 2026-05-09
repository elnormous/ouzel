#ifndef RTSAMPLE_HPP
#define RTSAMPLE_HPP

#include "gui/Widgets.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class RTSample: public fluxion::scene::Scene
    {
    public:
        RTSample();

    private:
        fluxion::scene::Layer layer;
        fluxion::scene::Camera camera;
        fluxion::scene::Actor cameraActor;

        fluxion::scene::Layer rtLayer;

        fluxion::scene::Camera rtCamera;
        fluxion::scene::Actor rtCameraActor;
        fluxion::scene::Camera camera1;
        fluxion::scene::Actor camera1Actor;
        fluxion::scene::Camera camera2;
        fluxion::scene::Actor camera2Actor;

        fluxion::scene::SpriteRenderer characterSprite;
        fluxion::scene::Actor rtCharacter;

        fluxion::scene::SpriteRenderer rtSprite;
        fluxion::scene::Actor rtActor;

        fluxion::EventHandler handler;

        fluxion::scene::Layer guiLayer;
        fluxion::scene::Camera guiCamera;
        fluxion::scene::Actor guiCameraActor;
        fluxion::gui::Menu menu;
        fluxion::gui::Button backButton;

        std::shared_ptr<fluxion::graphics::Texture> renderTexture;
        fluxion::graphics::Texture depthTexture;
        fluxion::graphics::RenderTarget renderTarget;
    };
}

#endif // RTSAMPLE_HPP
