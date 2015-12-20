//
//  Shader.h
//  Ouzel
//
//  Created by Elviss Strazdins on 14/03/15.
//  Copyright (c) 2015 Elviss. All rights reserved.
//

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
        
        GLuint getProgram() const { return _program; }
        
    private:
        void checkShaderError(GLuint shader);
        
        GLuint _vertexShader;
        GLuint _fragmentShader;
        GLuint _program;
    };
}
