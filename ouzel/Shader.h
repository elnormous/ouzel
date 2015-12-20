// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class Shader: public Noncopyable, public ReferenceCounted
    {
    public:
        Shader(const std::string& fragmentShader, const std::string& vertexShader);
        virtual ~Shader();
        
    protected:
        std::string _fragmentShader;
        std::string _vertexShader;
    };
}
