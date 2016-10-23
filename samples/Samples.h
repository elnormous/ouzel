// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class Samples: public ouzel::Noncopyable
{
public:
    void begin(const std::string& sample);

    void setSample(const std::string& sample);
    
    std::unique_ptr<ouzel::scene::Scene> currentScene;
};
