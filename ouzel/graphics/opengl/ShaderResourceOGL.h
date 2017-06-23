// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#if OUZEL_SUPPORTS_OPENGLES
    #define GL_GLEXT_PROTOTYPES 1
    #include "GLES/gl.h"
    #include "GLES2/gl2.h"
    #include "GLES2/gl2ext.h"
    #include "GLES3/gl3.h"
#else
    #define GL_GLEXT_PROTOTYPES 1
    #include "GL/glcorearb.h"
    #include "GL/glext.h"
#endif

#include "graphics/ShaderResource.h"

namespace ouzel
{
    namespace graphics
    {
        class ShaderResourceOGL: public ShaderResource
        {
        public:
            ShaderResourceOGL();
            virtual ~ShaderResourceOGL();

            struct Location
            {
                GLint location;
                DataType dataType;
            };

            const std::vector<Location>& getPixelShaderConstantLocations() const { return pixelShaderConstantLocations; }
            const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

            GLuint getProgramId() const { return programId; }

        protected:
            virtual bool upload() override;

            void printShaderMessage(GLuint shaderId);
            void printProgramMessage();

            GLuint pixelShaderId = 0;
            GLuint vertexShaderId = 0;
            GLuint programId = 0;

            std::vector<Location> pixelShaderConstantLocations;
            std::vector<Location> vertexShaderConstantLocations;
        };
    } // namespace graphics
} // namespace ouzel

#endif
