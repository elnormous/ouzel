// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#ifndef GAMESAMPLE_HPP
#define GAMESAMPLE_HPP

#include "ouzel.hpp"

class GameSample: public ouzel::scene::Scene
{
public:
    GameSample();

private:
    bool handleKeyboard(const ouzel::KeyboardEvent& event);
    bool handleMouse(const ouzel::MouseEvent& event);
    bool handleTouch(const ouzel::TouchEvent& event);
    bool handleGamepad(const ouzel::GamepadEvent& event);
    bool handleUI(const ouzel::UIEvent& event) const;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;
    ouzel::scene::Actor cameraActor;

    ouzel::EventHandler handler;
};

#endif // GAMESAMPLE_HPP
