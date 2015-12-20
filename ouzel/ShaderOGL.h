// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Shader.h"

namespace ouzel
{
    class ShaderOGL: public Shader
    {
    public:
        ShaderOGL(const std::string& fragmentShader, const std::string& vertexShader);
        virtual ~ShaderOGL();
        
        GLuint getProgramId() const { return _programId; }
        
    protected:
        void checkShaderError(GLuint shader);
        
        GLuint _vertexShader;
        GLuint _fragmentShader;
        GLuint _programId;
    };
}
