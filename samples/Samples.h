// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class Samples: public ouzel::Noncopyable
{
public:
    void begin(const std::string& sample);
    void back();

    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event);

private:
    ouzel::EventHandler eventHandler;
    std::shared_ptr<ouzel::scene::Scene> mainMenu;
};
