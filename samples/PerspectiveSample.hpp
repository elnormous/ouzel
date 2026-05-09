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
    class PerspectiveSample: public fluxion::scene::Scene
    {
    public:
        PerspectiveSample();

    private:
        void updateArrowPosition();

        fluxion::audio::Submix submix;
        fluxion::audio::Listener listener;

        fluxion::scene::Layer layer;
        fluxion::scene::Camera camera;
        fluxion::scene::Actor cameraActor;

        fluxion::math::Vector<float, 3> cameraRotation{};

        fluxion::scene::SpriteRenderer floorSprite;
        fluxion::scene::Actor floor;

        fluxion::scene::SpriteRenderer characterSprite;
        fluxion::scene::Actor character;
        fluxion::audio::Submix jumpSubmix;
        fluxion::audio::Voice jumpVoice;
        fluxion::audio::Panner jumpPanner;

        fluxion::scene::StaticMeshRenderer boxModel;
        fluxion::scene::Actor box;

        fluxion::scene::SpriteRenderer arrowSprite;
        fluxion::scene::Actor arrow;

        fluxion::scene::Rotate rotate;

        fluxion::EventHandler handler;

        fluxion::scene::Layer guiLayer;
        fluxion::scene::Camera guiCamera;
        fluxion::scene::Actor guiCameraActor;
        fluxion::gui::Menu menu;
        fluxion::gui::Button backButton;

        fluxion::input::Cursor cursor;
    };
}

#endif // PERSPECTIVESAMPLE_HPP
