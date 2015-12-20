// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class Renderer;
    
    class Shader: public Noncopyable, public ReferenceCounted
    {
    public:
        Shader(const std::string& fragmentShader, const std::string& vertexShader, Renderer* renderer);
        virtual ~Shader();
        
        virtual uint32_t getPixelShaderConstantId(const std::string& name);
        virtual uint32_t getVertexShaderConstantId(const std::string& name);
        
    protected:
        std::string _fragmentShader;
        std::string _vertexShader;
        Renderer* _renderer;
    };
}
