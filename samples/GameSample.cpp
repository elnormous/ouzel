// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GameSample.h"

GameSample::GameSample(Samples& aSamples):
    samples(aSamples)
{
    addLayer(&layer);
    layer.addCamera(&camera);
}
