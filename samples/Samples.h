// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class Samples: public ouzel::Noncopyable
{
public:
    void begin(const std::string& sample);

    void setScene(std::unique_ptr<ouzel::scene::Scene>&& scene);
    
    std::unique_ptr<ouzel::scene::Scene> currentScene;
};
