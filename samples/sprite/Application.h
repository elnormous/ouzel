// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Application: public Noncopyable, public App
    {
    public:
        virtual ~Application();

        virtual void begin() override;

        bool handleKeyboard(const KeyboardEventPtr& event, const VoidPtr& sender) const;
        bool handleMouse(const MouseEventPtr& event, const VoidPtr& sender) const;
        bool handleTouch(const TouchEventPtr& event, const VoidPtr& sender) const;
        bool handleGamepad(const GamepadEventPtr& event, const VoidPtr& sender) const;

    protected:
        scene::LayerPtr _layer;
        scene::LayerPtr _uiLayer;

        scene::SpritePtr _sprite;
        scene::SpritePtr _witch;
        scene::ParticleSystemPtr _flame;

        gui::ButtonPtr _button;

        EventHandlerPtr _eventHandler;
    };
}
