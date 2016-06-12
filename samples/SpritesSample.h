//
//  SpritesSample.h
//  samples
//
//  Created by Elviss Strazdins on 13/06/16.
//  Copyright (c) 2016 Bool Games. All rights reserved.
//

#pragma once

class SpritesSample: public ouzel::scene::Scene
{
public:
    SpritesSample();
    virtual ~SpritesSample();

    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event, const ouzel::VoidPtr& sender) const;
    bool handleMouse(ouzel::Event::Type type, const ouzel::MouseEvent& event, const ouzel::VoidPtr& sender) const;
    bool handleTouch(ouzel::Event::Type type, const ouzel::TouchEvent& event, const ouzel::VoidPtr& sender) const;
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event, const ouzel::VoidPtr& sender) const;
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event, const ouzel::VoidPtr& sender) const;

private:
    ouzel::scene::LayerPtr layer;
    ouzel::scene::LayerPtr uiLayer;

    ouzel::scene::NodePtr character;
    ouzel::scene::NodePtr witch;
    ouzel::scene::NodePtr flame;

    ouzel::gui::ButtonPtr button;

    ouzel::EventHandlerPtr eventHandler;
};
