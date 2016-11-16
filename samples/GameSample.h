// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class GameSample: public ouzel::scene::Scene
{
public:
    GameSample(Samples& aSamples);

private:
    Samples& samples;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;
};
