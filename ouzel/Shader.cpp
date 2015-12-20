// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Shader.h"
#include "Utils.h"

namespace ouzel
{
    Shader::Shader(const std::string& fragmentShader, const std::string& vertexShader)
    {
        _fragmentShader = fragmentShader;
        _vertexShader = vertexShader;
    }

    Shader::~Shader()
    {
        
    }
}
