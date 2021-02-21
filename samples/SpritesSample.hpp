// Ouzel by Elviss Strazdins

#ifndef SPRITESSAMPLE_HPP
#define SPRITESSAMPLE_HPP

#include "gui/Widgets.hpp"
#include "scene/Animators.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class SpritesSample: public ouzel::scene::Scene
    {
    public:
        SpritesSample();

    private:
        ouzel::scene::Layer layer;
        ouzel::scene::Camera camera;
        ouzel::scene::Actor cameraActor;

        ouzel::scene::SpriteRenderer characterSprite;
        ouzel::scene::Actor character;
        ouzel::scene::SpriteRenderer fireSprite;
        ouzel::scene::Actor fireActor;
        ouzel::scene::SpriteRenderer triangleSprite;
        ouzel::scene::Actor triangleActor;

        std::unique_ptr<ouzel::scene::Animator> move;

        ouzel::EventHandler handler;

        ouzel::gui::Button hideButton;
        ouzel::gui::Button wireframeButton;

        ouzel::scene::Layer guiLayer;
        ouzel::scene::Camera guiCamera;
        ouzel::scene::Actor guiCameraActor;
        ouzel::gui::Menu menu;
        ouzel::gui::Button backButton;
    };
}

#endif // SPRITESSAMPLE_HPP
