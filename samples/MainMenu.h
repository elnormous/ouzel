// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class MainMenu: public ouzel::scene::Scene
{
public:
    MainMenu();
    virtual ~MainMenu();

    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event, const ouzel::VoidPtr& sender) const;

private:
    ouzel::EventHandlerPtr eventHandler;
    ouzel::gui::ButtonPtr button;
};
