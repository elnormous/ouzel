// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef GAMESAMPLE_HPP
#define GAMESAMPLE_HPP

#include "ouzel.hpp"

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

#endif // GAMESAMPLE_HPP
