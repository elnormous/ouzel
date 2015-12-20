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
        Shader(Renderer* renderer);
        virtual ~Shader();
        
        virtual bool loadFromFiles(const std::string& fragmentShader, const std::string& vertexShader);
        virtual bool loadFromStrings(const std::string& fragmentShader, const std::string& vertexShader);
        
        virtual uint32_t getPixelShaderConstantId(const std::string& name);
        virtual uint32_t getVertexShaderConstantId(const std::string& name);
        
    protected:
        std::string _fragmentShaderFilename;
        std::string _vertexShaderFilename;
        Renderer* _renderer;
    };
}
