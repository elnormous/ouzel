#ifndef GAMESAMPLE_HPP
#define GAMESAMPLE_HPP

#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class GameSample: public fluxion::scene::Scene
    {
    public:
        GameSample();

    private:
        fluxion::scene::Layer layer;
        fluxion::scene::Camera camera;
        fluxion::scene::Actor cameraActor;

        fluxion::EventHandler handler;
    };
}

#endif // GAMESAMPLE_HPP
