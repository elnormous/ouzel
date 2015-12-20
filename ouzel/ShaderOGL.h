// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#ifdef OUZEL_PLATFORM_OSX
#include <OpenGL/gl3.h>
#endif

#include "Shader.h"

namespace ouzel
{
    class ShaderOGL: public Shader
    {
    public:
        ShaderOGL(const std::string& fragmentShader, const std::string& vertexShader, Renderer* renderer);
        virtual ~ShaderOGL();
        
        virtual bool loadFromFiles(const std::string& fragmentShader, const std::string& vertexShader) override;
        virtual bool loadFromStrings(const std::string& fragmentShader, const std::string& vertexShader) override;
        
        GLuint getProgramId() const { return _programId; }
        
        virtual uint32_t getPixelShaderConstantId(const std::string& name) override;
        virtual uint32_t getVertexShaderConstantId(const std::string& name) override;
        
    protected:
        bool checkShaderError(GLuint shader);
        
        GLuint _vertexShader;
        GLuint _fragmentShader;
        GLuint _programId;
    };
}
