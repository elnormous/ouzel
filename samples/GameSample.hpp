// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef GAMESAMPLE_HPP
#define GAMESAMPLE_HPP

#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class GameSample: public ouzel::scene::Scene
    {
    public:
        GameSample();

    private:
        ouzel::scene::Layer layer;
        ouzel::scene::Camera camera;
        ouzel::scene::Actor cameraActor;

        ouzel::EventHandler handler;
    };
}

#endif // GAMESAMPLE_HPP
