// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class Application: public ouzel::Noncopyable
{
public:
    virtual ~Application();

    void begin();

    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event, const ouzel::VoidPtr& sender) const;

private:
    ouzel::EventHandlerPtr eventHandler;
    std::shared_ptr<ouzel::scene::Scene> mainMenu;
};
